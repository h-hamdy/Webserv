#include"lexer.hpp"

bool    is_type(std::string type) {
    std::string types[] = {"host", "port", "client_max_body_size", "index", "error_page", "root", "location", "autoindex"};
    int i = 0;
    while(i < 9)
    {
        if(types[i] == type)
            return true;
        i++;
    }
    return false;
}



void init_config(std::list<t_config> &cfg) {
    t_config config;

    config.host = "";
    config.body_size_limit = 0;
    config.root = "";
    config.autoindex = false;
    cfg.push_back(config);
}

std::list<t_config>    parse_config(std::string filename) {

    Lexer lexer(filename);
    std::list<t_config> config;

    std::string types[] = {"host", "port", "client_max_body_size", "index", \
    "error_page", "root", "location", "autoindex"};
    void    (*foo[]) (Lexer &,t_config &) = {parse_host , parse_port, parse_client_max_body_size, parse_index, \
        parse_error_page, parse_root, parse_location, parse_auto_index};
    
    while (lexer.gettype() != "EOF"){
        lexer.get_token();
        if(lexer.gettype() == "" ||lexer.gettype() == "server")
        {
            if(lexer.gettype() == "server")
                init_config(config);
            continue;
        }
        for(int i = 0 ; i < 8; i++)
        {
            if(lexer.gettype() == types[i]){
                foo[i](lexer, config.back());
            }
        }
    }
    return config;
}

int main( ){

    int i = 0;
    std::list<t_config> config;
    try
    {
       config = parse_config("newfile.conf");
       for(int i = 0; i < config.size(); i++)
       {
           std::cout << "host: " << config.front().host << std::endl;
           std::cout << "port: " << config.front().port.front() << std::endl;
           std::cout << "client_max_body_size: " << config.front().body_size_limit << std::endl;
           std::cout << "index: " << config.front().index.front() << std::endl;
           std::cout << "error_page: " << config.front().default_error_page["404"] << std::endl;
           std::cout << "root: " << config.front().root << std::endl;
           std::cout << "autoindex: " << config.front().autoindex << std::endl;
           config.pop_front();
       }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}