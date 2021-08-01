//
//  main.cpp
//  CP_LAB4
//
//  Created by 蔡嘉轩 on 2021/6/20.
//

#include <iostream>
#include "semantic.h"

class ObjectCodeTranslator{
public:
    vector<Quaternion> internal_code_sequence;
    vector<string> object_code_sequence;
    
    void output_object_code(string fp){
        ofstream f(fp);
        for(auto i : object_code_sequence)
            f << i;
        f.close();
    }
    
    //查询得出MIP目标机器一般使用8个寄存器，分别编号为7～0
    vector<int> regs = {7，6，5，4，3，2，1，0};
    
    //寄存器状态存储数据结构
    map<string, int> regstatus;
    
    ObjectCodeTranslator(vector<Quaternion> internal){
        internal_code_sequence = internal;
        translate();
    }
    
    void translate(){
        for(auto i : internal_code_sequence){
            transfer(i);
        }
    }
    
    int getregs(string id){
        //分配寄存器
        int av_reg = regs.back();
        regs.pop_back();
        regstatus[id] = av_reg;
        return av_reg;
    }

    int recallregs(string id){
        //收回寄存器
        int al_reg = regstatus[id];
        regstatus.erase(id);
        regs.push_back(al_reg);
        return al_reg;
    }
        
    void transfer(Quaternion inter){
        ostringstream oss;
        string Operator = inter.Operator;
        string Operand1 = inter.Operand1;
        string Operand2 = inter.Operand2;
        string Result = inter.Result;
        
        if(Operator == "+"){
            oss<<"ADD $t" << getregs(Result) << " $t" <<recallregs(Operand1) << " $t" << recallregs(Operand2) << endl;
        }
        else if(Operator == "-"){
            oss<<"SUB $t" << getregs(Result) << " $t" <<recallregs(Operand1) << " $t" << recallregs(Operand2) << endl;
        }
        else if(Operator == "*"){
            oss<<"MULT $t" << recallregs(Operand1) << " $t" << recallregs(Operand2) << endl;
            oss<<"MFLO $t" <<getregs(Result) <<endl;
        }
        else if(Operator == "/"){
            oss<<"DIV $t" << recallregs(Operand1) << " $t" << recallregs(Operand2) << endl;
            oss<<"MFLO $t" <<getregs(Result) <<endl;
        }
        else if(Operator == "RETURN"){
            oss << "MOVE $v0 $t" << recallregs(Result) << endl;
            oss << "MOVE $ra $s0" << endl;
            oss << "jr $ra" << endl;
        }
        else if(Operator == "LOAD_CONST"){
            oss << "LI $t" << getregs(Result) << " " << Operand1 << endl;
        }
        else if(Operator == "LOAD_ID"){
            oss << "LW $t" << getregs(Result) << " " << stoi(Operand1) << "($SP)" << endl;
        }
        else if(Operator == "LABEL"){
            oss << endl << Result << ":" <<endl;
        }
        else if(Operator == "READ"){
            oss << "JAL READ" <<endl;
            oss << "SW $v0" << stoi(Result) << "($SP)" <<endl;
        }
        else if(Operator == "WRITE"){
            oss << "MOVE $a0 $t" << recallregs(Result) <<endl;
            oss << "JAL WRITE" << endl;
        }
        else if(Operator == ">"){
            oss << "BGT $t" << recallregs(Operand1) << " $t" << recallregs(Operand2) << " " << Result <<endl;
        }
        else if(Operator == "<"){
            oss << "BLT $t" << recallregs(Operand1) << " $t" << recallregs(Operand2) << " " << Result <<endl;
        }
        else if(Operator == ">="){
            oss << "BGE $t" << recallregs(Operand1) << " $t" << recallregs(Operand2) << " " << Result <<endl;
        }
        else if(Operator == "<="){
            oss << "BLE $t" << recallregs(Operand1) << " $t" << recallregs(Operand2) << " " << Result <<endl;
        }
        else if(Operator == "="){
            oss << "BEQ $t" << recallregs(Operand1) << " $t" << recallregs(Operand2) << " " << Result <<endl;
        }
        else if(Operator == ":="){
            if(Operand1 == "0"){
                oss << "SW $zero" << " " << stoi(Result) << "($sp)" << endl;
            }
            else{
                oss << "SW $t" << recallregs(Operand1) << " " << stoi(Result) << "($sp)" << endl;
            }
        }
        else if(Operator == "GOTO"){
            oss << "j " << Result << endl;
        }
        else if(Operator == "EXIT"){
            oss << "LI $v0 10" << endl <<"syscall" << endl;
        }
        object_code_sequence.push_back(oss.str());
    }
        
};

int main(int argc, const char * argv[]) {
    //对源程序进行词法分析
    Lexical l("/Users/caijiaxuan/workspace/xcodecpp/CP_LAB4/CP_LAB4/test2.txt");
    
//    l.output();
    //符号流中的符号数
    int nums = l.token_stream.size();
    
    //对词法分析得到的符号流进行语法分析
    Grammar g(l.token_stream, nums);
        
    //将语法分析结果输出得到语法树
    g.output_tree("/Users/caijiaxuan/workspace/xcodecpp/CP_LAB4/CP_LAB4/output.txt");
    ofstream f("/Users/caijiaxuan/workspace/xcodecpp/CP_LAB4/CP_LAB4/graphviz.txt");
    g.output_graphviz(f,g.S);
    f.close();
    
    Semantic s(g.S);
    ObjectCodeTranslator o (s.quaternions.quaternions);
    o.output_object_code("/Users/caijiaxuan/workspace/xcodecpp/CP_LAB4/CP_LAB4/object.txt");
    return 0;
}
