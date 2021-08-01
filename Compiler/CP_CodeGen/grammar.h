//
//  grammar.h
//  SemanAna
//
//  Created by 蔡嘉轩 on 2021/6/15.
//

#ifndef grammar_h
#define grammar_h

#include "lexical.h"

// 用于表示语法分析树中的节点
class Node{
    
public:
    //当前节点代表的符号
    string symbol;
    string start_pos;
    string end_pos;
    //后加的属性，用于graphviz画图
    int id;
    static int node_ptr;

    //当前节点的子节点列表，用于判断叶节点时使用和输出时使用
    vector<Node> children;
    
    Node(){
        symbol = "Root";
        start_pos = "";
        end_pos = "";
        id = ++node_ptr;
    }
    
    Node(string s){
        symbol = s;
        start_pos = "";
        end_pos = "";
        id = ++node_ptr;
    }
    
    Node(string s, string st, string ed){
        symbol = s;
        start_pos = st;
        end_pos = ed;
        id = ++node_ptr;
    }
    
    // 是否为叶节点（终结符）
    bool is_leaf() const{
        //没有子节点说明是叶节点
        if(children.size() == 0)
            return true;
        else
            return false;
    }
    
    void add_child_node(Node child){
        children.push_back(child);
    }
};

int Node::node_ptr = 0;

//代表语法树及语法分析器
class Grammar{
    
public:
    //根节点
    Node S;
    
