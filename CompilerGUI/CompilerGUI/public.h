#pragma once
#include "Grammar.h"

using namespace std;

using AnalyzeLine = tuple<deque<string>, deque<string>, string>;
using AnalyzeTable = vector<AnalyzeLine>;

namespace PublicFun
{
	// 根据符号表切割表达式
	deque<string> splitExpr(const string &expr, const Grammar &g);
	// 以HTML富文本格式打印分析表
	ostream &printAnalyzeTableToHtml(ostream & os, const AnalyzeTable &table);
}

namespace LeftRecursive
{
	// 消除左递归
	void eliminateLeftRecursive(Grammar &g);

	namespace SubProc
	{
		// 消除循环左递归
		vector<Terminal::Production> eliminateLoopLeftRecursive(map<string, Terminal> &terminals,
			Terminal &thisTerminal, const set<string> &terminalSet);
		// 消除直接左递归
		void eliminateDirectLeftRecursive(map<string, Terminal> & terminals,
			Terminal &thisTerminal, vector<Terminal::Production> &_newProductions);
	}
}

namespace FirstSet
{
	// 生成first集
	void generateFirstSet(Grammar &g);

	namespace SubProc
	{
		boolTable generateFirstTable(Grammar &g);
		void dealFirstTable(boolTable &firstTable, const map<string, Terminal> &terminals);
		bool refreshFirstTable(boolTable &firstTable, const map<string, set<string>> &firstNonterminalsSet);
	}
}

namespace FollowSet
{
	// 生成follow集
	void generateFollowSet(Grammar &g);

	namespace SubProc
	{
		boolTable generateFollowTable(Grammar &g);
		void dealFollowTable(boolTable &followTable, const map<string, Terminal> &terminals);
		bool refreshFollowTable(boolTable &followTable, const map<string, set<string>> &followNonterminalsSet);
	}
}

namespace LastSet
{
	// 生成last集
	void generateLastSet(Grammar &g);

	namespace SubProc
	{
		boolTable generateLastTabel(Grammar &g);
		void dealLastTable(boolTable &lastTable, const map<string, Terminal> &terminals);
		bool refreshLastTable(boolTable &lastTable, const map<string, set<string>> &lastNonterminalsSet);
	}
}

namespace FirstVtSet
{
	// 生成firstVt集
	void generateFirstVtSet(Grammar &g);
	
	namespace SubProc
	{
		boolTable generateFirstVtTable(Grammar &g);
		void dealFirstVtTable(boolTable &firstVtTable, const map<string, Terminal> &terminals);
		bool refreshFirstVtTable(boolTable &firstVtTable, const map<string, set<string>> &firstVtNonterminalsSet);
	}
}

namespace LastVtSet
{
	// 生成lastVt集
	void generateLastVtSet(Grammar &g);

	namespace SubProc
	{
		boolTable generateLastVtTable(Grammar &g);
		void dealLastVtTable(boolTable &lastVtTable, const map<string, Terminal> &terminals);
		bool refresLastVtTable(boolTable &lastVtTable, const map<string, set<string>> &lastVtNonterminalsSet);
	}
}