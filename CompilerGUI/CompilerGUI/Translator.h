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

// 符号信息
using SymbolInfo = struct strSymbolInfo
{
	SymbolTagType tag;
	size_t addr;
	unsigned value;
};
// 作用域内符号表
using SymbolTable = map<string, SymbolInfo>;
// 以作用域构建的符号表树
using SymbolTree = struct strSymbolTree
{
	string nodeName;
	size_t deepth;
	strSymbolTree *parent;
	// 该过程的参数个数
	size_t argNum;
	size_t line;
	// 该作用域的符号表
	SymbolTable thisSymbolTable;
	// 子作用域-子符号表结点
	vector<strSymbolTree> childs;
};

class PL0Translator
{
	friend class SingleTon<PL0Translator>;

public:
	// 利用语法制导翻译器将抽象语法树翻译成符号表树和目标代码
	vector<tuple<unsigned char, unsigned char, unsigned>> Translate(const SyntaxTree &syntaxTree);

private:
	PL0Translator() {};
	void init();

private:
	// 符号表树
	SymbolTree symbolTree;

private:
	// 从当前符号表结点往上层作用域寻找符号
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