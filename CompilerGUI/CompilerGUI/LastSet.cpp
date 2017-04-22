#include "public.h"

void LastSet::generateLastSet(Grammar & g)
{
	boolTable lastTable(SubProc::generateLastTabel(g));
	g.setTerminalBoolTableSet(Grammar::LastSet, lastTable);
}

boolTable LastSet::SubProc::generateLastTabel(Grammar & g)
{
	boolTable lastTable(g.getTerminalNonterminalBoolTable());
	dealLastTable(lastTable, g.terminals());
	return move(lastTable);
}

void LastSet::SubProc::dealLastTable(boolTable & lastTable, const map<string, Terminal>& terminals)
{
	map<string, set<string>> lastNonterminalsSet;
	for (const pair<const string, Terminal> &PairTerminal : terminals)
	{
		if (PairTerminal.second.isNonTerminal())
		{
			lastNonterminalsSet.insert(make_pair(PairTerminal.first, set<string>()));
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
					lastTable[thisTerminal.name()][finalTerminal] = true;
				}
				else
				{
					for (deque<string>::const_reverse_iterator currIt(thisProduction.terminals.crbegin()),
						edIt(thisProduction.terminals.crend()); currIt != edIt; ++currIt)
					{
						const Terminal &targetTerminal(terminals.find(*currIt)->second);
						if (!targetTerminal.isNonTerminal())
						{
							lastTable[thisTerminal.name()][targetTerminal.name()] = true;
						}
						else
						{
							lastNonterminalsSet[thisTerminal.name()].insert(targetTerminal.name());
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

	for (; refreshLastTable(lastTable, lastNonterminalsSet););
}

bool LastSet::SubProc::refreshLastTable(boolTable & lastTable, const map<string, set<string>>& lastNonterminalsSet)
{
	bool flag(false);
	for (pair<const string, map<string, bool>> &thisLine : lastTable)
	{
		const TerminalSet &thisLastNonterminalSet(lastNonterminalsSet.find(thisLine.first)->second);
		for (const string &targetTerminal : thisLastNonterminalSet)
		{
			for (const pair<const string, bool> &hasLast : lastTable[targetTerminal])
			{
				if (!thisLine.second[hasLast.first] && hasLast.second)
				{
					flag = true;
					thisLine.second[hasLast.first] = true;
				}
			}
		}
	}

	return flag;
}