#include "netaddress.h"
#include "network.h"
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>
#include <poll.h>

#include <atomic>
#include <csignal>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <ostream>
#include <vector>

struct Args {
    Network network = Network(Network::Mainnet);
};

Network g_network(Network::Mainnet);


[[noreturn]] void usage(char* prog, int status)
{
    std::ostream& out = (status == 0) ? std::cout : std::cerr;
    out << "Usage: " << prog << " [options]\n"
        << "\n"
        << "Monitor Bitcoin Core DNS seeds.\n"
        << "\n"
        << "Options:\n"
        << "  -n, --network <net>  mainnet|testnet|testnet4 (default: mainnet)\n"
        << "  -h, --help           Show this help and exit\n";
    exit(status);
}

Args parse_cli(int argc, char* argv[])
{
    Args args;

    static const struct option long_opts[] = {
        {"network", required_argument, nullptr, 'n'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0},
    };

    int c;
    while ((c = getopt_long(argc, argv, "n:h", long_opts, nullptr)) != -1) {
        switch (c) {
        case 'n': {
            auto net = Network::from_string(optarg);
            if (!net) {
                std::cerr << "error: unknown network '" << optarg << "'\n";
                usage(argv[0], EXIT_FAILURE);
            }
            args.network = *net;
            break;
        }
        case 'h':
            usage(argv[0], EXIT_SUCCESS);
        case '?':
        default:
            usage(argv[0], EXIT_FAILURE);
        }
    }

    return args;
}

std::atomic<bool> g_shutdown{false};

void handle_sigterm(int) { g_shutdown = true; }

void init_signals()
{
    std::signal(SIGTERM, handle_sigterm);
    std::signal(SIGINT, handle_sigterm);
}


struct ProbeResult {
    std::string seed;
    bool resolved = false;
    int error = 0;                  // EAI_* code when !resolved
    std::vector<NetAddr> addresses; // populated when resolved
};

std::vector<ProbeResult> probe_all()
{
    std::vector<ProbeResult> results;

    for (const auto& seed : g_network.seeds()) {
        if (g_shutdown) break;

        ProbeResult r;
        r.seed = seed;

        struct addrinfo hints{};
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_family = AF_UNSPEC;
        hints.ai_flags = AI_ADDRCONFIG;

        struct addrinfo* res = nullptr;
        const int n_err{getaddrinfo(seed.c_str(), nullptr, &hints, &res)};
        if (n_err != 0) {
            std::cout << seed << ": resolved failed: " << gai_strerror(n_err) << "\n";
            r.error = n_err;
            results.push_back(std::move(r));
            continue;
        }

        r.resolved = true;
        std::cout << "Seed " << seed << " resolved, iterating over its IPs" << "\n";
        for (auto* p = res; p && !g_shutdown; p = p->ai_next) {
            NetAddr a(p->ai_addr);
            if (!a.is_valid()) continue;

            int rc = a.try_connect(g_network.default_port(), 2000);
            std::cout << "  " << a.to_string() << ": "
                      << (rc == 0 ? "connected" : strerror(rc)) << "\n";
            if (rc == 0) r.addresses.push_back(a);
        }

        freeaddrinfo(res);
        results.push_back(std::move(r));
    }

    return results;
}


int main(int argc, char* argv[])
{
    Args args = parse_cli(argc, argv);

    g_network = args.network;

    init_signals();

    std::cout << "Checking seeds for " << g_network.display_name() << " network...\n";
    auto results = probe_all();

    return 0;
}
