#include "mmyshunting.h"

debugger DBG = debugger::heavy;

mmynode* parse_to_tree(const string& s){
    mmynode* root = nullptr;
    mmynode* child = nullptr;
    mmynode* notchild = nullptr;
    //boolean operator indeces, and comparison operator indeces
//    set<size_t> bool_indeces, op_indeces; //places where operators were found (starting index)
    size_t length = s.length();

    if(DBG >= debugger::light)
        cout << "[MMYSHUNTING] Starting parse_to_tree: "
             << s << endl;

    enum class state{empty, getbbool, getnextblock};

    state mystate = state::getnextblock;
    int spare_parentheses = 0;
    size_t p_left = 0; //left parentheses of that block
    size_t b_left = 0; //left block index
    for(size_t i = 0; i < length; i++){
        //traverse the string. Build a tree to represent the order of
        //operations of the boolean comparisons and unions/intersections
        if(spare_parentheses==0 && s[i] == '('){
            spare_parentheses++;
            p_left = i+1;
            continue; //skip the rest, we are only trying to find this block string
        }else if(spare_parentheses > 0){
            if(s[i] == '(')
                spare_parentheses++;
            else if(s[i] == ')')
                spare_parentheses--;
            b_left = i+1;
            mystate = state::getbbool; //after parentheses, expect bool
            if(spare_parentheses==0)
                if(!root){//if nullptr
                    if(DBG>=debugger::medium)
                        cout << "[MMYSHUNTING] P BLOCK: "
                             << s.substr(p_left, i-p_left) << endl;
                    child = parse_to_tree(s.substr(p_left, i-p_left));
                }else{
                    root->right = parse_to_tree(s.substr(p_left, i-p_left));
                    if(DBG>=debugger::medium)
                        cout << "[MMYSHUNTING] P BLOCK right root: "
                             << root->right->val << endl;
                }

            continue;
        }
        if(mystate == state::getnextblock){
            /** @todo add in NOT unary operation **/
            //Process the comparison operators
            string temp="";
            size_t index = 0;
            if((temp=parsetree::in_set(s, COMPARITORS, i))!=""){
                index = i;
                i += temp.size();
                string left = mmytrim(s, b_left, index);
                size_t i_left = i;
                bool hasword = false; //Move i up until the next word begins
                bool inquote = false;
                size_t quoteend = 0;
                for(i=i; i < length;i++){
                    if(!hasword && isspace(s[i])){
                        i_left++;
                        continue;
                    }
                    hasword = true;
                    if(s[i] == '\"'){
                        inquote ^= true;
                        if(inquote)
                            i_left = i+1;
                        else
                            quoteend = i;
                        continue;
                    }
                    if(!inquote && parsetree::in_set(s[i], PREOPERATORS)){
                        break;
                    }
                }
                string right;
                if(quoteend)
                    right = mmytrim(s, i_left, quoteend);
                else
                    right = mmytrim(s, i_left, i);
                mmynode* tempnode = new mmynode(temp);
                tempnode->ttype = tokentype::comparitor;
                tempnode->left = new mmynode(left);
                tempnode->right = new mmynode(right);
                tempnode->left->ttype = tokentype::fieldname;
                tempnode->right->ttype = tokentype::comparee;
                mystate = state::getbbool;
                if(root && !root->right)
                    root->right = tempnode;
                else
                    child = tempnode;
                if(DBG>=debugger::light)
                    cout << "[MMYSHUNTING] COMP BLOCK: " << left << ", "
                         << temp << ", " << right << endl;

//                continue;
            }
        }

        //Process the boolean operators
        string temp="";
        size_t index;
        if(parsetree::in_set(s[i],PREOPERATORS) &&
                (temp=parsetree::in_set(s, BBOOLEANS, i+1))!="" &&
                (i+temp.size() < length) &&
                parsetree::in_set(s[i+temp.size()+1], POSTOPERATORS)){
            index = i+1;
            i += temp.size();
            if(!root){//nullptr
                root = new mmynode(temp, child);
                root->ttype = tokentype::boolean;
                child = nullptr;
            }else{
                mmynode* tempnode = new mmynode(temp, root);
                root = tempnode;
                root->ttype = tokentype::boolean;
            }
            b_left = i+1;
            mystate = state::getnextblock;
            if(DBG>=debugger::light)
                cout << "[MMYSHUNTING] BOOL BLOCK: " << (root->left ? root->left->val:"None")
                     << ", " << root->val << ", "
                     << (root->right ? root->right->val:"None") << endl;
        }
    }
    if(!root){
        if(child)
            root = child;
        else {
            cout << "Failed myshunting return\n";
            assert(true);
        }
    }

    return root;
}

