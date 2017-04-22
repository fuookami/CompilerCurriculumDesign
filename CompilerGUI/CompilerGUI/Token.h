#pragma once

#include "basic.h"

enum TokenTag
{
	phrase,				//短语
	keyword,			//关键字
	id,						//标识符
	integer,				//常数
};
using TokenTagType = size_t;

using Token = struct strToken
{
	strToken() {};
	strToken(const string &_lexeme, const size_t _line, const TokenTagType _tag) :
		lexeme(_lexeme), line(_line), tag(_tag) {}
	strToken(const strToken &_ano) : lexeme(_ano.lexeme), line(_ano.line), tag(_ano.tag),
		childs(_ano.childs) {}
	strToken(strToken &&_ano) : lexeme(move(_ano.lexeme)), line(_ano.line), tag(_ano.tag),
		childs(move(_ano.childs)) {}
	strToken &operator=(const strToken &ano) { this->strToken::strToken(ano); return *this; }

	string lexeme;			//词法单元
	size_t line;				//行号
	TokenTagType tag;			//词法单元类型
	vector<strToken> childs;       //词法单元孩子节点
};

// 语法树
using SyntaxTree = Token;