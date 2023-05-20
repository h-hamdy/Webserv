#include"lexer.hpp"

Lexer::Lexer(std::string filename) {
    this->filename = filename;
    this->file.open(filename);
    this->type_set = false;
}

Lexer::~Lexer() {
    this->file.close();
}

void    Lexer::lex(std::string line) {
    std::string buffer;
    int j = 0;
    
    this->type_set = true;
    this->type = "";
    this->value.clear();

    if (line.find('#') != std::string::npos){
        line = line.substr(0, line.find('#'));
    }
    while(line.length() > 0){
        line = line.erase(0,line.find_first_not_of(" \t\r\f\v"));
        j = line.find_first_of(" \t\r\f\v");
        if(j == std::string::npos)
            j = line.length();
        buffer = line.substr(0, j);
        line = line.erase(0, buffer.length());
        if(buffer == "")
            continue;
        if(buffer == "{" || buffer == "}"){
            if(buffer == "{")
                brackets.push(true);
            else{
                if(!brackets.empty())
                    brackets.pop();
                else
                    throw std::runtime_error("Error: brackets mismatch");
            }
        }
        else if(this->type_set)
        {
            this->settype(buffer);
            this->type_set = false;
        }
        else
            this->setvalue(buffer);
    }
}

//==============================================================================================================================================================

Lexer  &Lexer::get_token() {

    std::string line;
    if(!std::getline(this->file, line))
    {
        this->type = "EOF";
        this->value.push_back("EOF");
        return *this;
    }
    this->lex(line);

    return *this;
}

void Lexer::settype(std::string type) {
    this->type = type;
}

void Lexer::setvalue(std::string value) {
    this->value.push_back(value);
}

std::string Lexer::gettype() {
    return this->type;
}

std::string Lexer::getvalue(int i) {
    return this->value[i];
}

int Lexer::get_size_value()
{
    return(this->value.size());
}