//MultiMap SQL (MMySQL)
#include <iostream>
//#include "../bplustree_mmap/multimap.h"
#include "mmytable.h"
//#include "../state_machine/ftokenizer.h"
#include "../state_machine/ftokenizer.h"

int DEBUG = 0;

using namespace std;
//using FTokenizer = sm::FTokenizer;
//using Token = sm::SToken;
using iter = typename multimap<string, long>::Iterator;
void field_test1();
multimap<string, long> get_word_indices(const string& file_name);

int main(int argc, char *argv[])
{
    vector<string> fields{"field1", "field2", "field3"};
//    writer w("test", fields);
    record rec;
    vector<string> data{"data1", "data2", "more", "extra"};
    vector<string> data1{"data3", "data4"};
    vector<string> data2{"data5", "data6"};
    vector<string> data3{"data7"};
//    rec.set_file("test");
//    rec.insert(data);
//    rec.insert(data3);
//    rec.insert(data1);
//    rec.insert(data2);
//    rec.insert(data3);
//    rec.insert(data2);
//    rec.insert();
//    while(rec.more())
//        cout << rec.next() << endl;
    mmytable table("test");

    cout << table.__itables << endl;

    table.select("");

    string s = "(last = \"Van Gogh\" or last = Jackson and salary >= 165000) or (andandor = 5)";

    mmynode* tree = parse_to_tree(s);
    tree->print();

//    simple_map<string, multimap<string, long> > test;
//    simple_map<string, string> test2;
//    multimap<string, long> test3;
//    multimap<string, long> test4;

//    while(rec.more())
//        cout << rec.next() << endl;

//    multimap<string, long> word_indices;
//    word_indices = get_word_indices("../state_machine/solitude.txt");
//    cout<<endl<<endl<<endl;

//    //list all nodes of the index multimap:
//    for (iter it = word_indices.begin();
//         it != word_indices.end(); it++){
////        cout<<*it<<endl;
//    }

//    cout<<endl<<endl<<endl;
//    cout<<"---------------------------------------------------"<<endl;
//    string this_word = "ice";
//    cout<<"---------------------------------------------------"<<endl;
//    cout<<"Indices of \""<<this_word<<"\""<<endl;
//    //list indices of this_word:
//    if (word_indices.contains(this_word)){
//        cout<<this_word<<": "<<word_indices[this_word]<<endl;
//    }
//    cout<<endl<<endl<<endl;

//    cout<<"---------------------------------------------------"<<endl;
//    string from = "ask";
//    string to = "ass";
//    //list from .. to:
//    cout<<"listing indices from \""<<from<<"\" to \""<<to<<"\""<<endl;
//    cout<<"---------------------------------------------------"<<endl;
//    for (iter it =
//                word_indices.lower_bound(from);
//         it != word_indices.upper_bound(to) &&
//         it != word_indices.end(); it++){
//        cout<<*it<<endl;
//    }

//    cout <<endl<<endl<<endl<< "========== E N D  ====================" << endl;
//    return 0;
}


multimap<string, long> get_word_indices(const string &file_name){
    const bool debug = false;
    multimap<string, long> word_indices;
    typedef sm::FTokenizer FTokenizer;
    typedef sm::SToken Token;
    FTokenizer ftk(file_name);
    Token t;
    long count = 0;

    ftk >> t;
    while (ftk.more()){
        //only the "words"
        if (t.type_string() == "ALPHA"){
            string s;
            s = t.token_str();
            word_indices[s] += count;
            count++;
            if (debug)
                cout<<"|"<<t.token_str()<<"|"<<endl;
        }
        ftk >> t;
    }
    return word_indices;
}

void field_test1()
{
    mmytable test;
    cout << "printing\n";
//    test.create("Test Table", "one", "two", "three", "four", "five");
    test.init("testtable");
    test.field_add("one");
    test.field_add("two");
    test.field_add("three");
    test.field_add("four");
//    test.__itables.print();
    string teststr = "123| foo | \nbar|    final test|jk";
    test.parse(teststr, teststr.size());
//    test.__itables.print();

//    return 0;
}
