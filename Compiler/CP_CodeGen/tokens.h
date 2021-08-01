//
//  tokens.h
//  CP_LexAna
//
//  Created by 蔡嘉轩 on 2021/4/30.
//

#ifndef tokens_h
#define tokens_h

# include <iostream>
//注释部分是对各词类的记号进行定义，由于需要对记号进行统一编码，所以这里不用到，而是用到所有记号汇总的列表tokens。

using namespace std;

/*
string RESERVED[26] = {
    "int", "float", "double", "long", "short", "signed", "unsigned", "char", "void", "bool", "true", "false", "return", "break", "continue", "const", "if", "else", "else if", "for", "while", "sizeof", "static", "get", "put", "struct"
}; //保留字

string PREPROCESS[2] = {
    "include","define"
}; //预处理

string ANNOTATION[3] = {
}; //注释

string ESCAPE[4] = {
    "\n", "\r", "\t", "\b"
}; //转义

string OPERATOR[41] = {
    "+", "-", "*", "/", "%", "=", "!=", "==", ">", ">=", "<", "<=", "&&", "&", "||", "|", "!", "~", "^", "+=", "-=", "/=","*=", "^=", "&=", "|=", "%=", "~=", ">>", "<<", "++", "--", "(", ")", "[", "]", "{", "}", ".", ",", ";"
}; //操作符

string IDENTIFIER = "IDENTIFIER"; //标识符

string STRING_CONST = "STRING_CONST"; //字符串常量

string BINARY_CONST = "BINARY_CONST"; //二进制常数

string DECIMAL_CONST = "DECIMAL_CONST"; //十进制常数（包含浮点数 指数形式表示的浮点数等等）

string OCTONARY_CONST = "OCTONARY_CONST"; //八进制常数

string HEXADECIMAL_CONST = "HEXADECIMAL_CONST"; //十六进制常数
*/


const string TOKENS[84] = {
    "int", "float", "double", "long", "short",
    "signed", "unsigned", "char", "void",
    "bool", "true", "false",
    "return", "break", "continue", "const",
    "if", "else", "else if", "for", "while",
    "sizeof", "static",
    "get3", "put3", "struct",
    
    "include","define",
    
    "*/", "/*", "//",
    
    "\n", "\r", "\t", "\b",
    
    "+", "-", "*", "/", "%",
    "=", "!=", "==", ">", ">=", "<", "<=",
    "&&", "&", "||", "|", "!", "~", "^",
    "+=", "-=", "/=","*=", "^=", "&=", "|=", "%=", "~=",
    ">>", "<<", "++", "--",
    "(", ")", "[", "]", "{", "}", ".", ",", ";" ,"\"","'",
    
    "IDENTIFIER", "STRING_CONST",
    "DECIMAL_CONST", "OCTONARY_CONST", "HEXADECIMAL_CONST",
    ":"
};


#endif /* tokens_h */
