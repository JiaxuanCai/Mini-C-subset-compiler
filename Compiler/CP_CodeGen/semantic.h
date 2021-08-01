//
//  semantic.h
//  CP_CodeGen
//
//  Created by 蔡嘉轩 on 2021/4/20.
//

#ifndef semantic_h
#define semantic_h

#include "grammar.h"
#include "symbol_table.h"
#include "quaternion.h"

class Semantic{
public:
    Semantic(Node &node){
        S = node;
        //语义分析
        //处理根节点（方法：遍历子节点）
        scope_ptr = 0;
        quaternions.add_quaternion("LABEL", "NIL", "NIL", "main");
        for(auto i : S.children){
            if(i.symbol == "Function_Body")
                proc_function_body(i);
            else if (i.symbol == "External_Statements")
                proc_external_statements(i);
        }
        
        //输出错误
        //error_output();
        
        //输出符号表
        //identifiers.output();
        
        //输出中间代码
        quaternions.output_quaternions("/Users/caijiaxuan/workspace/xcodecpp/CP_LAB4/CP_LAB4/inter.txt");
    }
    Quaternions quaternions;

    
private:
    Node S;
    Identifiers identifiers;
    
    vector<vector<string>> seman_error_stream;
    vector<int> scope_list;
    int scope_ptr;
    
//    void error_output(){
//        if(seman_error_stream.size() == 0)
//            cout<<" 0 semantic error founded."<<endl<<endl;
//        else{
//            cout<<" "<<seman_error_stream.size()<<" semantic errors founded:"<<endl<<endl;
//            for(auto i : seman_error_stream){
//                cout<<"[From "<<i[1]<<" to "<<i[2]<<"] "<<i[0]<<endl;
////                cout<<i[0]<<endl;
//            }
//        }
//    }
    
    //定位域
    void locate_scope(int id){
        scope_list.push_back(id);
        scope_ptr = id;
    }
    
    //重定位域
    void relocate_scope(){
        scope_list.pop_back();
        scope_ptr = scope_list.back();
        int size = identifiers.variable_list[scope_ptr].size();
        Variable::total_offset += 4*size;
    }
    
    void proc_external_statements(const Node& External_statements){
        if(External_statements.children[0].symbol == "Function Declaration") proc_function_decleration(External_statements.children[0]);
        else if (External_statements.children[0].symbol == "Variable_Definition_Without_Initial_Assignment")
            proc_variable_definition_without_initial_assignment(External_statements.children[0]);
        else if (External_statements.children[0].symbol == "Variable_Definition_With_Initial_Assignment")
            proc_variable_definition_with_initial_assignment(External_statements.children[0]);
    }

    void proc_function_decleration(const Node& Function_decleration){
        //在符号表中构建新函数的项目
        Function f;
        f.return_type = Function_decleration.children[0].children[0].symbol;
        f.name = Function_decleration.children[1].children[0].symbol;
        
//        if(identifiers.function_exist_in_scope(0, f.name)){
//            seman_error_stream.push_back(vector<string>{"[Redecleration of Function/Process]: Function '" + f.name + "' already exists in the current scope.",Function_decleration.start_pos,Function_decleration.end_pos});
//        }
//        else{
//            if(scope_ptr != 0){
//                seman_error_stream.push_back(vector<string>{"[Nested decleration of Function/Process]: You can not declare: '" + f.name + "' because it's nestedly declared.",Function_decleration.start_pos,Function_decleration.end_pos});
//            }
//            else{
                for(auto param : Function_decleration.children){
                    if(param.symbol == "Type" || param.symbol == "Func_Name" || param.symbol == "(" || param.symbol == ")") continue;
                    f.parameters.push_back(pair<string, string>(param.children[0].children[0].symbol, param.children[1].children[0].symbol));
                }
                identifiers.function_list[scope_ptr][f.name] = f;
//            }
//        }
    }
    
