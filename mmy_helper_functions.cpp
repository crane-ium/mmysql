#include "mmy_helper_functions.h"

booleantype get_booleantype(const std::string& s){
    //Let's assume the string was already turned lowercase
    if(s=="and"){
        return booleantype::AND;
    }else if(s=="or"){
        return booleantype::OR;
    }else if(s=="xor")
        return booleantype::XOR;
    else if(s=="not")
        return booleantype::NOT;
    else{
        std::cout << "[ISSUE] mmy_helper_functions: Invalid booleantype\n";
        return booleantype::INVALID;
    }
}

std::string get_lowercase(std::string s){
    //Transform the string copy into a lowercase and return it
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}
std::string& transform_lowercase(std::string& s){
    //Transform the string copy into a lowercase and return it
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}
std::string get_uppercase(std::string s){
    //Transform the string copy into a lowercase and return it
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}
std::string& transform_uppercase(std::string& s){
    //Transform the string copy into a lowercase and return it
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}
