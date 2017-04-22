#pragma once

#include <utility>
#include <vector>
#include <string>
#include <map>
#include "Terminal.h"

using namespace std;

using boolTable = map<string, map<string, bool>>;

class Grammar
{
public:
	enum 
	{
		FirstSet,
		FollowSet,
		LastSet,
		FirstVtSet,
		LastVtSet
	};

public:
	Grammar() {};
	Grammar(const vector<string> &inputs);
	Grammar(const Grammar &ano);
	Grammar(const Grammar &&ano);

	map<string, Terminal> &terminals(void);
	const map<string, Terminal> &terminals(void) const;
	const TerminalSet &terminalSet(void) const;
	const TerminalSet &nonterminalSet(void) const;
	const string &startNonterminalName(void) const;
	boolTable getTerminalNonterminalBoolTable(void) const;

	void setStartNonterminal(const string &newStartNonterminal);
	void setTerminalBoolTableSet(const size_t type, const boolTable &table);

	void generateTerminalSetAndNonterminalSet(void);

private:
	void ReadProduction(const vector<string> &strProductions);
	void generateNonterminalTerminalBoolTable(void);

private:
	string startNonTerminal;
	TerminalSet _terminalSet, _nonterminalSet;
	boolTable _tempBoolTable;
	map<string, Terminal> _terminals; // ·ûºÅÃû×Ö-·ûºÅÓ³Éä±í
};