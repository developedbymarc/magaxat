#include <cstdio>
#include <iostream>
#include <memory>

#include <magaxat/magaxat.hpp>

int main(int argc, char *argv[]) {
    std::unique_ptr<magaxat::IUdpListener> udpListener;
    std::unique_ptr<magaxat::IVirtualTablet> tablet;

    try {
        udpListener = magaxat::createUdpListener();
        tablet = magaxat::createVirtualTablet();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::printf("Initialized %s host - v%i.%i.%i-%s-%s running protocol v%i\n",
                magaxat::metadata::name, magaxat::metadata::version_major,
                magaxat::metadata::version_minor, magaxat::metadata::version_patch,
                magaxat::metadata::git_branch, magaxat::metadata::git_commit,
                magaxat::metadata::protocol_version);
}
