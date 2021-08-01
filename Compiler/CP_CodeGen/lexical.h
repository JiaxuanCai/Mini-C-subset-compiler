//
//  lexical.h
//  CP_LexAna
//
//  Created by 蔡嘉轩 on 2021/4/14.
//

#ifndef lexical_h
#define lexical_h

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "tokens.h"
#include "distinguisher.h"
using namespace std;

class Lexical {
    
public:
    vector<vector<string>> token_stream;
    Lexical(string path){
        for(int i = 0; i < 84; i ++){
            tokens[TOKENS[i]] = i + 1 ;
        }
        //初始化字符表
        
        ifstream fs_input;
        fs_input.open(path.data());
        if (!fs_input.is_open()) {
            cout << "Failed in opening source code." << endl;
        }
        stringstream ss_input;
        ss_input << fs_input.rdbuf();
        source = ss_input.str();
        //通过文件流和字符串流读入源文件
        
        scan();
        //调用scan函数对源文件进行扫描，进行词法分析。
    }
    
    void output(){
        cout<<"--------------------------Lexical analysis completed--------------------------"<<endl;
        cout<<"There are "<<error_stream.size()<<" errors:"<<endl;
        for(auto i : error_stream){
            cout<<i[0]<<" "<<i[1]<<" "<<i[2]<<" "<<i[3]<<endl;
        }
        
        cout<<endl<<"---------------------------Lexical analysis results---------------------------"<<endl;
        for(auto i : token_stream){
            cout<<"code= "<<i[3]<<" ;located at: "<<i[1]<<" - "<<i[2]<<"; property: "<<i[0]<<endl;
            statistics[TOKENS[stoi(i[3])-1]]++;
        }
        
        cout<<endl<<"-------------------------statistical analysis results-------------------------"<<endl;
        for(auto i : statistics){
            string word = i.first;
            int count = i.second;
            cout<<word<<": appears "<<count<<" times"<<endl;
            word_cnt += count;
        }
        cout<<endl<<"------------------------------------------------------------------------------"<<endl;
        cout<<"total number of code lines: "<<r<<endl;
        cout<<"total number of words: "<<word_cnt<<endl;
        cout<<"total number of characters(spaces excluded): "<<ch_cnt<<endl;
        cout<<"total number of characters(spaces included): "<<ch_cnt+space_cnt;
        cout<<endl<<"------------------------------------------------------------------------------"<<endl;
    }
    
    
private:
    lex_distinguisher d;
    map<string, int> tokens;
    map<string, int> statistics;
    vector<vector<string>> error_stream;
    int ptr = 0;
    int r = 1;
    int c = 1;
    int ch_cnt = 0;
    int space_cnt = 0;
    int word_cnt = 0;
    string source;
    
    void seperators(){
        char ch = source[ptr];
        ptr ++;
        space_cnt ++;
        if(ch == '\n'){
            c = 1;
            r ++;
        }else
            c ++;
    }
    
    void string_const(){
        int len = 1;
        bool legal = false;
        while (ptr + len < source.length()){
            
            if(source[ptr+len] == '\n'){
                error_stream.push_back(vector<string>{"ERROR: Unclosed Quotation marks, expected '\"' here", position(r, c), position(r, c + len - 1)});
                break;
            }
            
            if(source[ptr+len] == '"'){
                legal = true;
                len ++;
                break;
            }
            len ++;
        }
        
        if(legal)
            token_stream.push_back(vector<string>{source.substr(ptr,len), position(r, c), position(r, c + len - 1), to_string(tokens["STRING_CONST"])});
        
        c = c + len;
        ptr = ptr + len;
    }
    
    void identifier(){
        int len = 1;
        if(source.substr(ptr, 7) == "else if")
            len = 7;
        else
            while(d.is_character(source[ptr + len]) || d.is_digit(source[ptr + len]))
                len++;
            
        int code = tokens[source.substr(ptr, len)];
        if(code == 0)
            token_stream.push_back(vector<string>{source.substr(ptr,len),position(r, c),position(r, c + len - 1),to_string(tokens["IDENTIFIER"])});
        else
            token_stream.push_back(vector<string>{source.substr(ptr,len),position(r, c),position(r, c + len - 1),to_string(tokens[source.substr(ptr, len)])});
        
        ptr = ptr + len;
        c = c + len;
        ch_cnt = ch_cnt +len;
    }
    
