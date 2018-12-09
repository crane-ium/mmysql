#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <iostream>
#include <string>
#include <set>

/**
 * @brief Parse Tree: Custom define it, then give it strings to parse,
 *                      then get back queue of commands to execute
 */
template<typename enumtype>
class parse_tree
{
public:
    //CTOR
    parse_tree(){}
    //BIG3

    //MEMBERS

    void set_delimiter(char delimiter){__delimiter=delimiter;}
    //overload cin >> parse_tree
    //Parse the given stream
    friend std::istream& operator >>(std::istream& ins, const parse_tree<enumtype>& pt){

        return ins;
    }
private:
    char __delimiter=','; //for seperating fieldnames
};



#endif // PARSE_TREE_H
