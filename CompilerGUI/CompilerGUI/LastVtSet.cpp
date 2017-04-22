#include "public.h"

void LastVtSet::generateLastVtSet(Grammar & g)
{
	boolTable lastVtTable(SubProc::generateLastVtTable(g));
	g.setTerminalBoolTableSet(Grammar::LastVtSet, lastVtTable);
}

boolTable LastVtSet::SubProc::generateLastVtTable(Grammar & g)
{
	boolTable lastVtTable(g.getTerminalNonterminalBoolTable());
	dealLastVtTable(lastVtTable, g.terminals());
	return move(lastVtTable);
}

void LastVtSet::SubProc::dealLastVtTable(boolTable & lastVtTable, const map<string, Terminal>& terminals)
{
	map<string, set<string>> lastVtNonterminalsSet;
	for (const pair<const string, Terminal> &PairTerminal : terminals)
	{
		if (PairTerminal.second.isNonTerminal())
		{
			lastVtNonterminalsSet.insert(make_pair(PairTerminal.first, set<string>()));
		}
	}

	for (const pair<const string, Terminal> &PairTerminal : terminals)
	{
		if (PairTerminal.second.isNonTerminal())
		{
			const Terminal &thisTerminal(PairTerminal.second);
			const vector<Terminal::Production> &production(thisTerminal.productions());
			for (const Terminal::Production &thisProduction : production)
			{
				if (!thisProduction.isFinal)
				{
					const Terminal &targetTerminal(terminals.find(thisProduction.terminals.back())->second);
					if (targetTerminal.isNonTerminal())
					{
						lastVtNonterminalsSet[thisTerminal.name()].insert(targetTerminal.name());
						if (thisProduction.size() > 1)
						{
							const TerminalSet &tempTerminalLast(terminals.find(*(thisProduction.terminals.crbegin() + 1))->second.last());
							for (const string &terminal : tempTerminalLast)
							{
								if (terminal != finalTerminal)
								{
									lastVtTable[thisTerminal.name()][terminal] = true;
								}
							}
						}
					}
					else 
					{
						lastVtTable[thisTerminal.name()][targetTerminal.name()] = true;
					}
				}
			}
		}
	}

	for (; refresLastVtTable(lastVtTable, lastVtNonterminalsSet););
}

bool LastVtSet::SubProc::refresLastVtTable(boolTable & lastVtTable, const map<string, set<string>>& lastVtNonterminalsSet)
{
	bool flag(false);
	for (pair<const string, map<string, bool>> &thisLine : lastVtTable)
	{
		const TerminalSet &thisLastVtNonterminalSet(lastVtNonterminalsSet.find(thisLine.first)->second);
		for (const string &targetNonterminal : thisLastVtNonterminalSet)
		{
			for (const pair<const string, bool> &pairTerminalIsLastVt : lastVtTable[targetNonterminal])
			{
				if (!thisLine.second[pairTerminalIsLastVt.first] && pairTerminalIsLastVt.second)
				{
					flag = true;
					thisLine.second[pairTerminalIsLastVt.first] = true;
				}
			}
		}
	}

	return flag;
}
