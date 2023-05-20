#include"lexer.hpp"


// bool isValidMethod(std::string const &str) {
//   return (str == "GET" ||
//           str == "POST" ||
//           str == "HEAD" ||
//           str == "PUT" ||
//           str == "DELETE");
// }

// bool isValidDirective(std::string const &str) {
//   return (str == "listen" ||
//           str == "server_name" ||
//           str == "root" ||
//           str == "auth" ||
//           str == "error_page" ||
//           str == "upload" ||
//           str == "autoindex" ||
//           str == "index" ||
//           str == "cgi" ||
//           str == "cgi_bin");
// }

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}