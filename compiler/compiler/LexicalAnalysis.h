#pragma once

#include "basic.h"
#include "Terminal.h"
#include "Token.h"
#include <regex>

using namespace std;

static const vector<string> KeyWords = {
	"program", "const", "var", "procedure", "begin", "end", "if", "then", "while", "do",
	"call", "write", "odd", "read", "else", 
	":=", "\\<\\>", "\\<=", "\\>=", 
	"=", "\\(", "\\)", ";", ",", "=", "\\+", "-", "\\*", "/"
};

class LexicalAnalysis
{
	friend class SingleTon<LexicalAnalysis>;

public:
	vector<Token> scan(istream &is) const;

private:
	LexicalAnalysis();

private:
	vector<shared_ptr<regex>> regexs;
};