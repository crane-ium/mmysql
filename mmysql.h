/**
  * @file mmysql.h
  */
#ifndef MMYSQL_H
#define MMYSQL_H

#include <sstream>
#include <iostream>
#include <queue>
#include "mmytable.h" //read/writes from/to database. SQL gives commands

using namespace std;

/**
 * @brief Interact with database
 */
class mmysql
{
public:
    //DECLARE OUR MODES/STATES FOR OUR STATEMACHINE
    enum class token{none=0, modename, fieldname, tablename, constraint};
    enum class mode {start=0, select=1, create=2,
                     insert=3, exit=4, history=5, DEFAULT=mode::start};
    mode modelist[6] = {mode::start, mode::select, mode::create,
                       mode::insert, mode::exit, mode::history};
    //I am going ot use stateflags like integer representations
    //  of bits, so I can do clean bitwise operations
    /** @def sf = stateflag **/
    enum class sf{none=0x00, allowcomma=0x01, allowstring=0x02,
                  allowquotes=0x04, repeatable=0x08, DEFAULT=sf::none};
    //Give it a bitwise pipe for combining flags
    inline friend sf operator |(sf lhs, sf rhs){
        sf piped = static_cast<sf>(static_cast<int>(lhs) | static_cast<int>(rhs));
        return piped;
    }
    inline friend sf operator &(sf lhs, sf rhs){
        sf piped = static_cast<sf>(static_cast<int>(lhs) & static_cast<int>(rhs));
        return piped;
    }
    //Pairs a specific word with flags. This way we can put into our
    //  statemachine and do easy clean comparisons.
    struct rulepair{
        rulepair(sf flags=sf::none, string specifier="")
            :_flags(flags), _specifier(specifier){}
        string _specifier;
        sf _flags;
    };
    static inline string get_mode_string(mode m){
        switch(m){
        case mode::start:
            return "start";
        case mode::select:
            return "select";
        case mode::create:
            return "create";
        case mode::insert:
            return "insert";
        case mode::exit:
            return "exit";
        case mode::history:
            return "history";
        default:
            return "none";
        }
    }
    enum class state{start, DEFAULT=state::start};
    //CTOR
    mmysql();
    ~mmysql(); //deletes mmytable pointers in our map
    //MEMBER FUNCTIONS

    //return a 2d table with with requested fields
//    vector<vector<string> >& select(vector<string>& fields);
    void run();
private:
    //Our statemachine adjacency "matrix"
    /** @example __parsetree[static_cast<int>(mode)][__currentstate] **/
    vector<simple_map<state, rulepair> > __parsetree;
    simple_map<string, mode> __modetree; //simple tree to act like a set
    simple_map<string, mmytable*> __database; //Store all accessed tables
    string __line;
    string __constraint;
    //sections of the parsed line. Interpret these in the parse tree
    queue<string> __linewords;
    vector<string> __history; //History of user's data
    mode __currentmode=mode::start;
    state __currentstate=state::start;
    void display_terminal();
    void define_parsetree(); //define our parsetree aka statemachine
};

//trying out some inline functions. I don't think i like it though because
//  it clogs the build time
inline ostream& operator <<(ostream& outs, mmysql::mode rhs){
    cout << "mode::" << mmysql::get_mode_string(rhs);
    return outs;
}
inline ostream& operator <<(ostream& outs, mmysql::state rhs){
    /** @todo **/
    cout << "state::" << static_cast<int>(rhs);
    return outs;
}
inline ostream& operator <<(ostream& outs, const mmysql::rulepair& rhs){
    cout << "[rp:"<<static_cast<int>(rhs._flags)<<"|"<<rhs._specifier<<"]";
    return outs;
}

class mmysql_user{
public:
    mmysql_user();
private:
    mmysql __sql;
    string __history;
};

#endif // MMYSQL_H
