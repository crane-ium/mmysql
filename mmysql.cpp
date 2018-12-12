/**
  * @file mmysql.cpp
  * @header mmysql.h
  */
#include "mmysql.h"

string DEFAULTRETURNFILE="returnfile";

//Default constructor for mmysql querying controls
mmysql::mmysql(){
    cout << "INITIALIZING MMYSQL(TM) DATABASE QUERY INTERPRETER...";
    cout << "INITIALIZED!\n";
    cout << "PREPARING INTERPRETER...\n";
    cout << "~~~WELCOME TO MMYSQL(TM) DATABASE QUERY INTERPRETER~~~\n";
    define_parsetree();
}

mmysql::~mmysql(){
    for(auto it=__database.begin(); it!= __database.end(); it++){
        delete (*it).val;
    }
}

void mmysql::start(){
    __currentmode == mode::DEFAULT;
    while(__currentmode != mode::exit)
        interpret(cin);
}
void mmysql::display_history() const{
    cout << "\n------ PAST HISTORY -------\n";
    size_t count = 0;
    for(auto it = __history.begin(); it != __history.end(); it++){
        cout << "[" /*<< setfill('0') << setw(3)*/ << count << "]  "
             << (*it) << endl;
        count++;
    }
}
void mmy::history_directory::display_history() const{
    cout << "\n------ PAST HISTORY -------\n";
    size_t count = 0;
    for(auto it = data.begin(); it != data.end(); it++){
        cout << "[" << (int(log10(data.size()))<3 ? setw(3) : setw(int(log10(data.size()))+1));
        if((*it).type > mmy::historytype::unknown){
            cout << setfill('0') << count;
            count++;
        }else
            cout << setfill('X') << "";
        cout << "]  " << (*it) << endl;
    }
    cout << endl;
//    cout << "[END]" << endl;
}
void mmy::history_directory::set_lasttype(mmy::historytype t){
    if(data.empty())
        return;
    (*(data.end()-1)).type = t;
}
bool mmy::history_directory::insert(const string &s, mmy::historytype t){
    data.push_back(mmy::history_dataunit(s, t));
    return true;
}

