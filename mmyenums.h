#ifndef MMYENUMS_H
#define MMYENUMS_H

#include <iostream>

//DECLARE OUR MODES/STATES FOR OUR STATEMACHINE
enum class token{none, modename, fieldname, tablename, constraint, wildcard};
enum class mode {start=0, select=1, create=2, make=6,
                 insert=3, exit=4, history=5, DEFAULT=mode::start};
//This array is used for a ranged for loop
static mode modelist[7] = {mode::start, mode::select, mode::create, mode::make,
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
static sf sflist[7] = {sf::none, sf::allowasterisk, sf::allowcomma,
                sf::allowstring, sf::allowquotes,
                sf::repeatable, sf::getall};
//Give it a bitwise pipe for combining flags
inline sf operator |(sf lhs, sf rhs){
    sf piped = static_cast<sf>(static_cast<int>(lhs) | static_cast<int>(rhs));
    return piped;
}
inline sf operator &(sf lhs, sf rhs){
    sf piped = static_cast<sf>(static_cast<int>(lhs) & static_cast<int>(rhs));
    return piped;
}
/** @example sf::allowasterk == prev_defined -> return true **/
inline bool operator==(sf lhs, sf rhs){
    //Checks that the bit of lhs exists within rhs
    return static_cast<int>(lhs) == static_cast<int>(lhs & rhs);
}
//token overloaded comparisons for using in map
inline bool operator ==(const token lhs, const token rhs){
    return static_cast<int>(lhs)==static_cast<int>(rhs);
}
inline bool operator <(const token lhs, const token rhs){
//    return lhs < rhs;
    return static_cast<int>(lhs)<static_cast<int>(rhs);
}
inline bool operator >(const token lhs, const token rhs){
//    return lhs > rhs;
    return static_cast<int>(lhs)>static_cast<int>(rhs);
}
inline bool operator <=(const token lhs, const token rhs){
//    return lhs <= rhs;
    return static_cast<int>(lhs)<=static_cast<int>(rhs);
}
inline std::ostream& operator <<(std::ostream& outs, const token& rhs){
    outs << "token::";
    switch(rhs){
    case token::none:
        outs << "none";
        break;
    case token::constraint:
        outs << "constraint";
        break;
    case token::fieldname:
        outs << "fieldname";
        break;
    case token::modename:
        outs << "modename";
        break;
    case token::tablename:
        outs << "tablename";
        break;
    default:
        outs << "INVALID";
        break;
    }
    return outs;
}

#endif // MMYENUMS_H
