#include "network.h"

std::optional<Network> Network::from_string(std::string_view sv)
{
    if (sv == "main" || sv == "mainnet")
        return Network(Mainnet);
    if (sv == "test" || sv == "testnet")
        return Network(Testnet);
    if (sv == "testnet4")
        return Network(Testnet4);
    return std::nullopt;
}


std::vector<std::string> Network::seeds() const
{
    switch (m_value) {
    case Mainnet:
        return {"seed.bitcoin.sipa.be", "dnsseed.bluematt.me",
                "seed.bitcoin.jonasschnelli.ch", "seed.btc.petertodd.net",
                "seed.bitcoin.sprovoost.nl", "dnsseed.emzy.de",
                "seed.bitcoin.wiz.biz", "seed.mainnet.achownodes.xyz"};
    case Testnet:
        return {"testnet-seed.bitcoin.jonasschnelli.ch", "seed.tbtc.petertodd.net",
                "seed.testnet.bitcoin.sprovoost.nl", "testnet-seed.bluematt.me",
                "seed.testnet.achownodes.xyz"};
    case Testnet4:
        return {"seed.testnet4.bitcoin.sprovoost.nl", "seed.testnet4.wiz.biz"};
    }
    return {};
}

unsigned short Network::default_port() const
{
    switch (m_value) {
    case Mainnet:
        return 8333;
    case Testnet:
        return 18333;
    case Testnet4:
        return 48333;
    }
    return 8333;
}

std::string_view Network::display_name() const
{
    switch (m_value) {
    case Mainnet:
        return "Mainnet";
    case Testnet:
        return "Testnet";
    case Testnet4:
        return "Testnet4";
    }
    return "";
}
