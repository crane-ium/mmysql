#include "sethelper.h"

//adds a vector of ulongs to a set of ulongs
std::set<unsigned long>& operator +=(std::set<unsigned long>& lhs, const std::vector<unsigned long>& rhs){
    for(auto it=rhs.begin(); it != rhs.end(); it++){
        lhs.insert(*it);
    }
    return lhs;
}
//cout a set
std::ostream& operator <<(std::ostream& outs, const std::set<unsigned long>& rhs){
    outs << "set<ul>: {";
    for(auto it = rhs.begin(); it != rhs.end(); it++){
        if(it != rhs.begin())
            outs << ", ";
        outs << (*it);
    }
    outs << "}";
    return outs;
}
//Set join
ulset operator+(const ulset& lhs, const ulset& rhs){
    ulset copy = lhs;
    copy.insert(rhs.begin(), rhs.end());
    return copy;
}
//Set meet
ulset operator*(const ulset& lhs, const ulset& rhs){
    //Find intersection by iterating through both
    ulset intersection;
    auto lit = lhs.begin();
    auto rit = rhs.begin();
    for(;lit!=lhs.end() && rit != rhs.end();){
        if((*lit) != (*rit)){
            if((*lit) < (*rit))
                lit++;
            else
                rit++;
        }else{
            intersection.insert(*lit);
            lit++;
            rit++;
        }
    }
    return intersection;
}
//Set XOR
ulset operator^(const ulset& lhs, const ulset& rhs){
    //Find where it ORs but does ont intersect
    ulset copy = rhs;
    for(auto it = lhs.begin(); it != lhs.end(); it++){
        auto cit = copy.erase((*it));
        if(!cit)
            copy.insert(*it);
    }
    return copy;
}
