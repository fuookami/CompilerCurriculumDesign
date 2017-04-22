#include "public.h"
#include <algorithm>

using namespace std;

void LeftRecursive::eliminateLeftRecursive(Grammar & g)
{
	map<string, Terminal> &terminals(g.terminals());
	TerminalSet nonterminalSet(g.nonterminalSet());
	for (string terminalName : nonterminalSet)
	{
		Terminal &thisTerminal(terminals[terminalName]);
		vector<Terminal::Production> newProductions(
			SubProc::eliminateLoopLeftRecursive(terminals, thisTerminal, nonterminalSet));
		SubProc::eliminateDirectLeftRecursive(terminals, thisTerminal, newProductions);
	}
}

vector<Terminal::Production> LeftRecursive::SubProc::eliminateLoopLeftRecursive(map<string, Terminal> &terminals,
	Terminal &thisTerminal, const set<string> &nonterminalSet)
{
	vector<Terminal::Production> newProductions(thisTerminal.productions());
	for (set<string>::iterator nonterminalCurrit(nonterminalSet.begin()), edIt(nonterminalSet.find(thisTerminal.name()));
		nonterminalCurrit != edIt; ++nonterminalCurrit)
	{
		string targetTerminalName(*nonterminalCurrit);
		Terminal &targetTerminal(terminals[targetTerminalName]);
		vector<Terminal::Production>::iterator it(find_if(newProductions.begin(), newProductions.end(),
			[targetTerminalName](const Terminal::Production &thisProduction)->bool
		{return !thisProduction.isFinal && thisProduction[0] == targetTerminalName; }));

		if (it != newProductions.end())
		{
			Terminal::Production _tempProduction(*it);
			newProductions.erase(it);
			_tempProduction.terminals.pop_front();

			Terminal targetTerminal(terminals[targetTerminalName]);
			for (size_t i(0), j(targetTerminal.size()); i < j; ++i)
			{
				Terminal::Production tempProduction(_tempProduction), &targetProduction(targetTerminal[i]);
				tempProduction.terminals.insert(tempProduction.terminals.begin(),
					targetProduction.terminals.begin(), targetProduction.terminals.end());
				newProductions.push_back(move(tempProduction));
			}
		}
	}
	return move(newProductions);
}

void LeftRecursive::SubProc::eliminateDirectLeftRecursive(map<string, Terminal> & terminals,
	Terminal &thisTerminal, vector<Terminal::Production> &_newProductions)
{
	vector<Terminal::Production> newProductions, tempProductions;
	string tempTerminalName(string("_") + thisTerminal.name());
	bool flag(false);
	for (const Terminal::Production &production : _newProductions)
	{
		if (!production.isFinal && production[0] == thisTerminal.name())
		{  // exist direct left recursive
			flag = true;
			break;
		}
	}

	if (flag)
	{
		for (Terminal::Production production : _newProductions)
		{
			if (production[0] == thisTerminal.name())
			{
				production.terminals.pop_front();
				production.terminals.push_back(tempTerminalName);
				tempProductions.push_back(move(production));
			}
			else
			{
				production.terminals.push_back(tempTerminalName);
				newProductions.push_back(move(production));
			}
		}
		thisTerminal.setProductions(move(newProductions));

		terminals.insert(make_pair(tempTerminalName, NonTerminal(tempTerminalName)));
		Terminal &tempTerminal(terminals[tempTerminalName]);
		tempTerminal.setProductions(move(tempProductions));
		tempTerminal.addProduction(makeProduction(finalTerminal, terminals));
	}
	else
	{
		thisTerminal.setProductions(move(_newProductions));
	}
}