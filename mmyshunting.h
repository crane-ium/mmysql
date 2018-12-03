#ifndef MMYSHUNTING_H
#define MMYSHUNTING_H

#include <cstdlib>
#include <string>
#include <set>
#include <stack>
#include <iomanip>
#include "parse_tree.h"
#include "../bplustree_mmap/multimap.h"
#include "../bplustree_mmap/map.h"

//sorted by precedence, from lowest to highest
enum class debugger{none, light, medium, heavy};
enum class tokentype {none, fieldname, comparee, comparitor, boolean};
static const set<string> BBOOLEANS{"and", "or", "xor"}; //binary
static const set<string> UBOOLEANS{"not"}; //unary
static const set<string, greater<string> > COMPARITORS{"==", "=", "!=", "<=", ">=", "<", ">"};
static const set<string> PREOPERATORS{")", " "};
static const set<string> POSTOPERATORS{"(", " "};
static const set<string> BLOCKSKIP{"\""};

extern debugger DBG;

//Holds shunting_yard data
struct mmynode{
    mmynode(const string& value, mmynode* l=nullptr, mmynode* r=nullptr)
        :val(value), left(l), right(r), ttype(tokentype::none){
    }
    ~mmynode(){delete left; delete right;}
    //MEMBERS
    print(size_t level = 0){
        if(right)
            right->print(level+10);
        cout << setw(level) << "[" << val << "]\n";
        if(left)
            left->print(level+10);
    }
    mmynode* left, *right; //children
    string val; //current value with an associated tokentype
    tokentype ttype;
};
//-----------------------------------------------------------------
//HELPER FUNCTIONS
//Creates a tree to be used in shunting_yard
mmynode* parse_to_tree(const string &s);

//Trims a string to be passed down to children
string mmytrim(const string& s, size_t left, size_t right);

//get next string block
string next_block(const string& s, size_t& i);
    //Given a string and a starting point, find the next string block
    //string-block: Either a block defined by parentheses, OR
    //  the


/**
 * @brief mmyshunting provides a means to parse the constraints
 *      on field names and return the ID#s that fulfill that
 */
class mmyshunting{
public:
    mmyshunting();

private:

};

#endif // MMYSHUNTING_H