    void proc_variable_definition_without_initial_assignment(const Node& Variable_Definition_Without_Initial_Assignment){
        //在符号表中构建新变量的项目
        Variable v;
        v.type = Variable_Definition_Without_Initial_Assignment.children[0].children[0].symbol;
        v.name = Variable_Definition_Without_Initial_Assignment.children[1].children[0].children[0].symbol;
        v.value = "";
        
//        if(identifiers.variable_exist_in_scope(scope_ptr, v.name)){
//            seman_error_stream.push_back(vector<string>{"[Redecleration of Variable]: Variable '" + v.name + "' already exists in the current scope.",Variable_Definition_Without_Initial_Assignment.start_pos,Variable_Definition_Without_Initial_Assignment.end_pos});
//        }
//
//        else
            identifiers.variable_list[scope_ptr][v.name] = v;
    }
    
    void proc_variable_definition_with_initial_assignment(const Node& Variable_Definition_With_Initial_Assignment){
        //先处理Variable Description再在符号表中构建新变量的项目
        string res = proc_expression(Variable_Definition_With_Initial_Assignment.children[2].children[1]);
        Variable v;
        v.type = Variable_Definition_With_Initial_Assignment.children[0].children[0].symbol;
        v.name = Variable_Definition_With_Initial_Assignment.children[1].children[0].children[0].symbol;
        v.value = " ";
        
//        if(identifiers.variable_exist_in_scope(scope_ptr, v.name)){
//            seman_error_stream.push_back(vector<string>{"[Redecleration of Variable]: Variable '" + v.name + "' already exists in the current scope.",Variable_Definition_With_Initial_Assignment.start_pos,Variable_Definition_With_Initial_Assignment.end_pos});
//        }
//        else
            identifiers.variable_list[scope_ptr][v.name] = v;
        int offset = identifiers.variable_list[scope_ptr][v.name].offset;
        quaternions.add_quaternion(":=", "0", "NIL", to_string(offset));
    }
    
    void proc_function_body(const Node& Function_Body){
        proc_function_decleration(Function_Body.children[0]);
        quaternions.add_quaternion("GOTO", "NIL", "NIL", "BLOCK");
        quaternions.add_quaternion("EXIT", "NIL", "NIL", "NIL");
        quaternions.add_quaternion("LABEL", "NIL", "NIL", "BLOCK");
        proc_block(Function_Body.children[1]);
    }
    
    void proc_return(const Node& Return){
        string res = proc_expression(Return.children[1]);
        quaternions.add_quaternion("RETURN", "NIL", "NIL", res);
    }
    
    void proc_while(const Node& While){
        string while_name = "WHILE" + to_string(While.id);
        string while_block_name = "WHILE BLOCK" + to_string(While.id);
        string while_exit_name = "EXIT" + to_string(While.id);
        
        quaternions.add_quaternion("LABEL", "NIL", "NIL", while_name);
        proc_expression(While.children[2]);
        quaternions.quaternions.back().Result = while_block_name;
        quaternions.add_quaternion("GOTO", "NIL", "NIL", while_exit_name);
        quaternions.add_quaternion("LABEL", "NIL", "NIL", while_block_name);
        proc_block(While.children[4]);
        quaternions.add_quaternion("GOTO", "NIL", "NIL", while_name);
        quaternions.add_quaternion("LABEL", "NIL", "NIL", while_exit_name);
    }
    
    void proc_funcall(const Node& Funcall){
        string func_name = Funcall.children[0].children[0].symbol;
        if(func_name == "get"){
            for(auto i : Funcall.children[2].children){
                string v_name = i.children[0].children[0].children[0].symbol;
                int scope = identifiers.find_variable(scope_list, v_name);
                int offset = identifiers.variable_list[scope][v_name].offset;
                quaternions.add_quaternion("READ", "NIL", "NIL", to_string(offset));
            }
        }
        else if (func_name == "put"){
            string res = proc_operand(Funcall.children[2].children[0].children[0]);
            quaternions.add_quaternion("WRITE", "NIL", "NIL", res);
        }
    }
    
