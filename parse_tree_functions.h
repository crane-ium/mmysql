#ifndef PARSE_TREE_FUNCTIONS_H
#define PARSE_TREE_FUNCTIONS_H

#include <set>
#include <iostream>

//HELPER FUNCTIONS
//in_set: return T if c exists as starting char of a string in set s
namespace parsetree{
    bool in_set(char c, std::set<std::string> s); //Checks if char c is in set s
    template<class settype> //Iterates through a string and finds any matches based on given set
    std::string in_set(const std::string& str,
                                  settype s, std::size_t index){

        for(auto it = s.begin(); it != s.end(); it++){
            if((*it) == str.substr(index, (*it).size()))
                return (*it);
        }
        return "";
    }
}
#endif // PARSE_TREE_FUNCTIONS_H
