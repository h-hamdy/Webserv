#include "socket.hpp"

int main(int ac, char** av) {
    if (ac < 2) {
        return 1;
    }
    (void)av;
    Socket s;
    s.setupServer(8080,"0.0.0.0");
    s.~Socket();

    return 0;
}