void mmynode::generate_ids(simple_map<string, multimap<string, unsigned long> >& map){

    if(ttype != tokentype::comparitor){
        if(left)
            left->generate_ids(map);
        if(right)
            right->generate_ids(map);
        return;
    }
    enum class comparee_type {str, ul, db, ll};
    //Generate ids
    double comparee_ul;
    comparee_type cflag = comparee_type::str;
    try{
        //Check for decimal and negative sign
        /** @todo **/
//        bool decimal = false;
//        bool negative = false;
//        if(right->val[0] == '-')
//            negative = true;
//        if(right->val.find('.', 1))
//            decimal = true;

        comparee_ul = stod(right->val);
        cflag = comparee_type::db;
    }catch(...){
        if(DBG>=debugger::heavy)
            cout << "Cannot string to double (stod): " << right->val << endl;
    }
    for(auto it = map[left->val].begin(); it != map[left->val].end(); it++){
        if(cflag==comparee_type::db){
            try{
//                unsigned long it_ul = std::stoul((*it).key);
                double it_db= std::stod((*it).key);
                if(compare_fields(it_db, comparee_ul, val)){
                    idnums += (*it).vec;
                }
                continue;
            }catch(...){
                if(DBG>=debugger::light)
                    cout << "[MMYNODE] Failed to compare as ul\n";
//                if(compare_fields((*it).key, right->val, val))
//                    idnums += (*it).vec;
//                continue;
            }
        }
        if(compare_fields((*it).key, right->val, val)){
            idnums += (*it).vec;
        }
    }
    //MAYBE not necessary, but just for cleanliness
    delete left;
    delete right;
    left = nullptr;
    right = nullptr;
}

set<unsigned long> mmynode::get_ids(){
    if(ttype == tokentype::boolean){
        if(val == "and"){
            return left->get_ids()*right->get_ids();
        }else if(val == "or"){
            return left->get_ids()+right->get_ids();
        }else if(val == "xor"){
            return left->get_ids()^right->get_ids();
        }
    }else{
        return idnums;
    }
}

set<unsigned long> mmynode::get_ids(simple_map<string, multimap<string, unsigned long> >& map){
    generate_ids(map);
    return get_ids();
}

string mmytrim(const string& s, size_t left, size_t right){
    while(isspace(s[left]))
        left++;
    while(isspace(s[right-1]))
        right--;
    //Then let's remove any parentheses surrounding it all
//    size_t p_left = 0; //left parentheses count
//    size_t p_right = 0;
//    if(s[left] != '(' && s[right-1] != ')')
    return s.substr(left, right-left);
}

//Compare lexicographically
bool compare_fields(const string&left, const string&right, const string &comparee){
    size_t left_size = left.size();
    size_t right_size = right.size();
    size_t smaller_size = (left_size<right_size ? left_size : right_size);

    if(comparee == "=" or comparee == "=="){
        return left == right;
    }else if(comparee == ">"){
        for(size_t i = 0; i < smaller_size; i++){
            if(left[i] != right[i])
                return left[i] > right[i];
        }
        //Then they are identical through the iteration
        //Determine based on size which is larger
        return (left_size != right_size ?
                    (left_size > right_size ? true:false):false);
    }else if(">="){
        for(size_t i = 0; i < smaller_size; i++){
            if(left[i] != right[i])
                return left[i] > right[i];
        }
        return (left_size >= right_size ? true:false);
    }else if("<"){
        for(size_t i = 0; i < smaller_size; i++){
            if(left[i] != right[i])
                return left[i] < right[i];
        }
        return (left_size != right_size ?
                    (left_size < right_size ? true:false):false);
    }else if("<="){
        for(size_t i = 0; i < smaller_size; i++){
            if(left[i] != right[i])
                return left[i] < right[i];
        }
        return (left_size <= right_size ? true:false);
    }else if("!=")
        return left != right;

    if(DBG>=debugger::none)
        cout << "Failed to reach a comparison, crashing...\n";
    assert(false);
}
