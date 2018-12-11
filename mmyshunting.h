#ifndef MMYSHUNTING_H
#define MMYSHUNTING_H

#include <cstdlib>
#include <string>
#include <set>
#include <stack>
#include <iomanip>
#include "sethelper.h"
#include "../bplustree_mmap/multimap.h"
#include "../bplustree_mmap/map.h"
#include "parse_tree_functions.h"

//Include constants
#include "mmy_constants.h"

extern debugger DBG; //debug flag

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
        cout << setw(level) << "[";
        if(ttype == tokentype::emptyset)
            cout << "nullset" << "]\n";
        else if(idnums.empty()){
            cout << val << "]\n";
        }else
            cout << idnums << "]\n";
        if(left)
            left->print(level+10);
    }
    //Insert a child until it finds a null ptr left/right or
    //  insert into the rightchild
    void insert_boolean(mmynode *&ptr);
    //Generate ids for each node that has a comparitor
    void generate_ids(simple_map<string, multimap<string, unsigned long> >& map);
    //Generate ids by applying the boolean operations on the generated ids
    set<unsigned long> get_ids(); //requires generate_ids called before
    set<unsigned long> get_ids(simple_map<string, multimap<string, unsigned long> >& map);
    mmynode* left, *right; //children
    string val; //current value with an associated tokentype
    tokentype ttype;
    //If it's a boolean, it will use this to check for precedence
    booleantype btype=booleantype::NONE;
    set<unsigned long> idnums; //set of idnums that will be processed
};
//-----------------------------------------------------------------
//HELPER FUNCTIONS
//Creates a tree to be used in shunting_yard
mmynode* parse_to_tree(const string &s);

//Trims a string to be passed down to children
string mmytrim(const string& s, size_t left, size_t right);

//Compare two variables based on a given comparitor
template<class numbertype, class compareetype>
bool compare_fields(const numbertype& left, const compareetype& right, const string& comparee){
    if(DBG>=debugger::light) cout << "Using templated compare fields: "
                                  << left << comparee << right << endl;
    if(comparee == "=" or comparee == "==")
        return left == static_cast<numbertype>(right);
    else if(comparee == ">")
        return left > static_cast<numbertype>(right);
    else if(comparee == ">=")
        return left >= static_cast<numbertype>(right);
    else if(comparee == "<")
        return left < static_cast<numbertype>(right);
    else if(comparee == "<=")
        return left <= static_cast<numbertype>(right);
    else if(comparee == "!=")
        return left != static_cast<numbertype>(right);

    if(DBG>=debugger::none)
        cout << "Failed to reach a comparison, crashing...\n";
    assert(false);
}
//Compare lexicographically
bool compare_fields(const string&left, const string&right, const string &comparee);

/**
 * @brief mmyshunting provides a means to parse the constraints
 *      on field names and return the ID#s that fulfill that
 */
class mmyshunting{
public:
    mmyshunting(const string& constraint){
        __root = parse_to_tree(constraint);
    }
    ~mmyshunting(){delete __root;}
    //MEMBERS
    void print(){__root->print();}
    /**
     * @brief Returns a set of id#s that reference the data entries
     *          that pass both the comparison tests AND pass the
     *          union/intersection tests.
     */
    template<class T>
    set<unsigned long> get_ids(simple_map<string, multimap<string, T> >& map){
        //First we do an infix traversal to find and evaluate at
        //each tokentype::comparitor node on the tree, replacing that
        //node with a set of id#s that pass the comparison
        assert(__root);
        __root->generate_ids(map);
        set<unsigned long> result = __root->get_ids();
        return result;
    }
private:
    mmynode* __root;
};

#endif // MMYSHUNTING_H
