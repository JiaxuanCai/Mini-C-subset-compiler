//
//  distinguisher.h
//  CP_GramAna
//
//  Created by 蔡嘉轩 on 2021/4/30.
//

#ifndef distinguisher_h
#define distinguisher_h

class lex_distinguisher {
    
public:
    bool is_digit(char ch){
        return (ch >= '0' && ch <='9');
    }
    
    bool is_oct_digit(char ch){
        return (ch >= '0' && ch <= '7');
    } // 是八进制数的数位
    
    bool is_hex_digit(char ch){
        return ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'));
    }// 是十六进制数的数位
    
    bool is_character(char ch){
        return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
    } // 是字母
    
    bool is_seperator(char ch){
        return (ch=='\n' || ch==' ' || ch=='\t' || ch=='\r');
    } // 是分隔符（空格、换行、回车、锁进）

    
    bool is_operator(char ch){
        return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '=' || ch == '!' || ch == '>' || ch == '<' || ch == '&' || ch == '|' || ch == '~' || ch == '^' || ch == '-' || ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '.' || ch == ',' || ch == ';' || ch == '"' || ch == '\'' || ch == ':');
    } // 是运算符,注释
};


#endif /* distinguisher_h */
