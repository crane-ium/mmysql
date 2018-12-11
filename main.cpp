//MultiMap SQL (MMySQL)
#include <iostream>
#include <sstream>
#include <iterator>
//#include "../bplustree_mmap/multimap.h"
#include "mmytable.h"
#include "mmysql.h"
//#include "../state_machine/ftokenizer.h"
#include "../state_machine/ftokenizer.h"

int DEBUG = 0;

using namespace std;
//using FTokenizer = sm::FTokenizer;
//using Token = sm::SToken;
using iter = typename multimap<string, long>::Iterator;
void field_test1();
void various_test1();
multimap<string, long> get_word_indices(const string& file_name);

int main(int argc, char *argv[])
{
    vector<string> fields{"lname", "fname", "age", "major"};
    record rec;
    vector<string> data0{"Crane", "Stephen", "120", "Statistics"};
    vector<string> data1{"Blow", "Joe", "15", "Math"};
    vector<string> data2{"Barkeshli", "Sassan", "100", "CS"};
    vector<string> data3{"Stack", "Bryan", "20", "CS"};
    vector<string> data4{"Grey", "Chris", "20000", "Computer Science"};
    mmytable table("test");

//    vector<string> allowedfields{"age", "lname", "fname", "Does not exist"};

    string s = "age>15 or fname = Joe or (major = \"Statistics\" or major=CS)";
    vector<string> allowedfields{"age", "fname", "lname", "major", "nothing", "major", "age"};
    s = "age<100 or age>100"; /** @todo FIX FIX!!!!!! FIX THE EXPRESSSION TREE **/
    fstream resultsfile("resultsfile.bin", ios::binary | ios::out | ios::ate);
    table.select(resultsfile, s, allowedfields);
    resultsfile.close();

    stringstream ss("123  456");
    noskipws(ss);
    ss.seekg(2);
    auto it = istream_iterator<char>(ss);
    cout << "Iterating\n";
    for(;it!=istream_iterator<char>();it++){
        if((*it)==' ')
            break;
        cout << (*it) << endl;
    }
    cout << "remaining string: " << ss.str().substr(ss.tellg()) << endl;

     mmysql sql;
     sql.start();

//    s = "one, 12345, three";
//    stringstream ss(s);
//    string s2;
//    ss >> s2 >> s2;
//    ss.seekg(ss.tellg()-6);
//    ss >> s2;
//    cout << s2;
//    cout << "ss data: " << ss.tellg() << endl;

//    parse_tree<int> parse_tree_test;
//    cin >> parse_tree_test;

//    simple_map<string, int> foo;
//    foo["seen"];
//    foo["see"];
//    foo["seems"];
//    foo["sea"];
//    cout << foo << endl;
//    auto it = foo.lower_bound("s");
//    cout << "lower: ";
//    cout << (it!=foo.end() ? (*it).key : "out of bounds") << endl;

//    string temp;
//    string start = "select name, age from students where name=Stephen \n"
//            "insert into employees values Hahaha, Hehehe, Hohoho\n";
//    stringstream tempss;
////    istream tempis;
//    tempss << start;
////    tempis << start;
//    char* tempptr;
//    while(tempss >> temp){
//        cout << temp;
//        while(tempss.peek()==' ')
//            tempss.seekg(tempss.tellg()+1);
//        if(tempss.peek()=='\n')
//            cout <<"Hit endline\n";
//    }


    //I THINK I AM GONNA GET RID OF THIS WHOLE CODE
//    mmystate<> sm;
//    sm.add_states(flagstate::none, flagstate::alphanumeric, mmy::ALPHANUMERICS);
//    sm.add_states(flagstate::none, flagstate::comma, mmy::COMMA);
//    sm.add_states(flagstate::none, flagstate::quote, mmy::QUOTE);
//    sm.add_states(flagstate::quote, flagstate::inquote, mmy::NOTQUOTE);
//    sm.add_states(flagstate::inquote, flagstate::inquote, mmy::NOTQUOTE);
//    sm.add_states(flagstate::inquote, flagstate::endquote, mmy::QUOTE);
//    sm.add_states(flagstate::inquote, flagstate::endquote, mmy::QUOTE);

//    sm.__parsemap.print();
//    string stest = "select name, age from people";
//    vector<string> parsedstring = sm.parse_stream(stest);
    //-----------------------------------------------------------------

//    cout << parsedstring << endl;

//    string s = "(last = \"Van Gogh\" or last = Jackson and salary >= 165000) or (andandor = 5)";

//    mmynode* tree = parse_to_tree(s);
//    tree->print();

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
void various_test1(){

    set<unsigned long> s1{1, 2, 3, 4};
    set<unsigned long> s2{3, 4, 5, 6};
    set<unsigned long> result;
    result = s1+s2;
    cout << result << endl;
    result = s1*s2;
    cout << result << endl;
    result = s1^s2;
    cout << result << endl;

    simple_map<string, long> lexo;

    simple_map<string, string> nondefault(false);
    nondefault.insert("one","ONE!");
    cout << nondefault["one"] << endl;
//    cout << nondefault["two"] << endl;
}
