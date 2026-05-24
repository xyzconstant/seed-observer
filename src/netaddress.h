#ifndef NETADDRESS_H
#define NETADDRESS_H

#include <arpa/inet.h>

#include <array>
#include <cstring>
#include <string>

class NetAddr
{
public:
    NetAddr() = default;
    explicit NetAddr(const sockaddr* s);

    bool is_ipv4() const { return m_family == AF_INET; }
    bool is_ipv6() const { return m_family == AF_INET6; }
    bool is_valid() const { return m_family != AF_UNSPEC; }

    std::string to_string() const;
    socklen_t to_sockaddr(sockaddr_storage& ss, uint16_t port) const;
    int try_connect(uint16_t port, int timeout_ms) const;

    bool operator==(const NetAddr& o) const
    {
        return m_family == o.m_family && m_addr == o.m_addr;
    }

    bool operator<(const NetAddr& o) const
    {
        if (m_family != o.m_family) return m_family < o.m_family;
        return m_addr < o.m_addr;
    }


private:
    int m_family = AF_UNSPEC;
    std::array<uint8_t, 16> m_addr{};
};

#endif
