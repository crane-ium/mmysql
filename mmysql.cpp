/**
  * @file mmysql.cpp
  * @header mmysql.h
  */
#include "mmysql.h"

string DEFAULTRETURNFILE="returnfile.bin";

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
void mmysql::display_history() const{
    cout << "\n------ PAST HISTORY -------\n";
    size_t count = 0;
    for(auto it = __history.begin(); it != __history.end(); it++){
        cout << "[" << setfill('0') << setw(3) << count << "]  "
             << (*it) << endl;
    }
}
//Runs the processes, starting by getting the next line;
void mmysql::run(){
    debug = bugflag::heavy;
    //reset anything
//    cin.clear();
//    cin.ignore();
    __currentmode = mode::DEFAULT;
    __currentstate = state::DEFAULT;
    //lambda reset function for local variables
    auto reset = [&](){
        __currentmode = mode::DEFAULT;
        cin.clear();
        if(debug >= bugflag::light) cout << "[mmysql] reset(): There is a problem\n";
    };

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
    }catch(BPTIndexException& e){
        //Illegal input. Reset and read next line
        reset();
//        cin.ignore();
        if(debug>=bugflag::heavy) cout << e.what() << endl;
        return;
    }

    //Immediately, let's check for simple inputs, such as
    //  history, or exit
    /** @todo I don't like this. It defeats the purpose of
     * all the work I've put towards my own data structure,
     * but this temporarily works **/
    if(commandmode=="history"){
        display_history();
        /** @todo Add functionality for more history commands
         * eg. history -5 (shows the last 5 history inputs
         * eg. history -r 5 (repeats last 5 inputs) **/
        return;
    }else if(commandmode=="exit"){
        return;
    }

    //PARSETREE MIRACLES HAPPEN HERE
    string nextblock;
    multimap<token, string> shuntingqueue; //infix style shunting : contradictory!
    bool repeatingflag=true;
    rulepair rules;
    while(parsestream >> nextblock){
        if(debug >= bugflag::medium)
            cout << "nextblock: " <<static_cast<int>(__currentmode) << ":"
                 << nextblock << ":" << static_cast<int>(__currentstate) << endl;
        //We defined our __parsetree. Now let's use it
        rules = __parsetree[static_cast<int>(__currentmode)][__currentstate];
        auto it = rules.specifier.find(nextblock);
        cout << "Got here\n";
        //means it's only a syntax keyword
        if(!rules.specifier.empty()){
            if(it==rules.specifier.end()){ //verify if incorrect specifier
                if(debug>=bugflag::light) cout << "invalid rules specifier: " << nextblock << endl;
                reset();
                return;
            }
            __currentstate = rules.nextstate;
            continue;
        }
        //Else, we will analyze the data based on our rules
        //Iterate through all rules using ranged for loop
        bool invalid = false; //ends parse
        bool asteriskflag = false; //ends ranged loop early
        repeatingflag = false; //does not go to nextstate yet
        size_t length = nextblock.size(); //hold so we don't repeat calculate
        cout << "Now here\n";
        for(sf rule : sflist){
            switch(rule){
            case sf::allowasterisk:
                if(rule==rules.flags && nextblock == "*"){
                    asteriskflag=true;
                }
                continue;
            case sf::allowcomma:
                if(nextblock[length-1]==','){
                    if(!(rule==rules.flags)){
                        invalid = true;
                        reset();
                        return;
                    }
                    repeatingflag=true;
                    nextblock = nextblock.substr(0, length-1);
                    length-=1;
                }
                continue;
            case sf::allowquotes:
                if(nextblock[0]=='\"' && nextblock[length-1]=='\"'){
                    if(!(rule==rules.flags)){
                        invalid=true;
                        reset();
                        return;
                    }
                    nextblock = nextblock.substr(1, length-2);
                    length-=2;
                }
                continue;
            case sf::getall:
                if(rule == rules.flags){
                    string temp;
                    getline(parsestream, temp);
                    nextblock += " " + temp;
                }
                continue;
            case sf::none:
            case sf::allowstring:
            case sf::repeatable:
                continue;
            default:
                if(debug >= bugflag::light) cout << "[mmysql] hit a default in switch\n";
                invalid = true;
                reset();
                return;
            }
            if(asteriskflag || invalid) //end early if asterisk
                break;
        }
        //Set the next state if didn't have a repeatable
        if(!repeatingflag)
            __currentstate = rules.nextstate;

        cout << "Now here\n";
        cout << shuntingqueue << endl;
        shuntingqueue[rules.type] += nextblock;
        cout << shuntingqueue << endl;
        cout << "End here\n";
    }
    if(!rules.valid){
        if(debug>= bugflag::light) cout << "[mmysql] Invalid rule\n";
        reset();
        return;
    }
    if(debug >= bugflag::medium)
        cout << "Final shunting queue: " << shuntingqueue << endl;

    //Quick lambda to test for file existance
    auto file_exists = [](const string& filename){
        ifstream checker(filename+".bin");
        bool exists = checker.good();
        checker.close();
        return exists;
    };

    //If we have reached here, we are ready to turn the lines into
    //  commands an make tables/make changes
    switch(__currentmode){
    case mode::select:{
        /** @todo Could make this more robust here, by cleaning up directory controls **/
        string temptablename = shuntingqueue[token::tablename][0];
        if(!file_exists(temptablename)){
            cout << "ERROR 355: TABLE FILE DOES NOT EXIST\n";
            return;
        }
        mmytable* temp = new mmytable(temptablename);
        __database.insert(temptablename, temp);
        fstream selector(DEFAULTRETURNFILE, ios::binary|ios::out|ios::ate);
        auto get_string= [&](){
            if(shuntingqueue[token::constraint].empty())
                return (string)"";
            else
                return shuntingqueue[token::constraint][0];
        };
        if(shuntingqueue[token::fieldname][0] == "*")
            temp->select(selector, get_string());
        else
            temp->select(selector, get_string(), shuntingqueue[token::fieldname]);
        cout << "fields: " << shuntingqueue[token::fieldname] << endl;
        /** @todo Get that file written up and read from it **/
        break;
    }
    case mode::create:
    case mode::make:{
        //Check if file exists first
        string temptablename = shuntingqueue[token::tablename][0];
        if(file_exists(temptablename)){ //table already exists, exit.
            cout << "ERROR 998: CANNOT CREATE: FILE ALREADY EXISTS\n";
            break;
        }
        /** @todo Add create functions for mmytable **/
        mmytable* temp = new mmytable(temptablename);
        __database.insert(temptablename, temp);
        if(debug >= bugflag::medium)
            cout << "[mmysql] Creating " << temptablename << ". \n";
        break;
    }
    case mode::insert:{

        break;
    }
    case mode::start:
    default:
        if(debug>=bugflag::light) cout << "[mmysql] Invalid __currentmode, cannot parse\n";
        break;
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
        __parsetree.push_back(static_cast<int>(i));
    }
    //Set rules for parsing the line
    //For mode select
    /** @example select * from students where lname=this and fname=that **/
    int intmode = static_cast<int>(mode::select);
    __parsetree[intmode][state::start] =
            rulepair(state::getfields, token::fieldname, sf::allowasterisk|
                     sf::repeatable|sf::allowcomma|sf::allowquotes);
    __parsetree[intmode][state::getfields] =
            rulepair(state::fromkey, token::none, sf::DEFAULT,set<string>{"from"});
    __parsetree[intmode][state::fromkey] =
            rulepair(state::gettable, token::tablename,
                     sf::allowquotes|sf::allowstring, set<string>(), true);
    __parsetree[intmode][state::gettable] =
            rulepair(state::wherekey, token::none, sf::getall,set<string>{"where"});
    __parsetree[intmode][state::wherekey] =
            rulepair(state::getconstraint, token::constraint,
                     sf::getall, set<string>(), true);

    intmode = static_cast<int>(mode::create);
    __parsetree[intmode][state::start] =
            rulepair(state::tablekey, token::none, sf::DEFAULT, set<string>{"table"});
    __parsetree[intmode][state::tablekey] =
            rulepair(state::gettable, token::tablename, sf::allowquotes);
    __parsetree[intmode][state::gettable] =
            rulepair(state::fieldskey, token::none, sf::DEFAULT, set<string>{"field", "fields"});
    __parsetree[intmode][state::fieldskey] =
            rulepair(state::getfields, token::fieldname, sf::allowcomma|
                     sf::allowquotes|sf::repeatable, set<string>(), true);

    intmode = static_cast<int>(mode::make); //synonymous with create
    __parsetree[intmode] = __parsetree[static_cast<int>(mode::create)];

    intmode = static_cast<int>(mode::insert);
    __parsetree[intmode][state::start] =
            rulepair(state::intokey, token::none, sf::DEFAULT, set<string>{"into"});
    __parsetree[intmode][state::intokey] =
            rulepair(state::gettable, token::tablename, sf::allowquotes);
    __parsetree[intmode][state::gettable] =
            rulepair(state::valueskey, token::none, sf::DEFAULT, set<string>{"value", "values"});
    __parsetree[intmode][state::valueskey] =
            rulepair(state::getvalues, token::fieldname, sf::allowcomma|
                     sf::allowquotes|sf::repeatable, set<string>(), true);
}

//vector<vector<string> >& mmysql::select(vector<string>& fields){

//}
