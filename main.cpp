//MultiMap SQL (MMySQL)
#include <iostream>
#include "mmytable.h"

using namespace std;

int main()
{
    mmytable test;
    cout << "printing\n";
//    test.create("Test Table", "one", "two", "three", "four", "five");
    test.init("testtable");
    test.field_add("one");
    test.field_add("two");
    test.field_add("three");
    test.field_add("four");
    test.__itables.print();
//    string s = "hello";
//    cout << (int)s.find('x') << endl;
    string teststr = "123| foo | \nbar|    final test|jk";
    test.parse(teststr, teststr.size());
    test.__itables.print();

    return 0;
}
