#include "Grammar.h"
#include <algorithm>

Grammar::Grammar(const vector<string> &inputs)
{
	vector<string> strTerminals, strNonTerminals;
	vector<string> strProductions;
	split(strTerminals, inputs[0]);
	split(strNonTerminals, inputs[1]);
	startNonTerminal = inputs[2];
	strProductions.insert(strProductions.begin(), inputs.cbegin() + 3, inputs.cend());

	for (const string &terminalName : strTerminals)
	{
		_terminalSet.insert(terminalName);
		_terminals.insert(make_pair(terminalName, Terminal(terminalName)));
	}
	for (const string &nonterminalName : strNonTerminals)
	{
		_nonterminalSet.insert(nonterminalName);
		_terminals.insert(make_pair(nonterminalName, NonTerminal(nonterminalName)));
	}
	generateTerminalSetAndNonterminalSet();

	ReadProduction(strProductions);
}

Grammar::Grammar(const Grammar & ano)
	: startNonTerminal(ano.startNonTerminal),
	_terminalSet(ano._terminalSet),
	_nonterminalSet(ano._nonterminalSet),
	_terminals(ano._terminals),
	_tempBoolTable(ano._tempBoolTable)
{

}

Grammar::Grammar(const Grammar && ano)
	: startNonTerminal(move(ano.startNonTerminal)),
	_terminalSet(move(ano._terminalSet)),
	_nonterminalSet(move(ano._nonterminalSet)),
	_terminals(move(ano._terminals)),
	_tempBoolTable(move(ano._tempBoolTable))
{
}

map<string, Terminal>& Grammar::terminals(void)
{
	return _terminals;
}

const map<string, Terminal>& Grammar::terminals(void) const
{
	return _terminals;
}

const string & Grammar::startNonterminalName(void) const
{
	return startNonTerminal;
}

boolTable Grammar::getTerminalNonterminalBoolTable(void) const
{
	return _tempBoolTable;
}

void Grammar::generateTerminalSetAndNonterminalSet(void)
{
	_terminalSet.clear();
	_nonterminalSet.clear();
	for (const pair<const string, Terminal> &PairTerminals : _terminals)
	{
		if (PairTerminals.second.isNonTerminal())
		{
			_nonterminalSet.insert(PairTerminals.first);
		}
		else
		{
			_terminalSet.insert(PairTerminals.first);
		}
	}
	generateNonterminalTerminalBoolTable();
}

void Grammar::setStartNonterminal(const string & newStartNonterminal)
{
	startNonTerminal = newStartNonterminal;
}

void Grammar::setTerminalBoolTableSet(const size_t type, const boolTable & table)
{
	auto setFirstTable = [&table](pair<const string, Terminal> &PairTerminal) {
		if (PairTerminal.second.isNonTerminal())
			PairTerminal.second.setFirst(table.find(PairTerminal.first)->second);
	};
	auto setFollowTable = [&table](pair<const string, Terminal> &PairTerminal) {
		if (PairTerminal.second.isNonTerminal())
			PairTerminal.second.setFollow(table.find(PairTerminal.first)->second);
	};
	auto setLastTable = [&table](pair<const string, Terminal> &PairTerminal) {
		if (PairTerminal.second.isNonTerminal())
			PairTerminal.second.setLast(table.find(PairTerminal.first)->second);
	};
	auto setFirstVtTable = [&table](pair<const string, Terminal> &PairTerminal) {
		if (PairTerminal.second.isNonTerminal())
			PairTerminal.second.setFirstVt(table.find(PairTerminal.first)->second);
	};
	auto setLastVtTable = [&table](pair<const string, Terminal> &PairTerminal) {
		if (PairTerminal.second.isNonTerminal())
			PairTerminal.second.setLastVt(table.find(PairTerminal.first)->second);
	};
	
	switch (type)
	{
		case FirstSet:
			for_each(_terminals.begin(), _terminals.end(), setFirstTable);
			break;
		case FollowSet:
			for_each(_terminals.begin(), _terminals.end(), setFollowTable);
			break;
		case LastSet:
			for_each(_terminals.begin(), _terminals.end(), setLastTable);
			break;
		case FirstVtSet:
			for_each(_terminals.begin(), _terminals.end(), setFirstVtTable);
			break;
		case LastVtSet:
			for_each(_terminals.begin(), _terminals.end(), setLastVtTable);
			break;
		default:
			break;
	}
}

const TerminalSet & Grammar::terminalSet(void) const
{
	return _terminalSet;
}

const TerminalSet & Grammar::nonterminalSet(void) const
{
	return _nonterminalSet;
}

void Grammar::ReadProduction(const vector<string> &strProductions)
{
	for (const string &production : strProductions)
	{
		vector<string> temp;
		split(temp, production, string("->"));
		if (temp.size() == 2)
		{// if this temp be split to two parts which are nonterminal part and productions part
			string &nonterminalName = temp[0];
			auto pPairNonterminal = _terminals.find(nonterminalName);
			if (pPairNonterminal != _terminals.cend())
			{// if this nonterminal is existed
				vector<string> productions;
				split(productions, temp[1], string("|"));
				for (string &production : productions)
				{// add production
					pPairNonterminal->second.addProduction(production, _terminals);
				}
			}
		}
	}
}


void Grammar::generateNonterminalTerminalBoolTable(void)
{
	_tempBoolTable.clear();
	map<string, bool> tableLine;

	TerminalSet tempTerminalSet = _terminalSet;
	tempTerminalSet.insert(finalTerminal);

	for (const string &terminalName : tempTerminalSet)
	{
		tableLine.insert(make_pair(terminalName, false));
	}
	for (const string &nonterminalName : _nonterminalSet)
	{
		_tempBoolTable.insert(make_pair(nonterminalName, tableLine));
	}
}