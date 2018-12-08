#ifndef SETHELPER_H
#define SETHELPER_H
#include <set>
#include <iostream>
#include <vector>

typedef std::set<unsigned long> ulset;
//adds a vector of ulongs to a set of ulongs
std::set<unsigned long>& operator +=(std::set<unsigned long>& lhs, const std::vector<unsigned long>& rhs);
//cout a set
template<class T>
std::ostream& operator <<(std::ostream& outs, const std::set<T>& rhs){
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
ulset operator+(const ulset& lhs, const ulset& rhs);
//Set meet
ulset operator*(const ulset& lhs, const ulset& rhs);
//Set XOR
ulset operator^(const ulset& lhs, const ulset& rhs);
#endif // SETHELPER_H