    //符号流，由词法分析器得到
    vector<vector<string>> lex_token_stream;
    Grammar(vector<vector<string>> &stream, int nums){
        lex_token_stream = stream;
        S = Node("S",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        symbol_ptr = 0;
        Program(nums);
        error_output();
    }
    
    //递归方式向文件输出语法树
    void output_tree(string file_path){
        ofstream f(file_path);
        output_node(f, S, 0);
        f.close();
        
    }
    
    //输出单个节点
    void output_node(ofstream &f, Node &node, int column){
        f << "|";
        for(int i = 0; i<column; i++){
            f <<"    |";
        }
        f << "---" << node.symbol << endl;
        if(!node.is_leaf()){
            for (int i = 0; i < node.children.size(); i++){
                output_node(f, node.children[i], column + 1);
            }
        }
    }
    
    //输出graphviz可用的代码
    void output_graphviz(ofstream &f, Node &node){
        if(!node.is_leaf()){
            f << "node" << node.id << "[label=" <<"\"\\" <<node.symbol << "\"];"<<endl;
            for(int i = 0; i < node.children.size(); i++){
                f << "node" << node.children[i].id << "[label=" <<"\"\\" <<node.children[i].symbol << "\"];"<<endl;
            }
            f << endl;
            for(int i = 0; i < node.children.size(); i++){
                f << "node" << node.id << " -> " << "node" << node.children[i].id << ";" <<endl;
            }
            f << endl;
            for(int i = 0; i < node.children.size(); i++){
                output_graphviz(f, node.children[i]);
            }
        }
    }
    
    
private:
    //指向当前符号的指针
    int symbol_ptr;
    
    
    
    //存放语法错误
    vector<vector<string>> gram_error_stream;

    //获取当前符号
    string cur_sym(){
        return lex_token_stream[symbol_ptr][0];
    }
    
    //提前获取下一个符号
    string next_sym(){
        return lex_token_stream[symbol_ptr + 1][0];
    }
    
    //语法错误的输出
    void error_output(){
        if(gram_error_stream.size() == 0)
            cout<<"Compiled Successfully, 0 grammar error founded,";
        else{
            cout<<"Compiled Successfully, "<<gram_error_stream.size()<<" grammar errors founded:"<<endl<<endl;
            for(auto i : gram_error_stream){
                cout<<"[From "<<i[1]<<" to "<<i[2]<<"] "<<i[0]<<"."<<endl;
            }
        }
    }
    
    //对匹配型关键语法错误易发点进行匹配，如果没有语法错误，则可以顺利加入语法树；如果发生语法错误，则将应该存在的符号加入语法树，并且进行错误标记。
    void pair(Node &node, string sym){
        if(cur_sym() == sym){
            node.add_child_node(Node(sym));
            symbol_ptr ++;
        }
        else{
            node.add_child_node(Node("Error("+sym+")"));
            gram_error_stream.push_back(vector<string>{"ERROR: Missing \"" + sym +  + "\" after \"" + cur_sym() + "\"", lex_token_stream[symbol_ptr][1], lex_token_stream[symbol_ptr][2]});
        }
    }
    
    //对非法语句型语法错误进行标记
    void illegal(Node &node){
        node.add_child_node(Node("Illegal Statement"));
        gram_error_stream.push_back(vector<string>{"ERROR: Illegal Statement", lex_token_stream[symbol_ptr][1], lex_token_stream[symbol_ptr][2]});
    }
    
    //判断是类型
    bool is_type(){
        return (cur_sym()=="int" || cur_sym()=="char" || cur_sym()=="bool" || cur_sym()=="void" || cur_sym()=="string" || cur_sym()=="double" || cur_sym()=="float");
    }
    
    //判断是一元运算符
    bool is_unary_operator(){
        return (cur_sym() == "!" || cur_sym() == "~" || cur_sym() == "-" || cur_sym() == "++" || cur_sym() == "--");
    }
    
    //判断是二元运算符
    bool is_binary_operator(){
        return (cur_sym() == "||" || cur_sym() == "&&" || cur_sym() == "|" || cur_sym() == "^" || cur_sym() == "&" || cur_sym() == "!=" || cur_sym() == "==" || cur_sym() == ">" || cur_sym() == ">=" || cur_sym() == "<=" || cur_sym() == "<" || cur_sym() == "<<" || cur_sym() == ">>" || cur_sym() == "+" || cur_sym() == "-" || cur_sym() == "*" || cur_sym() == "/" || cur_sym() == "%");

    }
    
    //重载上面的函数
    bool is_binary_operator(string sym){
        return (sym == "||" || sym == "&&" || sym == "|" || sym == "^" || sym == "&" || sym == "!=" || sym == "==" || sym == ">" || sym == ">=" || sym == "<=" || sym == "<" || sym == "<<" || sym == ">>" || sym == "+" || sym == "-" || sym == "*" || sym == "/" || sym == "%");

    }
    
    //判断是标识符
    bool is_identifier(){
        return (lex_token_stream[symbol_ptr][3] == "79");
    }
    
    //判断是数字常量
    bool is_number_constant(){
        return (lex_token_stream[symbol_ptr][3] == "81" || lex_token_stream[symbol_ptr][3] == "82" || lex_token_stream[symbol_ptr][3] == "83");
    }
    
    //判断是字符串常量
    bool is_string_constant(){
        return (lex_token_stream[symbol_ptr][3] == "80");
    }
    
    //判断是布尔常量
    bool is_bool_constant(){
        return (cur_sym() == "true" || cur_sym() == "false");
    }
    
    //判断是常量
    bool is_constant(){
        return is_string_constant() || is_bool_constant() || is_number_constant();
    }
    
    //判断是操作数
    bool is_operand(){
        return is_constant() || is_identifier() || (is_identifier() && next_sym() == "(");
    }
    
    //判断是 「操作数 二元运算符 操作数」 的句型
    bool is_op_bin_op(){
        bool b1 = is_operand();
        symbol_ptr ++;
        bool b2 = is_binary_operator();
        symbol_ptr ++;
        bool b3 = is_operand();
        symbol_ptr -= 2;
        return b1 && b2 && b3;
    }
    
    //判断是 「一元运算符 操作数」 的句型
    bool is_u_op(){
        bool b1 = is_unary_operator();
        symbol_ptr ++;
        bool b2 = is_operand();
        symbol_ptr --;
        return b1 && b2;
    }
    
    //判断是赋值运算符
    bool is_assign_operator(){
        string symbol = cur_sym();
        return (symbol == "+=" || symbol == "-=" || symbol == "*=" || symbol == "/=" || symbol == "%=" || symbol == "&=" || symbol == "^=" || symbol == "|=" || symbol == "=");
    }
    
    //重载上面函数
    bool is_assign_operator(string symbol){
        return (symbol == "+=" || symbol == "-=" || symbol == "*=" || symbol == "/=" || symbol == "%=" || symbol == "&=" || symbol == "^=" || symbol == "|=" || symbol == "=");
    }
    
    //判断是不是表达式块的结尾
    bool is_statements_end(){
        return (cur_sym() == "}" || cur_sym() == "case" || cur_sym() == "default");
    }
    
    //判断是不是函数声明语句
    bool is_function_declaration(){
        bool b1 = is_type();
        symbol_ptr ++;
        bool b2 = is_identifier();
        symbol_ptr ++;
        bool b3 = (cur_sym() == "(");
        symbol_ptr ++;
        bool b4 = is_type();
        symbol_ptr ++;
        bool b5 = is_identifier();
        symbol_ptr ++;
        bool b6 = (cur_sym() == ")");
        symbol_ptr ++;
        bool b7 = (cur_sym() == ";");
        symbol_ptr -= 6;
        return b1 && b2 && b3 && b4 && b5 && b6 && b7;
        return true;
    }
    
    //判断是不是主函数声明语句
    bool is_main_function(){
        return cur_sym() == "int" && next_sym() == "main";
    }
    
    //判断是不是不进行初始赋值的变量定义语句
    bool is_variable_definition_without_initial_assignment(){
        bool b1 = is_type();
        symbol_ptr ++;
        bool b2 = is_identifier();
        bool b3 = (next_sym() == ";");
        symbol_ptr --;
        return b1 && b2 && b3;
    }
    
    //判断是不是进行初始赋值的变量定义语句
    bool is_variable_definition_with_initial_assignment(){
        bool b1 = is_type();
        symbol_ptr ++;
        bool b2 = is_identifier();
        symbol_ptr ++;
        bool b3 = (cur_sym() == "=");
        symbol_ptr ++;
        bool b4 = is_operand();
        bool b5 = (next_sym() == ";");
        symbol_ptr -= 3;
        return b1 && b2 && b3 && b4 && b5;
    }
    
    //判断是不是外部语句
    bool is_external_statements(){
        return is_function_declaration() || is_variable_definition_without_initial_assignment() || is_variable_definition_with_initial_assignment();
    }
    
    void Struct(){
        //最后如果还有时间就来写
    }
    
    // Program -> Function_Body Program | External_Statements Program | Empty
    void Program(int nums){
//                    if(is_external_statements()){
//                        S.add_child_node(External_Statements());
//                    }
//                    else{
//                        S.add_child_node(Function_Body());
//                    }
        while(symbol_ptr < nums-1){
            if(is_external_statements()){
                S.add_child_node(External_Statements());
            }
            else{
                S.add_child_node(Function_Body());
            }
        }
        
    }
    
    // External_Statements -> Variable_Definition_With_Initial_Assignment ; | Variable_Definition_Without_Initial_Assignment ; | Function_Decleration ;
    Node External_Statements(){
        Node External_Statements("External_Statements",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        if(is_function_declaration()){
            External_Statements.add_child_node(Function_Declaration());
        }
        else if(is_variable_definition_without_initial_assignment()){
            External_Statements.add_child_node(Variable_Definition_Without_Initial_Assignment());
        }
        else if(is_variable_definition_with_initial_assignment()){
            External_Statements.add_child_node(Variable_Definition_With_Initial_Assignment());
        }
        else{
            illegal(External_Statements);
            symbol_ptr ++;
        }
        
        pair(External_Statements, ";");
        return External_Statements;
    }
    
    // Function_Decleration -> Type Func_name (Params_Definition)
    Node Function_Declaration(){
        Node Function_Declaration("Function Declaration",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Function_Declaration.add_child_node(Type());
        Function_Declaration.add_child_node(Func_Name());
        pair(Function_Declaration, "(");
        Function_Declaration.add_child_node(Param_Definition());
        pair(Function_Declaration, ")");
        return  Function_Declaration;
    }
    
    //主函数声明
    Node Main_Function_Declaration(){
        Node Main_Function_Declaration("Main_Function_Declaration",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Main_Function_Declaration.add_child_node(Type());
        Main_Function_Declaration.add_child_node(Func_Name());
        pair(Main_Function_Declaration, "(");
        pair(Main_Function_Declaration, ")");
        return Main_Function_Declaration;
    }
    
    // Func_Name
    Node Func_Name(){
        Node Func_Name("Func_Name",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Func_Name.add_child_node(cur_sym());
        symbol_ptr ++;
        return Func_Name;
    }
    
    // Params_Definition -> Param_Definition | Param_Definition, Params_Definition | Empty
    Node Params_Definition(){
        Node Params_Definition("Params_Definition",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Params_Definition.add_child_node(Param_Definition());
        return Params_Definition;
    }
    
    // Param_Definition -> Type Identifier
    Node Param_Definition(){
        Node Param_Definition("Param_Definition",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Param_Definition.add_child_node(Type());
        Param_Definition.add_child_node(Identifier());
        return Param_Definition;
    }
    
    
    // Variable_Definition_Without_Initial_Assignment -> Type Variables
    Node Variable_Definition_Without_Initial_Assignment(){
        Node Variable_Definition_Without_Initial_Assignment("Variable_Definition_Without_Initial_Assignment",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Variable_Definition_Without_Initial_Assignment.add_child_node(Type());
        Variable_Definition_Without_Initial_Assignment.add_child_node(Variables());
        
        return Variable_Definition_Without_Initial_Assignment;
    }
    
    // Variable_Definition_With_Initial_Assignment -> Type Variables Variable_Description
    Node Variable_Definition_With_Initial_Assignment(){
        Node Variable_Definition_With_Initial_Assignment("Variable_Definition_With_Initial_Assignment",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Variable_Definition_With_Initial_Assignment.add_child_node(Type());
        Variable_Definition_With_Initial_Assignment.add_child_node(Variables());
        Variable_Definition_With_Initial_Assignment.add_child_node(Variable_Description());
        return Variable_Definition_With_Initial_Assignment;
    }
    
    // Type
    Node Type(){
        Node Type("Type",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Type.add_child_node(Node(cur_sym(),lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]));
        symbol_ptr ++;
        return Type;
    }
    
    // Variables -> Identifier | Identifier, Variables
    Node Variables(){
        Node Variables("Variables",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Variables.add_child_node(Identifier());
        while(cur_sym() == ","){
            pair(Variables, ",");
            Variables.add_child_node(Identifier());
        }
        return Variables;
    }
    
    // Variable_Description -> = Expression
    Node Variable_Description(){
        Node Variable_Description("Variable_Description",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Variable_Description.add_child_node(Node(cur_sym(),lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]));
        symbol_ptr ++;
        Variable_Description.add_child_node(Expression());
        return Variable_Description;
    }
    
    //Identifier
    Node Identifier(){
        Node Identifier("Identifier",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Identifier.add_child_node(Node(cur_sym(),lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]));
        symbol_ptr ++;
        return Identifier;
    }
    
    // Expression -> Operand | Operand Binary_Operator Operand | Unary_Operator Operand
    Node Expression(){
        Node Expression("Expression",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        
        if(is_u_op()){
            Expression.add_child_node(Unary_Operator());
            Expression.add_child_node(Operand());
        }
        else{
            Expression.add_child_node(Operand());
            if(is_binary_operator()){
                Expression.add_child_node(Binary_Operator());
                Expression.add_child_node(Operand());
            }
        }

        return Expression;
    }
    
    // Operand -> Number_Constant | String_Constant | Bool_Constant | Id | Function_Call | ( Expression )
    Node Operand(){
        Node Operand("Operand",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        if(is_number_constant()){
            Operand.add_child_node(Number_Constant());
        }
        else if(is_string_constant()){
            Operand.add_child_node(String_Constant());
        }
        else if(is_bool_constant()){
            Operand.add_child_node(Bool_Constant());
        }
        else if(cur_sym() == "("){
            pair(Operand, "(");
            Operand.add_child_node(Expression());
            pair(Operand, ")");
        }
        else if(is_identifier()){
            if(next_sym() == "("){
                Operand.add_child_node(Function_Call());
            }
            else{
                Operand.add_child_node(Identifier());
            }
        }
        else{
            illegal(Operand);
        }
        return Operand;
    }
    
    // Number_Constant
    Node Number_Constant(){
        Node Number_Constant("Number_Constant",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Number_Constant.add_child_node(cur_sym());
        symbol_ptr++;
        return Number_Constant;
    }
    
    // String_Constant
    Node String_Constant(){
        Node String_Constant("String_Constant",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        String_Constant.add_child_node(cur_sym());
        symbol_ptr++;
        return String_Constant;
    }
    
    // Bool_Constant
    Node Bool_Constant(){
        Node Bool_Constant("Bool_Constant",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Bool_Constant.add_child_node(cur_sym());
        symbol_ptr++;
        return Bool_Constant;
    }
    
    // Binary_Operator
    Node Binary_Operator(){
        Node Binary_Operator("Binary_Operator",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Binary_Operator.add_child_node(cur_sym());
        symbol_ptr ++;
        return Binary_Operator;
    }
    
    // Unary_Operator
    Node Unary_Operator(){
        Node Unary_Operator("Unary_Operator",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Unary_Operator.add_child_node(cur_sym());
        symbol_ptr ++;
        return Unary_Operator;
    }
    
    // Function_Body -> Function_Decleration Block
    Node Function_Body(){
        Node Function_Body("Function_Body",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        if(is_main_function()){
            Function_Body.add_child_node(Main_Function_Declaration());
        }
        else{
            Function_Body.add_child_node(Function_Declaration());
        }
        Function_Body.add_child_node(Block());
        return Function_Body;
    }
    
    // Block -> { Statements }
    Node Block(){
        Node Block("Block",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        pair(Block, "{");
        if(!is_statements_end()){
            Block.add_child_node(Statements());
        }
        pair(Block, "}");
        return Block;
    }
    
    // Statements -> Statement Statements | Empty
    Node Statements(){
        Node Statements("Statements",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Statements.add_child_node(Statement());
        while(!is_statements_end()){
            Statements.add_child_node(Statement());
        }
        return Statements;
    }
    
    // Statement -> Assign ; | Variable_Definition ; | Function_Decleration ; | Do_While ; | While | If | Switch | break ; | continue ; | Return  ; | Function_Call ;
    Node Statement(){
        Node Statement("Statement",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        if(is_type()){
            if(is_function_declaration()){
                Statement.add_child_node(Function_Declaration());
                pair(Statement, ";");
            }
            else if(is_variable_definition_without_initial_assignment()){
                Statement.add_child_node(Variable_Definition_Without_Initial_Assignment());
                pair(Statement, ";");
            }
            else if(is_variable_definition_with_initial_assignment()){
                Statement.add_child_node(Variable_Definition_With_Initial_Assignment());
                pair(Statement, ";");
            }
            else{
                symbol_ptr ++;
                pair(Statement, ";");
                symbol_ptr++;
            }
        }
        else if(cur_sym() == "return"){
            Statement.add_child_node(Return());
            pair(Statement, ";");
        }
        else if(cur_sym() == "break"){
            Statement.add_child_node(cur_sym());
            symbol_ptr ++;
            pair(Statement, ";");
        }
        else if(cur_sym() == "continue"){
            Statement.add_child_node(cur_sym());
            symbol_ptr ++;
            pair(Statement, ";");
        }
        else if(cur_sym() == "while")
            Statement.add_child_node(While());
        else if(cur_sym() == "if")
            Statement.add_child_node(If());
        
        else if(cur_sym() == "do")
            Statement.add_child_node(Do_While());
        else if(cur_sym() == "switch")
            Statement.add_child_node(Switch());
        else if(is_identifier() || is_constant()){
            if(is_assign_operator(next_sym())){
                Statement.add_child_node(Assign());
                pair(Statement, ";");
            }
            else if(next_sym() == "("){
                Statement.add_child_node(Function_Call());
                pair(Statement, ";");
            }
            else{
                pair(Statement, "Assign Operator");
                symbol_ptr ++;
            }
        }
        else{
            illegal(Statement);
            symbol_ptr ++;
        }
        return Statement;
    }
    
    // Assign -> Id Assign_Operators Expression
    Node Assign(){
        Node Assign("Assign",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Assign.add_child_node(Identifier());
        Assign.add_child_node(Assign_Operators());
        Assign.add_child_node(Expression());
        return Assign;
    }
    
    //Assign_Operators
    Node Assign_Operators(){
        Node Assign_Operators("Assign_Operators",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Assign_Operators.add_child_node(Node(cur_sym(),lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]));
        symbol_ptr ++;
        return Assign_Operators;
    }
    
    //Do_While -> do Block while ( Expression )
    Node Do_While(){
        Node Do_While("Do_While",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        pair(Do_While, "do");
        Do_While.add_child_node(Block());
        pair(Do_While, "while");
        pair(Do_While, "(");
        Do_While.add_child_node(Expression());
        pair(Do_While, ")");
        return Do_While;
    }
    
    // While -> while ( Expression ) Block
    Node While(){
        Node While("While",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        pair(While, "while");
        pair(While, "(");
        While.add_child_node(Expression());
        pair(While, ")");
        While.add_child_node(Block());
        return While;
    }
    
    // If -> if ( Expression ) Block | if ( Expression ) Block Else | if ( Expression ) Block Else_If Else
    Node If(){
        Node If("If",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        pair(If, "if");
        pair(If, "(");
        If.add_child_node(Expression());
        pair(If, ")");
        If.add_child_node(Block());
        while(cur_sym() == "else if"){
            If.add_child_node(Else_If());
        }
        if(cur_sym() == "else"){
            If.add_child_node(Else());
        }
        return If;
    }
    
    // Else_If -> else if ( Expression ) Block | else if ( Expression ) Block Else_If
    Node Else_If(){
        Node Else_If("Else_If",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        pair(Else_If, "else if");
        pair(Else_If, "(");
        Else_If.add_child_node(Expression());
        pair(Else_If, ")");
        Else_If.add_child_node(Block());
        return Else_If;
    }
    
    // Else -> else Block
    Node Else(){
        Node Else("Else",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        pair(Else, "else");
        Else.add_child_node(Block());
        return Else;
    }
    
    // Switch -> switch ( Expression ) { Cases Default }
    Node Switch(){
        Node Switch("Switch",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        pair(Switch, "switch");
        pair(Switch, "(");
        Switch.add_child_node(Expression());
        pair(Switch, ")");
        pair(Switch, "{");
        Switch.add_child_node(Cases());
        Switch.add_child_node(Default());
        pair(Switch, "}");
        return Switch;
    }
    
    // Cases -> Case | Case Cases
    Node Cases(){
        Node Cases("Cases",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        while(cur_sym() == "case"){
            Cases.add_child_node(Case());
        }
        return Cases;
    }
    
    // Case -> case Int : Statements
    Node Case(){
        Node Case("Case",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        pair(Case, "case");
        if(is_number_constant()){
            Case.add_child_node(Number_Constant());
        }
        pair(Case, ":");
        Case.add_child_node(Statements());
        return Case;
    }
    
    // Default -> default : Statements
    Node Default(){
        Node Default("Default",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        pair(Default, "default");
        pair(Default, ":");
        Default.add_child_node(Statements());
        return Default;
    }
    
    // Return -> return Expression
    Node Return(){
        Node Return("Return",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        pair(Return, "return");
        Return.add_child_node(Expression());
        return Return;
    }
    
    // Function_Call -> Func_name ( Params )
    Node Function_Call(){
        Node Function_Call("Function_Call",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        Function_Call.add_child_node(Func_Name());
        pair(Function_Call, "(");
        Function_Call.add_child_node(Params());
        pair(Function_Call, ")");
        return Function_Call;
    }
    
    // Params -> Expression | Expression, Params | Empty
    Node Params(){
        Node Params("Params",lex_token_stream[symbol_ptr][1],lex_token_stream[symbol_ptr][2]);
        while(cur_sym() != ")"){
            Params.add_child_node(Expression());
            if(cur_sym() == ",")
                pair(Params, ",");
        }
        return Params;
    }
    
};

/*
 文法



 √ Program -> Function_Body Program | External_Statements Program | Empty

 √ External_Statements -> Variable_Definition_With_Initial_Assignment ; | Variable_Definition_Without_Initial_Assignment ; | Function_Decleration ;



 √ Variable_Definition_Without_Initial_Assignment -> Type Variables

 √ Variable_Definition_With_Initial_Assignment -> Type Variables Variable_Description

 √ Variables -> Identifier | Identifier, Variables

 √ Variable_Description -> = Expression



 √ Function_Decleration -> Type Func_name (Params_Definition)

 √ Params_Definition -> Param_Definition | Param_Definition, Params_Definition | Empty

 √ Param_Definition -> Type Identifier





 √ Function_Body -> Function_Decleration Block

 √ Block -> { Statements }

 √ Statements -> Statement Statements | Empty

 √ Statement -> Assign ; | Variable_Definition ; | Function_Decleration ; | Do_While ; | While | If | Switch | break ; | continue ; | Return  ; | Function_Call ;



 √ Assign -> Id Assign_Operators Expression

 √ Assign_Operators -> += | -= | *= | /= | %= | &= | ^= | |= | =



 √ Do_While -> do Block while ( Expression )



 √ While -> while ( Expression ) Block



 √ If -> if ( Expression ) Block | if ( Expression ) Block Else | if ( Expression ) Block Else_If Else

 √ Else -> else Block

 √ Else_If -> else if ( Expression ) Block | else if ( Expression ) Block Else_If



 √ Switch -> switch ( Expression ) { Cases Default }

 √ Cases -> Case | Case Cases

 √ Case -> case Int : Statements

 √ Default -> default : Statements



 √ Return -> return Expression



 √ Expression -> Operand | Operand Binary_Operator Operand | Unary_Operator Operand

 √ Operand -> Number_Constant | String_Constant | Bool_Constant | Id | Function_Call | ( Expression )

 √ Binary_Operator -> || ｜ && ｜ | ｜ ^ ｜ & ｜ != ｜== ｜ > ｜ >= ｜ <= ｜ < ｜ << ｜ >> ｜ + ｜ - ｜ * ｜ / ｜ %

 √ Unary_Operator ->  ! | ~ | - | ++ | --



 √ Function_Call -> Func_name ( Params )

 √ Params -> Expression | Expression, Params | Empty



 √ Type -> bool | char | int | double | float | string
 
 */


#endif /* grammar_h */
