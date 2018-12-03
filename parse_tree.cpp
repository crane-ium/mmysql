#include "parse_tree.h"

//-----------------------------------------------------------
//HELPER FUNCTIONS

bool parsetree::in_set(char c, std::set<std::string> s){
    for(auto it = s.begin(); it != s.end(); it++){
        if(c == (*it)[0])
            return true;
    }
    return false;
}