    void proc_if(const Node& If){
        string if_name = "IF" + to_string(If.id);
        proc_expression(If.children[2]);
        quaternions.quaternions.back().Result = if_name;
        
        for(auto i : If.children){
            if(i.symbol == "Else_If") proc_else_if_header(i);
            else if(i.symbol == "Else") proc_else_header(i);
        }
        
        string exit_name = "EXIT" + to_string(If.id);
        quaternions.add_quaternion("GOTO", "NIL", "NIL", exit_name);
        
        proc_if_body(If, exit_name);
        for(auto i : If.children){
            if(i.symbol == "Else_If") proc_else_if_body(i, exit_name);
            else if(i.symbol == "Else") proc_else_body(i, exit_name);
        }
        quaternions.add_quaternion("LABEL", "NIL", "NIL", exit_name);
    }
    
    
    void proc_if_body(const Node& Ifbody, string exit_name){
        string if_name = "IF" + to_string(Ifbody.id);
        quaternions.add_quaternion("LABEL", "NIL", "NIL", if_name);
        proc_block(Ifbody.children[4]);
        quaternions.add_quaternion("GOTO", "NIL", "NIL", exit_name);
        
    }
    
    void proc_else_header(const Node& Elseheader){
        string else_name = "ELSE IF" + to_string(Elseheader.id);
        quaternions.add_quaternion("GOTO", "NIL", "NIL", else_name);
    }
    
    void proc_else_body(const Node& Elsebody, string exit_name){
        string else_name = "ELSE" + to_string(Elsebody.id);
        quaternions.add_quaternion("LABEL", "NIL", "NIL", else_name);
        proc_block(Elsebody.children[1]);
        quaternions.add_quaternion("GOTO", "NIL", "NIL", exit_name);
    }
    
    void proc_else_if_header(const Node& ElseIfheader){
        string else_if_name = "ELSE IF" + to_string(ElseIfheader.id);
        proc_expression(ElseIfheader.children[2]);
        quaternions.quaternions.back().Result = else_if_name;
    }
    
    void proc_else_if_body(const Node& ElseIfbody, string exit_name){
        string else_if_name = "ELSE IF" + to_string(ElseIfbody.id);
        quaternions.add_quaternion("LABEL", "NIL", "NIL", else_if_name);
        proc_block(ElseIfbody.children[4]);
        quaternions.add_quaternion("GOTO", "NIL", "NIL", exit_name);
    }
    
    void proc_block(const Node& Block){
        locate_scope(Block.id);
        
        for(auto i : Block.children[1].children){
            proc_statement(i);
        }
        
        relocate_scope();
    }
    
    void proc_statement(const Node& Statement){
        if(Statement.children[0].symbol == "Assign")
            proc_assign(Statement.children[0]);
        else if (Statement.children[0].symbol == "Variable_Definition_Without_Initial_Assignment")
            proc_variable_definition_without_initial_assignment(Statement.children[0]);
        else if (Statement.children[0].symbol == "Variable_Definition_With_Initial_Assignment")
            proc_variable_definition_with_initial_assignment(Statement.children[0]);
        else if (Statement.children[0].symbol == "Function Declaration")
            proc_function_decleration(Statement.children[0]);
        //添加
        else if (Statement.children[0].symbol == "If")
            proc_if(Statement.children[0]);
        else if (Statement.children[0].symbol == "While")
            proc_while(Statement.children[0]);
        else if (Statement.children[0].symbol == "Return")
            proc_return(Statement.children[0]);
        else if (Statement.children[0].symbol == "Function_Call")
            proc_funcall(Statement.children[0]);
    }
    
    void proc_assign(const Node& Assign){
        string id_name = Assign.children[0].children[0].symbol;
        
        int scope = identifiers.find_variable(scope_list, id_name);
        
        if(scope){
            string exp = proc_expression(Assign.children[2]);
            int offset = identifiers.variable_list[scope][id_name].offset;
            quaternions.add_quaternion(":=", exp, "NIL", to_string(offset));
        }
    }
    
    string proc_expression(const Node& Expression){
        string res;
        if (Expression.children.size() == 1){
            res = proc_operand(Expression.children[0]);
        }
        else{
            string op1 = proc_operand(Expression.children[0]), op2 = proc_operand(Expression.children[2]);
            string opr = Expression.children[1].children[0].symbol;
            string res = quaternions.get_temp();
            quaternions.add_quaternion(opr, op1, op2, res);
        }
        return "NIL";
    }
    
