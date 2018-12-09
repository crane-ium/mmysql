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
    enum class token{none, modename, fieldname, tablename, constraint};
    enum class mode {start=0, select=1, create=2, make=6,
                     insert=3, exit=4, history=5, DEFAULT=mode::start};
    //This array is used for a ranged for loop
    mode modelist[7] = {mode::start, mode::select, mode::create, mode::make,
                       mode::insert, mode::exit, mode::history};
    enum class state{start, tablekey, fieldskey, intokey, fromkey,
                     valueskey, wherekey, getfields, getconstraint,
                     gettable, getvalues, DEFAULT=state::start};
    //I am going ot use stateflags like integer representations
    //  of bits, so I can do clean bitwise operations
    /** @def sf = stateflag **/
    enum class sf{none=0x00, allowcomma=0x01, allowstring=0x02,
                  allowquotes=0x04, repeatable=0x08, allowasterisk=0x10,
                  getall=0x20, DEFAULT=sf::none};
    sf sflist[7] = {sf::none, sf::allowasterisk, sf::allowcomma,
                    sf::allowstring, sf::allowquotes,
                    sf::repeatable, sf::getall};
    //Give it a bitwise pipe for combining flags
    inline friend sf operator |(sf lhs, sf rhs){
        sf piped = static_cast<sf>(static_cast<int>(lhs) | static_cast<int>(rhs));
        return piped;
    }
    inline friend sf operator &(sf lhs, sf rhs){
        sf piped = static_cast<sf>(static_cast<int>(lhs) & static_cast<int>(rhs));
        return piped;
    }
    /** @example sf::allowasterk == prev_defined -> return true **/
    inline friend bool operator==(sf lhs, sf rhs){
        //Checks that the bit of lhs exists within rhs
        return static_cast<int>(lhs) == static_cast<int>(lhs & rhs);
    }
    //Pairs a specific word with flags. This way we can put into our
    //  statemachine and do easy clean comparisons.
    struct rulepair{
        rulepair(state next=state::DEFAULT, token type=token::none,
                 sf flags=sf::none, set<string> specifier=set<string>(),
                 bool valid=false)
            :nextstate(next), flags(flags),
              specifier(specifier), type(type), valid(valid){}
        state nextstate;
        set<string> specifier;
        sf flags;
        token type;
        bool valid;
        //goes to next state unless the repeatable requirements are fulfilled
    };
    //Pairs a string with a token
    struct mmytoken{
        mmytoken(const string& value, token type=token::none)
            :value(value), type(type){}
        string value;
        token type;
    };
    static inline string get_mode_string(mode m){
        switch(m){
        case mode::start:
            return "start";
        case mode::select:
            return "select";
        case mode::create:
            return "create";
        case mode::make:
            return "make";
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
    outs << "mode::" << mmysql::get_mode_string(rhs);
    return outs;
}
inline ostream& operator <<(ostream& outs, mmysql::state rhs){
    /** @todo **/
    outs << "state::" << static_cast<int>(rhs);
    return outs;
}
inline ostream& operator <<(ostream& outs, const mmysql::rulepair& rhs){
    outs << "[rp:"<<static_cast<int>(rhs.nextstate)<<"|"
         <<static_cast<int>(rhs.flags)<<"|"<<rhs.specifier<<"]";
    return outs;
}
inline ostream& operator << (ostream& outs, const mmysql::mmytoken& rhs){
    outs << static_cast<int>(rhs.type) << ":" << rhs.value;
}
inline ostream& operator << (ostream& outs, const vector<mmysql::mmytoken>& rhs){
    for(auto it=rhs.begin(); it!= rhs.end(); it++){
        if(it !=rhs.begin())
            outs << ", ";
        outs << (*it);
    }
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
