#include <iostream>
#include <string>

int main() {
    std::string request = "GET /index.html HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "Content-Type: application/json\r\n"
                          "\r\n" // Empty line indicating end of headers
                          "Request body goes here...";

    // Find the position of the empty line
    size_t emptyLinePos = request.find("\r\n\r\n");

    if (emptyLinePos != std::string::npos) {
        // Headers found before empty line
        std::string headers = request.substr(0, emptyLinePos);
        std::cout << "Headers:\n" << headers << std::endl;

        // Body starts after the empty line
        std::string body = request.substr(emptyLinePos + 4);
        std::cout << "Body:\n" << body << std::endl;
    } else {
        std::cerr << "Invalid request: No empty line found" << std::endl;
        return 1;
    }

    return 0;
}
