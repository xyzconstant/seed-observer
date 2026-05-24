#ifndef NETWORK_H
#define NETWORK_H

#include <optional>
#include <string>
#include <vector>

class Network
{
public:
    enum Value {
        Mainnet,
        Testnet,
        Testnet4
    };

    Network(Value v) : m_value(v) {}

    static std::optional<Network> from_string(std::string_view sv);

    std::vector<std::string> seeds() const;
    unsigned short default_port() const;
    std::string_view display_name() const;

private:
    Value m_value;
};

#endif
