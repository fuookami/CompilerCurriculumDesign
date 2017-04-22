#pragma once
#include "basic.h"
#include "LexicalAnalysis.h"
#include <tuple>

enum SymbolTag
{
	constant,
	variate
};
using SymbolTagType = size_t;

using SymbolInfo = struct strSymbolInfo
{
	SymbolTagType tag;
	size_t addr;
	unsigned value;
};
using SymbolTable = map<string, SymbolInfo>;
using SymbolTree = struct strSymbolTree
{
	string nodeName;
	size_t deepth;
	strSymbolTree *parent;
	size_t argNum;
	size_t line;
	SymbolTable thisSymbolTable;
	vector<strSymbolTree> childs;
};

class PL0Translator
{
	friend class SingleTon<PL0Translator>;

public:
	vector<tuple<unsigned char, unsigned char, unsigned>> Translate(const SyntaxTree &syntaxTree);

private:
	PL0Translator() {};
	void init();

private:
	SymbolTree symbolTree;

private:
	SymbolTree *findSymbol(SymbolTree & thisSymbolTree, const string &symbol);

private:
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateProg(const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateProc(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	void TranslateConstv(SymbolTree &symbolTree, const SyntaxTree &syntaxTree);
	void TranslateID(SymbolTree &symbolTree, const SyntaxTree &syntaxTree);

private:
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateBody(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateStatement(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateAssignment(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateIf(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateIfElse(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateWhileDo(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateCall(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateRead(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateWrite(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);

	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateLexp(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateExp(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateTerm(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
	vector<tuple<unsigned char, unsigned char, unsigned>> TranslateFactor(SymbolTree &thisSymbolTree, const SyntaxTree &syntaxTree);
};