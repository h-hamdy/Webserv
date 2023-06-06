#include <iostream>
#include <sstream>
#include <string>

// Function to parse a chunked body from a string
std::string parseChunkedBody(const std::string& body) {
    std::istringstream input(body);
    std::ostringstream bodyStream;

    while (true) {
        // Read the chunk size
        std::string line;
        std::getline(input, line);
        if (line.empty()) {
            break; // End of chunks
        }

        // Convert the chunk size from hexadecimal to integer
        std::istringstream sizeStream(line);
        size_t chunkSize = 0;
        sizeStream >> std::hex >> chunkSize;

        // Read the chunk data
        std::string chunkData;
        chunkData.resize(chunkSize);
        input.read(&chunkData[0], chunkSize);

        // Read and discard the CRLF after the chunk data
        std::getline(input, line);

        // Append the chunk data to the body stream
        bodyStream << chunkData;
    }

    return bodyStream.str();
}

int main() {
    // Assuming you have the chunked body stored as a string
    std::string requestBody = "4\r\ndata\r\n5\r\nchunk\r\n0\r\n\r\n";

    // Parse the chunked body
    std::string parsedBody = parseChunkedBody(requestBody);

    // Process the parsed body as needed
    std::cout << "Parsed request body: " << parsedBody << std::endl;

    return 0;
}

