//
//  quaternion.h
//  CP_CodeGen
//
//  Created by 蔡嘉轩 on 2021/6/10.
//

#ifndef quaternion_h
#define quaternion_h

class Quaternion{
public:
    string Operator, Operand1, Operand2, Result;
    
    Quaternion(string op, string o1, string o2, string res){
        Operator = op;
        Operand1 = o1;
        Operand2 = o2;
        Result = res;
    }
    
};

class Quaternions{
public:
    vector<Quaternion> quaternions;
    int temp_ptr = 0;
    
    void output_quaternions(string filepath){
        ofstream file(filepath);
        for(auto i : quaternions){
            file << i.Operator << " " << i.Operand1 << " " << i.Operand2 << " " << i.Result << endl;
        }
    }
    
    void add_quaternion(string op, string o1, string o2, string res){
        quaternions.push_back(Quaternion(op,o1,o2,res));
    }
    
    string get_temp(){
        return "TEMP"+to_string(temp_ptr++);
    }
};
#endif /* quaternion_h */
