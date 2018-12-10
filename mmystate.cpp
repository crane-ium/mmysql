#include "mmystate.h"

charinterval::charinterval(char bot, char top)
    :lower(bot), upper(top){
    if(upper=='\0')
        upper = lower;
}

ostream& operator <<(ostream& outs, const flagstate& rhs){
    //for now let's just do a simple cast to display flagstate
    outs << "flagstate::" << static_cast<int>(rhs);
//    switch(rhs){
//    case mmystate::flagstate::none:
//        outs << "flagstate::none";
//        break;
//    case mmystate::flagstate::alphanumeric:
//        outs << "flagstate::alphanumeric";
//        break;
//    case mmystate::flagstate::quote:
//        outs << "flagstate::alphanumeric";
//        break;
//    case mmystate::flagstate::alphanumeric:
//        outs << "flagstate::alphanumeric";
//        break;
//    case mmystate::flagstate::alphanumeric:
//        outs << "flagstate::alphanumeric";
//        break;
//    }
    return outs;
}

//HELPER FUNCTIONS FOR CHAR INTERVAL
ostream& operator <<(ostream& outs, const charinterval& rhs){
    outs << "[" << rhs.lower << ", " << rhs.upper << "]";
    return outs;
}
bool operator ==(const charinterval& lhs, const charinterval& rhs){
    //if there is any intersection, return true. This is to avoid intersecting
    //  intervals in a non-duplicate map
    return !(lhs.upper < rhs.lower || lhs.lower > rhs.upper);
}
bool operator !=(const charinterval& lhs, const charinterval& rhs){
    return !(lhs == rhs);
}
bool operator <=(const charinterval& lhs, const charinterval& rhs){
    return lhs.upper <= rhs.lower;
}
bool operator >=(const charinterval& lhs, const charinterval& rhs);
bool operator <(const charinterval& lhs, const charinterval& rhs){
    return lhs.upper < rhs.lower;
}
bool operator >(const charinterval& lhs, const charinterval& rhs){
    return lhs.lower > rhs.upper;
}
