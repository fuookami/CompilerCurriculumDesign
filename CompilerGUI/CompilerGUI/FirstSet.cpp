#include "public.h"

void FirstSet::generateFirstSet(Grammar & g)
{
	boolTable firstTable(SubProc::generateFirstTable(g));
	g.setTerminalBoolTableSet(Grammar::FirstSet, firstTable);
}

boolTable FirstSet::SubProc::generateFirstTable(Grammar &g)
{
	boolTable firstTable(g.getTerminalNonterminalBoolTable());
	dealFirstTable(firstTable, g.terminals());
	return move(firstTable);
}

void FirstSet::SubProc::dealFirstTable(boolTable& firstTable, const map<string, Terminal>& terminals)
{
	map<string, set<string>> firstNonterminalsSet;
	for (const pair<const string, Terminal> &PairTerminal : terminals)
	{
		if (PairTerminal.second.isNonTerminal())
		{
			firstNonterminalsSet.insert(make_pair(PairTerminal.first, set<string>()));
		}
	}

	for (const pair<string, Terminal> &PairTerminal : terminals)
	{
		if (PairTerminal.second.isNonTerminal())
		{
			const Terminal &thisTerminal(PairTerminal.second);
			const vector<Terminal::Production> &productions(thisTerminal.productions());
			for (const Terminal::Production &thisProduction : productions)
			{
				if (thisProduction.isFinal)
				{
					firstTable[thisTerminal.name()][finalTerminal] = true;
				}
				else
				{
					for (deque<string>::const_iterator currIt(thisProduction.terminals.cbegin()), edIt(thisProduction.terminals.cend());
						currIt != edIt; ++currIt)
					{
						const Terminal &targetTerminal(terminals.find(*currIt)->second);
						if (!targetTerminal.isNonTerminal())
						{
							firstTable[thisTerminal.name()][targetTerminal.name()] = true;
							break;
						}
						else
						{
							firstNonterminalsSet[thisTerminal.name()].insert(targetTerminal.name());
							if (!targetTerminal.hasFinalProduction())
							{
								break;
							}
						}
					}
				}
			}
		}
	}

	for (; refreshFirstTable(firstTable, firstNonterminalsSet););
}

bool FirstSet::SubProc::refreshFirstTable(boolTable& firstTable, const map<string, set<string>>& firstNonterminalsSet)
{
	bool flag(false);
	for (pair<const string, map<string, bool>> &thisLine : firstTable)
	{
		const TerminalSet &thisFirstNonterminalSet(firstNonterminalsSet.find(thisLine.first)->second);
		for (const string &targetNonterminal : thisFirstNonterminalSet)
		{
			for (const pair<const string, bool> &hasFirst : firstTable[targetNonterminal])
			{
				if (!thisLine.second[hasFirst.first] && hasFirst.second)
				{
					flag = true;
					thisLine.second[hasFirst.first] = true;
				}
			}
		}
	}

	return flag;
}