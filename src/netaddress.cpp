#include "netaddress.h"
#include <sys/fcntl.h>
#include <sys/poll.h>
#include <unistd.h>

NetAddr::NetAddr(const sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        m_family = AF_INET;
        auto* s = reinterpret_cast<const sockaddr_in*>(sa);
        std::memcpy(m_addr.data(), &s->sin_addr, 4);
    } else if (sa->sa_family == AF_INET6) {
        m_family = AF_INET6;
        auto* s = reinterpret_cast<const sockaddr_in6*>(sa);
        std::memcpy(m_addr.data(), &s->sin6_addr, 16);
    }
}

std::string NetAddr::to_string() const
{
    char buf[INET6_ADDRSTRLEN] = {};
    if (inet_ntop(m_family, m_addr.data(), buf, INET6_ADDRSTRLEN) != nullptr) {
        return std::string(buf);
    }
    return {};
}

socklen_t NetAddr::to_sockaddr(sockaddr_storage& ss, uint16_t port) const
{
    std::memset(&ss, 0, sizeof(ss));
    if (m_family == AF_INET) {
        auto* in = reinterpret_cast<sockaddr_in*>(&ss);
        in->sin_family = AF_INET;
        in->sin_port = htons(port);
        std::memcpy(&in->sin_addr, m_addr.data(), 4);
        return sizeof(sockaddr_in);
    } else if (m_family == AF_INET6) {
        auto* in6 = reinterpret_cast<sockaddr_in6*>(&ss);
        in6->sin6_family = AF_INET6;
        in6->sin6_port = htons(port);
        std::memcpy(&in6->sin6_addr, m_addr.data(), 16);
        return sizeof(sockaddr_in6);
    }
    return 0; // invalid
}

int NetAddr::try_connect(uint16_t port, int timeout_ms) const
{
    sockaddr_storage ss;
    socklen_t len = to_sockaddr(ss, port);
    if (len == 0) return EINVAL;

    int fd = socket(ss.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) return errno;

    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    int result;
    if (connect(fd, reinterpret_cast<sockaddr*>(&ss), len) == 0) {
        result = 0;
    } else if (errno == EINPROGRESS) {
        struct pollfd pfd{fd, POLLOUT, 0};
        int pr = poll(&pfd, 1, timeout_ms);
        if (pr == 0) {
            result = ETIMEDOUT;
        } else if (pr > 0) {
            int err = 0;
            socklen_t l = sizeof(err);
            getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &l);
            result = err;
        } else {
            result = errno;
        }
    } else {
        result = errno;
    }

    close(fd);
    return result;
}
