#include "public.h"

void FirstVtSet::generateFirstVtSet(Grammar & g)
{
	boolTable firstVtTable(SubProc::generateFirstVtTable(g));
	g.setTerminalBoolTableSet(Grammar::FirstVtSet, firstVtTable);
}

boolTable FirstVtSet::SubProc::generateFirstVtTable(Grammar & g)
{
	boolTable firstVtTable(g.getTerminalNonterminalBoolTable());
	dealFirstVtTable(firstVtTable, g.terminals());
	return move(firstVtTable);
}

void FirstVtSet::SubProc::dealFirstVtTable(boolTable & firstVtTable, const map<string, Terminal>& terminals)
{
	map<string, set<string>> firstVtNonterminalsSet;
	for (const pair<const string, Terminal> &PairTerminal : terminals)
	{
		if (PairTerminal.second.isNonTerminal())
		{
			firstVtNonterminalsSet.insert(make_pair(PairTerminal.first, set<string>()));
		}
	}

	for (const pair<const string, Terminal> &PairTerminal : terminals)
	{
		if (PairTerminal.second.isNonTerminal())
		{
			const Terminal &thisTerminal(PairTerminal.second);
			const vector<Terminal::Production> &productions(thisTerminal.productions());
			for (const Terminal::Production &thisProduction : productions)
			{
				if (!thisProduction.isFinal)
				{
					const Terminal &targetTerminal(terminals.find(thisProduction[0])->second);
					if (targetTerminal.isNonTerminal())
					{
						firstVtNonterminalsSet[thisTerminal.name()].insert(targetTerminal.name());
						if (thisProduction.size() > 1)
						{
							const TerminalSet &tempTerminalFirst(terminals.find(thisProduction[1])->second.first());
							for (const string &terminal : tempTerminalFirst)
							{
								if (terminal != finalTerminal)
								{
									firstVtTable[thisTerminal.name()][terminal] = true;
								}
							}
						}
					}
					else
					{
						firstVtTable[thisTerminal.name()][targetTerminal.name()] = true;
					}
				}
			}
		}
	}

	for (; refreshFirstVtTable(firstVtTable, firstVtNonterminalsSet););
}

bool FirstVtSet::SubProc::refreshFirstVtTable(boolTable & firstVtTable, const map<string, set<string>>& firstVtNonterminalsSet)
{
	bool flag(false);
	for (pair<const string, map<string, bool>> &thisLine : firstVtTable)
	{
		const TerminalSet &thisFirstVtNonterminalSet(firstVtNonterminalsSet.find(thisLine.first)->second);
		for (const string &targetNonterminal : thisFirstVtNonterminalSet)
		{
			for (const pair<const string, bool> &pairTerminalIsFirstVt : firstVtTable[targetNonterminal])
			{
				if (!thisLine.second[pairTerminalIsFirstVt.first] && pairTerminalIsFirstVt.second)
				{
					flag = true;
					thisLine.second[pairTerminalIsFirstVt.first] = true;
				}
			}
		}
	}

	return flag;
}
