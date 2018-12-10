/**
  * @file mmytable.cpp
  * @header mmytable.h
  */
#include "mmytable.h"

char DELIMITER = '|';
string RETURNFILENAME = "returnfile";
string FILESUFFIX = ".bin";

mmytable::mmytable(){

}
mmytable::mmytable(const string& filelocation)
    : __name(filelocation), __dir(""){

    read_file();
}

mmytable::mmytable(const string& table_name, const string &dir)
    : __name(table_name), __dir(dir)
{
    //Based on the table_name, open that file and read all the entries
    // into index tables, so that queries are able to effectively find
    // intersections/unions of those index requirements

    read_file();
}
//Create a file and establish our record class
bool mmytable::create(bool overwrite){
    //Overwrites or avoids deleting old data
    if(overwrite && !__init || !__init && !__fields.empty() &&
            !mmyhelper::file_exists(__dir+__name)){
        __init=true;
        /** @todo Perhaps add an easy template for putting a map
         * into a vector **/
        vector<string> fieldnames;
        for(auto it = __fields.begin(); it != __fields.end(); it++)
            fieldnames.push_back((*it).val);
        writer newfile;
        bool flag= newfile.write(__dir+__name, fieldnames); //write file
        read_file();
        return flag;
    }else if(!__init && !__fields.empty()){
        cout << "[mmytable] Cannot create " << __dir+__name
             << ": Already exists\n";
        return false;
    }
    if(__fields.empty())
        cout << "[mmytable] Cannot create: Missing fields\n";
    if(__init)
        cout << "[mmytable] Cannot create: Already initialized.\n";
    return false;
}
//Reads the file into our table
bool mmytable::read_file(){
    if(!mmyhelper::file_exists(__dir+__name))
        return false;
    if(!rec.set_file(__dir+__name))
        return false;

    __init = true;
    __delimiter = rec.get_delimiter();

    field_add_all(rec.get_fieldnames());

    //put the file into __itable
    while(rec.more()){
        string temp = rec.next();
        parse(temp, __delimiter);
    }
    return true;
}
void mmytable::init(const string &table_name, const string& dir){
    __dir = dir;
    __name = table_name;
}

void mmytable::field_add_all(const vector<string>& fieldnames){
    for(size_t i = 0; i < fieldnames.size(); i++){
        field_add(fieldnames[i]);
    }
}

void mmytable::field_add(const string &field_name){
    if(__init) //Block more fields from being added
        return;
    size_t fsize = __itables.size();
    __itables.insert(field_name);
    __fields[fsize] = field_name; //Associate the column to field. For file read/write
//    __field_ids[field_name] = fsize;
}

void mmytable::insert(vector<string>& fieldnames){
    if(!__init) //block inserts if it's not initialized
        return;
    unsigned long line = rec.insert(fieldnames);
    parse(rec.get_line(line), __delimiter);
}

/**
 * @brief mmytable::parse   : Turn a given file-line into __itables data
 * @param fileline          : string to be parsed
 * @param length            : Length of string
 * @param delimiter         : Delimiter to seperate substrings
 */
void mmytable::parse(const string &fileline, const char delimiter){
    //Split by a delimiter
    //The first substring is the id-number. Take and turn into int for better comparisons later
    //eg. "123|foo|bar|    Stephen| Crane | all one field|"
    enum class lineflag {start, end};
    lineflag flag = lineflag::start;
    mmyint length = fileline.size();
    size_t i_start=0; //start/end of substring
    size_t count = 0;
    string subs;
    mmyint lineid; //id value prefixed at start of the string

    //read from file into tables
//    cout << fileline << endl;
    for(mmyint i = 0; i < length; i++){
        if(fileline[i] == delimiter){
            subs = fileline.substr(i_start, i - i_start);
            //Put them into their fields
            if(count==0){
                lineid = stoul(subs);
            }else{
                __itables[__fields[count-1]][subs] += lineid;
            }
//            flag=lineflag::start; //change flag back
            i_start = i+1;
            count++;
            continue;
        }
    }
}

//return a vector of strings, parsed from a fileline syntax, and based on set
vector<string> mmytable::vector_parse(const string& fileline,
                                      bool wildcard,
//                                      const vector<mmyint>& ordering,
                            const vector<mmyint>& ordering){
    //Again, we assume it holds the proper format of:
    //"123|Crane|Stephen|etc|"
    //->vector<string> of {"Crane", "Stephen", "etc"}
    //set indicates which fields to take
    //an empty set indicates take ALL fields
    mmyint length = fileline.size();
    size_t i_start=0; //start/end of substring
    mmyint count = 0;
    string subs; //temp substring between delimiters
    vector<string> results;
    simple_map<mmyint, string> fieldvalues;

    if(ordering.empty() && !wildcard)
        return results;

    for(mmyint i = 0; i < length; i++){
        //fieldset starts at 0, and links with field 0 (not id#)
        if(count == 0 && fileline[i] != DELIMITER)
            continue;
        else if(count==0){
            count++;
            i_start = i+1;
            continue;
        }
        if(fileline[i] == DELIMITER){
            //Check if field is in constraints set
            if(ordering.empty()){
                subs = fileline.substr(i_start, i - i_start);
                results.push_back(subs);
            }else{
                subs = fileline.substr(i_start, i - i_start);
                fieldvalues[count-1] = subs;
            }
            i_start = i+1;
            count++;
            continue;
        }
    }
    if(!ordering.empty()){
        results.resize(ordering.size());
        for(size_t i = 0; i < ordering.size(); i++){
            results[i] = fieldvalues[ordering[i]];
        }
        if(debug >= bugflag::medium)
            cout << "[mmytable] specified results: " << results << endl
                 << "fv: " << fieldvalues << endl
                 << "results: " << results << endl;
    }
    return results;
}

