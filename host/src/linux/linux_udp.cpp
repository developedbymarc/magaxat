#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <memory>
#include <stdexcept>

#include <magaxat/magaxat.hpp>

class LinuxUdpListener : public magaxat::IUdpListener {
public:
    LinuxUdpListener() { _port = setup(); }

    ~LinuxUdpListener() { close(_socket); }

    ssize_t receive(uint8_t *buffer, size_t maxLen) override {
        return recv(_socket, buffer, maxLen, 0);
    }

private:
    int _socket = -1;
    std::uint16_t _port = -1;

    int setup() {
        struct sockaddr_in addr{};

        // prepare socket on ipv4 with UDP protocol
        if ((_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
            std::runtime_error("preparing socket");

        addr.sin_family = AF_INET;
        addr.sin_port = htons(0);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(_socket, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1) {
            close(_socket);
            std::runtime_error("preparing socket");
        }

        socklen_t addr_size = sizeof(addr);

        if (getsockname(_socket, reinterpret_cast<struct sockaddr *>(&addr), &addr_size) == -1) {
            close(_socket);
            std::runtime_error("getting socket name");
        }

        return ntohs(addr.sin_port);
    }
};

std::unique_ptr<magaxat::IUdpListener> magaxat::createUdpListener() {
    return std::make_unique<LinuxUdpListener>();
}