    void num_const(){
        if(source[ptr] == '0' && d.is_oct_digit(source[ptr + 1]))
            oct_const();
        else if(source[ptr + 1] == 'x' || source[ptr + 1] == 'X')
            hex_const();
        else
            dec_const();
    }
    
    void dec_const(){
        int len = 1;
        while (d.is_digit(source[ptr + len]) || source[ptr + len] == 'E' || source[ptr + len] == '.')
            len ++;
        
        if(d.is_character(source[ptr + len])){
            while(!d.is_operator(source[ptr + len]) && !d.is_seperator(source[ptr + len]))
                len ++;
            error_stream.push_back(vector<string>{"ERROR: Illeagle identifier", source.substr(ptr, len), position(r, c), position(r, c + len - 1)});
        }
        else{
            token_stream.push_back(vector<string>{source.substr(ptr,len), position(r, c), position(r, c + len - 1), to_string(tokens["DECIMAL_CONST"])});
        }
        
        ptr = ptr + len;
        c = c + len;
        ch_cnt = ch_cnt +len;
    }
    
    void oct_const(){
        int len = 1;
        while (d.is_oct_digit(source[ptr + len]))
            len ++;
        
        if(d.is_character(source[ptr + len])){
            while(!d.is_operator(source[ptr + len]) && !d.is_seperator(source[ptr + len]))
                len ++;
            error_stream.push_back(vector<string>{"ERROR: Illeagle identifier", source.substr(ptr, len), position(r, c), position(r, c + len - 1)});
        }
        else{
            token_stream.push_back(vector<string>{source.substr(ptr,len), position(r, c), position(r, c + len - 1), to_string(tokens["OCTONARY_CONST"])});
        }
        
        ptr = ptr + len;
        c = c + len;
        ch_cnt = ch_cnt +len;
    }
    
    void hex_const(){
        int len = 2;
        while (d.is_hex_digit(source[ptr + len]))
            len ++;
        
        if(d.is_character(source[ptr + len])){
            while(!d.is_operator(source[ptr + len]) && !d.is_seperator(source[ptr + len]))
                len ++;
            error_stream.push_back(vector<string>{"ERROR: Illeagle identifier", source.substr(ptr, len), position(r, c), position(r, c + len - 1)});
        }
        else{
            token_stream.push_back(vector<string>{source.substr(ptr,len), position(r, c), position(r, c + len - 1), to_string(tokens["HEXADECIMAL_CONST"])});
        }
        
        ptr = ptr + len;
        c = c + len;
        ch_cnt = ch_cnt +len;
    }
    
    void operators(){
        int len = 1;
        bool matched = false;
        char ch = source[ptr];
        
        if(ch == '/' && source[ptr + 1] == '*'){
            len ++;
            while(1){
                if(source[ptr + len] != '*'){
                    if(source[ptr + len] == '\n')
                        r ++;
                    len ++;
                }else{
                    if(source[ptr + len + 1] == '/'){
                        len +=2;
                        break;
                    }
                }
            }
        }
        else if (ch == '/' && source[ptr + 1] == '/'){
            while(1){
                len++;
                if(source[ptr + len] == '\n'){
                    break;
                }
            }
        }
        else{
            if(!matched && (ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '.' || ch == ',' || ch == ';' || ch == '\'' || ch == '"' || ch == ':')){
                matched = true;
            }
            if(!matched && (ch == '!' || ch == '&' || ch == '~' || ch == '^' || ch == '|' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '>' || ch == '<' || ch == '=')){
                if(source[ptr+len]=='='){
                    matched = true;
                    len ++;
                }
            }
            if(!matched && (ch == '&' || ch == '|' || ch == '+' || ch == '-' || ch == '>' || ch == '<')){
                if(source[ptr+len] == source[ptr]){
                    matched = true;
                    len ++;
                }
            }
            
            
            token_stream.push_back(vector<string>{source.substr(ptr,len), position(r, c), position(r, c + len - 1), to_string(tokens[source.substr(ptr,len)])});
        }

        
        ptr = ptr + len;
        c = c + len;
        ch_cnt = ch_cnt +len;
    }
    
    string position(int r, int c){
        return ("(" + to_string(r) + "," + to_string(c) + ")");
    }
    
    void scan(){
        while(ptr < source.length()){
            char ch = source[ptr];
            if(ch == '"')
                string_const();
            else if(d.is_character(ch))
                identifier();
            else if(d.is_seperator(ch))
                seperators();
            else if(d.is_digit(ch))
                num_const();
            else if(d.is_operator(ch))
                operators();
        }
    }
};

#endif /* lexical_h */