//Takes field constraints. Returns 2d table of relevant data.
//if fields is an empty vector, then we return all fields
void mmytable::select(fstream& filestream,
                      const string& constraints,
                      vector<string> fields){
    //Constraint format:
    //"where "fieldname >= comparee and fieldname2 = comparee2...""
    //Generate our constraint processing tree
    mmyshunting constraint_processor(constraints);

//    constraint_processor.print();
    set<mmyint> idnums = constraint_processor.get_ids(__itables);
//    constraint_processor.print();

    //Quick map representing string:column# for fields
    simple_map<string, mmyint> temp_field_ids;
    for(auto it=__fields.begin();it != __fields.end(); it++){
        temp_field_ids[(*it).val] = (*it).key;
    }

    //if the vector fields is empty, then return all fields
    //This is for wildcard (*) calls
    if(fields.empty()){
        //In this block:
        //  Create control vector for the fieldnames
        //  Build file header of the fields
        //  Insert the matched fields/id#s into the new temporary return-file
        vector<string> fieldnames;
        for(auto it = __fields.begin(); it != __fields.end(); it++)
            fieldnames.push_back((*it).val);
        filestream << DELIMITER; //start the file with the delimiter
        //Use helper function to insert vector of fieldnames into a fstream
        mmyhelper::stream_vecstring(filestream, fieldnames);
        //Ordering for the vector_parser
        for(auto it = idnums.begin(); it != idnums.end(); it++){
            vector<string> v = vector_parse(rec.get_line((*it)),true);
            //rearrange the given vector to match the proper ordering
            mmyhelper::stream_vecstring(filestream, v, true);
        }
    }else{
        //Given field parameters,
        //  find the associated fields to pull from the file
        //Build a quickmap so we can get our fields and put them inorder
        simple_map<mmyint, string> field_set;

        vector<mmyint> fieldorder; //Give it to reader so you put fields in order
        for(auto it = fields.begin(); it != fields.end(); it++){
            if(temp_field_ids.exists((*it)))
                fieldorder.push_back(temp_field_ids[(*it)]);
        }
        //Now that we have a clean temporary memoization, let's
        //  build a quick vector of fields
        for(auto it=fields.begin();it!=fields.end();it++){
            if(temp_field_ids.exists(*it))
                field_set.insert(temp_field_ids[*it]);
            else{
                //Remove invalid fields from our constraints?
                fields.erase(it);
                it--; //Erase it, so we must backtrack one so we can
                //do it++ in the for loop and not break code
            }
        }
        filestream << DELIMITER; //start the file with the delimiter
        mmyhelper::stream_vecstring(filestream, fields); //write our fields to file
        if(debug>=bugflag::light)
        cout << "Fields: " << field_set << endl
             << "temp fields: " << temp_field_ids << endl
             << "fieldorder: " << fieldorder << endl;
        for(auto it=idnums.begin(); it != idnums.end(); it++){
            vector<string> v = vector_parse(rec.get_line((*it)),
                                            false, fieldorder);
//            if(fieldorder.size() != v.size())
//                cout << "fieldorder.size() != v.size(): " << endl
//                     << "fieldorder: " << fieldorder << endl
//                     << "v: " << v << endl;
//            assert(fieldorder.size() == v.size());
            mmyhelper::stream_vecstring(filestream, v, true);
        }
    }
}
//Select * From ... where constraints
//Based on constraints. Returns all fields data.
//multimap<mmyint, string>& mmytable::select(const string& constraints){
//    multimap<mmyint, string> selection;


//    return selection;
//}

//----------------------------------------------------------------
//HELPER FUNCTIONS
//----------------------------------------------------------------

//Put a vector of strings into a filestream in our format
void mmyhelper::stream_vecstring(fstream& filestream,
                                 const vector<string>& vstr,
                                 bool write_idflag){
    //If the writer wants to write the ID# at the start of the line
    //  fetch the filestream cursor index and use that as new ID#
    if(write_idflag){
        filestream << filestream.tellg() << DELIMITER;
    }
    for(auto vit = vstr.begin(); vit != vstr.end(); vit++){
        filestream << *vit << DELIMITER;
    }
    filestream << '\n';
}

void mmyhelper::print_table(const vector<vector<string> >& table,
                            const string& tablename,
                            const vector<size_t>& column_width){
    /** @example
     * Table name: tablename, Records: 2
     * record no.   lname       fname       age
     *      0       Crane       Stephen     27
     *      1       Barkeshli   Sassan      100
     * **/
    cout << "Table name: " << tablename
         << ", Records: " << table.size()-1;
    size_t record_count = 0, count=0;
    cout << right;
    for(auto i = table.begin(); i != table.end(); i++){
        cout << endl;
        if(i==table.begin()){
            cout << setw(8) << "record";
        }else{
            cout << setw(8) << record_count;
            record_count++;
        }
        count=0;
        for(auto j = i->begin(); j != i->end(); j++){
            cout << setw(column_width[count]+2) << (*j);
            count++;
        }
        if(i==table.begin())
            cout << endl;
    }
    cout << endl;
}

bool mmyhelper::file_exists(const string& filename){
    ifstream checker(filename+FILESUFFIX);
    bool exists = checker.good();
    checker.close();
    return exists;
}

//Get a vector of all id#s that pass the comparison
//vector<mmyint>& vector_filter(const string& fieldname,
//                              const string& op,
//                              const string& comp){

//}
