/**
  * @file mmysql.cpp
  * @header mmysql.h
  */
#include "mmysql.h"

//Default constructor for mmysql querying controls
mmysql::mmysql(){
    cout << "INITIALIZING MMYSQL(TM) DATABASE QUERY INTERPRETER...";
    cout << "INITIALIZED!\n";
    cout << "PREPARING INTERPRETER...\n";
    cout << "~~~WELCOME TO MMYSQL(TM) DATABASE QUERY INTERPRETER~~~\n";
    define_parsetree();
    while(__currentmode != mode::exit)
        run();
}

mmysql::~mmysql(){
    for(auto it=__database.begin(); it!= __database.end(); it++){
        delete (*it).val;
    }
}
//Runs the processes, starting by getting the next line;
void mmysql::run(){
    //reset anything
//    cin.clear();
//    cin.ignore();
    __currentmode = mode::DEFAULT;
    __currentstate = state::DEFAULT;

    //Get ready to read the next terminal line
    display_terminal();
    string commandmode;
    string nextline;
    //absorb the next line in the terminal interpreter
    getline(cin, nextline); //Then get the rest of the line
    __history.push_back(commandmode+nextline); //Keep track of all history
    stringstream parsestream(nextline); //Put it into a stream to be parsed
    parsestream >> commandmode; //get first block in stream
    try{
        __currentmode = __modetree[commandmode];
//        cout << __modetree << endl;
    }catch(BPTIndexException& e){
        //Illegal input. Reset and read next line
        __currentmode = mode::DEFAULT;
        if(debug>=bugflag::heavy) cout << e.what() << endl;
        return;
    }
    //PARSETREE MIRACLES HAPPEN HERE
    while(parsestream){

    }
}
void mmysql::display_terminal(){
    cout << ">";
}
void mmysql::define_parsetree(){
    //Later, we do not want to access keys directly, we want to check if
    //  they exist;
    __modetree.set_not_default();
    for(mode i : modelist){
        __modetree.insert(get_mode_string(i), i);
    }
    //Set rules for parsing the line
    //For mode select
    int intmode = static_cast<int>(mode::select);
    __parsetree[intmode][]
}

//vector<vector<string> >& mmysql::select(vector<string>& fields){

//}