//Runs the line in our mmysql terminal, starting by getting the next line;
void mmysql::interpret(istream& ss){
//    debug = bugflag::heavy;
    __currentmode = mode::DEFAULT;
    __currentstate = state::DEFAULT;
    //lambda reset function for local variables
    auto reset = [&](){
        __currentmode = mode::DEFAULT;
        ss.clear();
        if(debug >= bugflag::light) cout << "[mmysql] reset(): There is a problem\n";
    };

    //Get ready to read the next terminal line
    display_terminal();
    string commandmode;
    string nextline;
    //absorb the next line in the terminal interpreter
    getline(ss, nextline); //Then get the rest of the line
//    __history.push_back(commandmode+nextline); //Keep track of all history
    __history_db.insert(commandmode+nextline);
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
    switch(__currentmode){
    case mode::history:
        /** @todo Add functionality for more history commands
         * eg. history -5 (shows the last 5 history inputs
         * eg. history -r 5 (repeats last 5 inputs) **/
        __history_db.display_history();
        return;
    case mode::exit:
        return;
    default:
        break;
    }

    //PARSETREE MIRACLES HAPPEN HERE
    string nextblock="";
    multimap<token, string> shuntingqueue; //infix style shunting : contradictory!
    bool repeatingflag=true;
    rulepair rules;
    size_t length=0;
//    noskipws(parsestream); //Full parsing of string
    parsestream.ignore(256, ' ');
    //Get next block. Only obstacle are quotes
    auto get_next_block = [&](){
        //local functions
        auto ss_inc = [&](){
            parsestream.seekg(parsestream.tellg()+1);
        };
        auto pk = [&](){
            return parsestream.peek();
        };

        while(isspace(pk()) && parsestream)
            ss_inc();
        bool inquotes = pk()=='\"';
        bool backslashflag = false;
        bool doneflag = false;
        size_t leftit = parsestream.tellg();

//        cout << "Start: " << (char)pk() << endl;
        if(inquotes) ss_inc();
        while(parsestream && !doneflag){
            switch(pk()){
            case '\\':
                backslashflag=true;
//                it++;
                continue;
            case '\"':
                if(inquotes && !backslashflag){
                    ss_inc();
                    doneflag = true;
                    if((pk()) == ',')
                        ss_inc();
                }
                break;
            case '\n':
            case ' ':
                if(!inquotes){
                    doneflag=true;
                }
            break;
            case ',':
                if(!inquotes){
                    ss_inc();
                    doneflag=true;
                }
                break;
            default:
                break;
            }
            if(doneflag)
                break;
            backslashflag=false;
            ss_inc();
        }
        length = parsestream.tellg()-leftit;
        nextblock = string(parsestream.str().substr(leftit,
                                                    length));
        while((pk()==' ' || pk()=='\n') && parsestream)
            ss_inc();
    };
    while(parsestream.tellg() != -1){
        get_next_block();
        if(debug >= bugflag::medium)
            cout << "nextblock: " <<static_cast<int>(__currentmode) << ":"
                 << nextblock << ":" << static_cast<int>(__currentstate) << endl;
        //We defined our __parsetree. Now let's use it
        rules = __parsetree[static_cast<int>(__currentmode)][__currentstate];
        auto it = rules.specifier.find(nextblock);
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
        for(sf rule : sflist){
            switch(rule){
            case sf::allowasterisk:
                if(rule==rules.flags && nextblock == "*"){
                    asteriskflag=true;
                }
                continue;
            case sf::allowcomma:
//                cout << "Comma: " << nextblock << ":" << (*(nextblock.end()-1))<< endl;
                if((*(nextblock.end()-1))==','){
                    if(!(rule==rules.flags)){
                        invalid = true;
                        reset();
                        return;
                    }
                    repeatingflag = true;
                    nextblock.erase(nextblock.end()-1); //erase comma
                    length--;
                }
                continue;
            case sf::allowquotes:
                if(nextblock[0]=='\"' && (*(nextblock.end()-1))=='\"'){
                    if(!(rule==rules.flags)){
                        invalid=true;
                        reset();
                        return;
                    }
                    nextblock.erase(nextblock.begin());//erase quotes
                    nextblock.erase(nextblock.end()-1);
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

        shuntingqueue[rules.type] += nextblock;
//        cout << shuntingqueue << endl;
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
        ifstream checker(filename+".bin", ios::binary);
        bool exists = checker.good();
        checker.close();
        return exists;
    };
    auto txt_file_exists = [](const string& filename){
        ifstream checker(filename+".txt");
        bool exists = checker.good();
        checker.close();
        return exists;
    };

    //If we have reached here, we are ready to turn the lines into
    //  commands an make tables/make changes
    switch(__currentmode){
        //SELECTION
    case mode::select:{
        /** @todo Could make this more robust here, by cleaning up directory controls **/
        string temptablename = shuntingqueue[token::tablename][0];
        if(!file_exists(temptablename)){
            cout << "ERROR 355: TABLE FILE DOES NOT EXIST\n";
            return;
        }
        mmytable* temp;
        if(!__database.exists(temptablename)){
            temp= new mmytable(temptablename);
            __database.insert(temptablename, temp);
        }else
            temp = __database[temptablename];
        fstream selector(DEFAULTRETURNFILE+".bin", ios::binary|ios::out|ios::ate);
        if(!selector.is_open()){
            cout << "[mmysql] Failed to open selector\n";
            return;
        }
        //Returns the correct string for constraint
        auto get_string= [&](){
            if(shuntingqueue[token::constraint].empty())
                return (string)"";
            else
                return shuntingqueue[token::constraint][0];
        };
        if(shuntingqueue[token::fieldname][0] == "*") //check wildcard value
            temp->select(selector, get_string());
        else
            temp->select(selector, get_string(), shuntingqueue[token::fieldname]);
        if(debug>=bugflag::medium)
            cout << "fields: " << shuntingqueue[token::fieldname] << endl;
        //Now: Requested data written to file
        //Let's simply display the data given back to us.
        selector.close(); //close old stream
        //Prepare to read using the record parsing
        record reader(DEFAULTRETURNFILE, temp->get_delimiter());
        vector<vector<string> > select_table;
        select_table.push_back(reader.get_fieldnames());
        while(reader.more()){
            auto temp = reader.next_vector();
            if(!temp.empty())
            select_table.push_back(temp);
        }
        vector<size_t> column_max = reader.longest();
        mmyhelper::print_table(select_table,
                               temptablename,
                               column_max);
        __history_db.set_lasttype(mmy::historytype::select);
        break;
    }
        //FORCEFUL CREATION
    case mode::forcecreate:
    case mode::forcemake:
        //Like create, but forcefully overwrites the basefile
        /** @todo AS OF NOW, to fit barkeshli's batch files,
         * I would rather not utilize this **/
        //CREATION
    case mode::create:
    case mode::make:{
        //Check if file exists first
        string temptablename = shuntingqueue[token::tablename][0];
//        if(file_exists(temptablename)){ //table already exists, exit.
//            cout << "[ERROR 998] CANNOT CREATE: FILE ALREADY EXISTS\n";
//            return;
//        }
        //Create a new table and fill it with our new fields
        mmytable* temp = new mmytable;
        temp->init(temptablename);
        __database.insert(temptablename, temp);
        temp->field_add_all(shuntingqueue[token::fieldname]);
        bool created = temp->create(true);
        temp->read_file();
        if(created)
            cout << "\nMMYSQL: Successfully created table " << temptablename << endl;
        else
            cout << "\nMMYSQL: Failed to create table " << temptablename << endl;
        if(debug >= bugflag::medium)
            cout << "[mmysql] Creating " << temptablename << ". \n";
        __history_db.set_lasttype(mmy::historytype::make);
        return;
    }
        //INSERTION
    case mode::insert:{
        string temptablename = shuntingqueue[token::tablename][0];
        if(!file_exists(temptablename)){
            cout << "[ERROR 1234] CANNOT INSERT: FILE DOES NOT EXIST\n";
            return;
        }
        mmytable* temp;
        if(!__database.exists(temptablename)){
            temp = new mmytable(temptablename);
            __database.insert(temptablename, temp);
        }else{
            temp = __database[temptablename];
        }
        temp->insert(shuntingqueue[token::fieldname]);
        cout << "\nMMYSQL: Successfully inserted\n";
        __history_db.set_lasttype(mmy::historytype::insert);
        break;
    }
        //BATCHES A FILE INTO OUR SQL
    case mode::batch:{
        string temptablename = shuntingqueue[token::tablename][0];
        if(!txt_file_exists(temptablename)){
            cout << "[ERROR HAHAHA] CANNOT BATCH: FILE DOES NOT EXIST\n";
            return;
        }
        size_t iterations = 1;
//        cout << shuntingqueue[token::number] << endl;
        if(shuntingqueue.exists(token::number)){
            try{
                cout << "Gonna try?\n";
                iterations = static_cast<unsigned long>(stod(shuntingqueue[token::number][0]));
                cout << "iterations: " << iterations << endl;
            }catch(...){
                cout << "Batch: Failed to handle given number\n";
            }
        }
        run_batch(temptablename, iterations);
        __history_db.set_lasttype(mmy::historytype::batch);
    }
        break;
    case mode::start:
    default:
        if(debug>=bugflag::light) cout << "[mmysql] Invalid __currentmode, cannot parse\n";
        break;
    }
}
void mmysql::display_terminal(){
//    cout << "[" << linecount << "]>";
    cout << ">";
//    linecount++;
}
void mmysql::run_batch(const string& filename, size_t iterations){

    for(size_t i = 0; i < iterations; i++){
        ifstream batchreader(filename+".txt");
        if(!batchreader.is_open())
            return;
    //    cin.clear();
        string templine;

        while(batchreader){
            getline(batchreader,templine);
            istringstream tempss(templine);
            cout << templine << endl;
            //        cin.rdbuf(tempss.rdbuf());
            interpret(tempss);
        }
    }
    cout << "MMYSQL Batch complete\n";
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
            rulepair(state::tablekey, token::none,
                     sf::DEFAULT, set<string>{"table"});
    __parsetree[intmode][state::tablekey] =
            rulepair(state::gettable, token::tablename, sf::allowquotes);
    __parsetree[intmode][state::gettable] =
            rulepair(state::fieldskey, token::none,
                     sf::DEFAULT, set<string>{"field", "fields"});
    __parsetree[intmode][state::fieldskey] =
            rulepair(state::getfields, token::fieldname, sf::allowcomma|
                     sf::allowquotes|sf::repeatable, set<string>(), true);

    intmode = static_cast<int>(mode::make); //synonymous with create
    __parsetree[intmode] = __parsetree[static_cast<int>(mode::create)];
    intmode = static_cast<int>(mode::forcecreate); //Force file overwrite
    __parsetree[intmode] = __parsetree[static_cast<int>(mode::make)];
    intmode = static_cast<int>(mode::forcemake); //synonymous with create
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
    intmode = static_cast<int>(mode::batch);
    __parsetree[intmode][state::start] =
            rulepair(state::fromkey, token::none, sf::DEFAULT, set<string>{"from"});
    __parsetree[intmode][state::fromkey] =
            rulepair(state::gettable, token::tablename, sf::allowquotes,
                     set<string>(), true);
    __parsetree[intmode][state::gettable] =
            rulepair(state::getquantity, token::number, sf::none,
                     set<string>(), true);
}

//vector<vector<string> >& mmysql::select(vector<string>& fields){

//}
