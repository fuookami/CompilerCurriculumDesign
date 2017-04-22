#pragma once
#include "Grammar.h"

using namespace std;

using AnalyzeLine = tuple<deque<string>, deque<string>, string>;
using AnalyzeTable = vector<AnalyzeLine>;

namespace PublicFun
{
	deque<string> splitExpr(const string &expr, const Grammar &g);
	ostream &printAnalyzeTableToHtml(ostream & os, const AnalyzeTable &table);
}

namespace LeftRecursive
{
	void eliminateLeftRecursive(Grammar &g);

	namespace SubProc
	{
		vector<Terminal::Production> eliminateLoopLeftRecursive(map<string, Terminal> &terminals,
			Terminal &thisTerminal, const set<string> &terminalSet);
		void eliminateDirectLeftRecursive(map<string, Terminal> & terminals,
			Terminal &thisTerminal, vector<Terminal::Production> &_newProductions);
	}
}

namespace FirstSet
{
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
	void generateLastVtSet(Grammar &g);

	namespace SubProc
	{
		boolTable generateLastVtTable(Grammar &g);
		void dealLastVtTable(boolTable &lastVtTable, const map<string, Terminal> &terminals);
		bool refresLastVtTable(boolTable &lastVtTable, const map<string, set<string>> &lastVtNonterminalsSet);
	}
}