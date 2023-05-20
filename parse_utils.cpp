#include"lexer.hpp"

void parse_host(Lexer &lexer,t_config &config) {

    if(lexer.get_size_value() > 1 || lexer.get_size_value() == 0 )
        throw std::runtime_error("Error: host take one value");
    config.host = lexer.getvalue(0);
}

void parse_port(Lexer &lexer,t_config &config) {

    if(lexer.get_size_value() > 3 || lexer.get_size_value() == 0)
        throw std::runtime_error("Error: port take three values");
    
    for(int i = 0; i < lexer.get_size_value(); i++)
    {
        if(!is_number(lexer.getvalue(i)) && lexer.getvalue(i) != "")
        {
            if(lexer.getvalue(i) == "")
            throw std::runtime_error("Error: port take numbers");
        }
        config.port.push_back(lexer.getvalue(i));
    }
}

void parse_client_max_body_size(Lexer &lexer,t_config &config) {

    if(lexer.get_size_value() > 1 || lexer.get_size_value() == 0)
        throw std::runtime_error("Error: client_max_body_size take one value");
    if(!is_number(lexer.getvalue(0)))
        throw std::runtime_error("Error: client_max_body_size take numbers");
    config.body_size_limit = std::stoi(lexer.getvalue(0));
}

void parse_index(Lexer &lexer,t_config &config) {

    if(lexer.get_size_value() > 3 || lexer.get_size_value() == 0 )
        throw std::runtime_error("Error: index can have only three values");
    
    for(int i = 0; i < lexer.get_size_value(); i++)
        config.index.push_back(lexer.getvalue(i));
}

void parse_error_page(Lexer &lexer,t_config &config) {

    if(lexer.get_size_value() > 2 || lexer.get_size_value() < 2 )
        throw std::runtime_error("Error: default_error_page take two values");
    config.default_error_page[lexer.getvalue(0)] = lexer.getvalue(1);
}

void parse_root(Lexer &lexer,t_config &config) {

    if(lexer.get_size_value() > 1 || lexer.get_size_value() == 0)
        throw std::runtime_error("Error: root take one value");
    config.root = lexer.getvalue(0);
}

void    parse_auto_index(Lexer &lexer,t_config &config) {

    if(lexer.get_size_value() > 1 || lexer.get_size_value() == 0)
        throw std::runtime_error("Error: autoindex take one value");
    if(lexer.getvalue(0) == "on")
        config.autoindex = true;
    else if(lexer.getvalue(0) == "off")
        config.autoindex = false;
    else
        throw std::runtime_error("Error: autoindex take on or off");
}

void    parse_location(Lexer &lexer,t_config &config) {
    std::cout << "location" << std::endl;
}