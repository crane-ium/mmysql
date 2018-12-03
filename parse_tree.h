#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <iostream>
#include <string>
#include <set>

//HELPER FUNCTIONS
//in_set: return T if c exists as starting char of a string in set s
namespace parsetree{
    bool in_set(char c, std::set<std::string> s);
    template<class settype>
    std::string in_set(const std::string& str,
                                  settype s, std::size_t index){

        for(auto it = s.begin(); it != s.end(); it++){
            if((*it) == str.substr(index, (*it).size()))
                return (*it);
        }
        return "";
    }
}
class parse_tree
{
public:
    parse_tree();
};



#endif // PARSE_TREE_H
