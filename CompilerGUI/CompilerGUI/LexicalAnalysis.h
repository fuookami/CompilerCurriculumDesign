#pragma once

#include "basic.h"
#include "Terminal.h"
#include "Token.h"
#include <regex>

using namespace std;

//关键字信息
static const vector<string> KeyWords = {
	"program", "const", "var", "procedure", "begin", "end", "if", "then", "while", "do",
	"call", "write", "odd", "read", "else", 
	":=", "\\<\\>", "\\<=", "\\>=", 
	"=", "\\(", "\\)", ";", ",", "=", "\\+", "-", "\\*", "/"
};

//词法分析类
class LexicalAnalysis       
{
	friend class SingleTon<LexicalAnalysis>;    		

public:
	vector<Token> scan(istream &is) const;      //扫描文件

private:
	LexicalAnalysis();     //词法分析逻辑

private:
	vector<shared_ptr<regex>> regexs;	
};