    string proc_operand(const Node& Operand){
        if(Operand.children[0].symbol == "Number_Constant"){
            string val = Operand.children[0].children[0].symbol;
            string res = quaternions.get_temp();
            quaternions.add_quaternion("LOAD_CONST", val, "NIL", res);
        }
        else if(Operand.children[0].symbol == "Identifier"){
            string id_name = Operand.children[0].children[0].symbol;
            int scope = identifiers.find_variable(scope_list, id_name);
            if(scope){
                string res = quaternions.get_temp();
                int offset = identifiers.variable_list[scope][id_name].offset;
                quaternions.add_quaternion("LOAD_ID", to_string(offset), "NIL", res);
            }
            else if(Operand.children[0].symbol == "("){
                return proc_expression(Operand.children[1]);
            }
        }
        return "NIL";
    }
    
//    pair<string, string> proc_expression(const Node& Expression){
//        if(Expression.children.size() == 1){
//            // Expression -> Operand
//            return proc_operand(Expression.children[0]);
//        }
//        else if (Expression.children.size() == 2){
//            // Expression -> uop Operand
//            pair<string, string> operand = proc_operand(Expression.children[1]);
//            if(operand.first == "int"){
//                if(Expression.children[0].children[0].symbol == "++"){
//                    return pair<string, string>("int", to_string(stoi(operand.second)+1));
//                }
//                else if(Expression.children[0].children[0].symbol == "--"){
//                    return pair<string, string>("int", to_string(stoi(operand.second)-1));
//                }
//                else {
//                    seman_error_stream.push_back(vector<string>{"Illegal unary opereator: " + Expression.children[0].children[0].symbol,Expression.start_pos,Expression.end_pos});
//                    return pair<string, string>("error","");
//                }
//            }
//            else {
//                seman_error_stream.push_back(vector<string>{"[Illegal operand]: '" + Expression.children[1].children[0].children[0].symbol + "': Cannot use type '" + operand.first + "' in Arithmetic Operation.",Expression.start_pos,Expression.end_pos});
//                return pair<string, string>("error","");
//            }
//        }
//        else if (Expression.children.size() == 3){
//            // Expression -> Operand binop Operand
//            pair<string, string> operand1 = proc_operand(Expression.children[0]);
//            pair<string, string> operand2 = proc_operand(Expression.children[2]);
//            string binop = Expression.children[1].children[0].symbol;
//            if(operand1.first == "int" && operand2.first == "int"){
//                int value1 = stoi(operand1.second);
//                int value2 = stoi(operand2.second);
//                if(binop == "+") return pair<string, string>("int", to_string(value1 + value2));
//                else if(binop == "-") return pair<string, string>("int", to_string(value1 - value2));
//                else if(binop == "*") return pair<string, string>("int", to_string(value1 * value2));
//                else if(binop == "/") return pair<string, string>("int", to_string(value1 / value2));
//                else return pair<string, string>("","");
//            }
//            else return pair<string, string>("","");
//        }
//        return pair<string, string>("","");
//    }
//
//    pair<string, string> proc_operand(const Node& Operand){
//        if(Operand.children[0].symbol == "Bool_Constant")
//            return pair<string,string>("bool", Operand.children[0].children[0].symbol);
//        else if(Operand.children[0].symbol == "String_Constant")
//            return pair<string,string>("string", Operand.children[0].children[0].symbol);
//        else if(Operand.children[0].symbol == "Number_Constant")
//            return pair<string,string>("int", Operand.children[0].children[0].symbol);
//        else if(Operand.children[0].symbol == "Identifier"){
//            if(identifiers.variable_exist_in_scope(scope_ptr, Operand.children[0].children[0].symbol)){
//                Variable v = identifiers.variable_list[scope_ptr][Operand.children[0].children[0].symbol];
//                return pair<string,string>(v.type, v.value);
//            }
//            else return pair<string, string>("","");
//        }
//        else if(Operand.children[0].symbol == "("){
//            return proc_expression(Operand.children[1]);
//        }
//        else return pair<string, string>("","");
//    }
    
};

#endif /* semantic_h */
