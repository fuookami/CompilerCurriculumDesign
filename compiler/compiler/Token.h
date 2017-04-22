#pragma once

#include "basic.h"

enum TokenTag
{
	phrase,
	keyword,
	id,
	integer,
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

	string lexeme;
	size_t line;
	TokenTagType tag;
	vector<strToken> childs;
};

using SyntaxTree = Token;