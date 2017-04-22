#include "public.h"

void FollowSet::generateFollowSet(Grammar &g)
{
	boolTable followTable(SubProc::generateFollowTable(g));
	g.setTerminalBoolTableSet(Grammar::FollowSet, followTable);
}

boolTable FollowSet::SubProc::generateFollowTable(Grammar &g)
{
	boolTable followTable(g.getTerminalNonterminalBoolTable());
	followTable[g.startNonterminalName()][finalTerminal] = true;
	dealFollowTable(followTable, g.terminals());
	return move(followTable);
}

void FollowSet::SubProc::dealFollowTable(boolTable & followTable, const map<string, Terminal>& terminals)
{
	map<string, set<string>> followNonterminalsSet;
	for (const pair<const string, Terminal> &PairTerminal : terminals)
	{
		if (PairTerminal.second.isNonTerminal())
		{
			followNonterminalsSet.insert(make_pair(PairTerminal.first, set<string>()));
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
					for (size_t i(0), j(thisProduction.size()); i < j; ++i)
					{
						const Terminal &targetTerminal(terminals.find(thisProduction[i])->second);
						if (targetTerminal.isNonTerminal() && (i + 1) != j)
						{
							const TerminalSet &nextTerminalFirstSet(terminals.find(thisProduction[i + 1])->second.first());
							for (const string &firstTerminal : nextTerminalFirstSet)
							{
								if (firstTerminal != finalTerminal)
								{
									followTable[targetTerminal.name()][firstTerminal] = true;
								}
							}
						}
					}
					for (int i(thisProduction.size() - 1); i >= 0; --i)
					{
						const Terminal &targetTerminal(terminals.find(thisProduction[i])->second);
						const TerminalSet &targetTerminalFirstSet(targetTerminal.first());
						if (!targetTerminal.isNonTerminal())
						{
							break;
						}
						followNonterminalsSet[targetTerminal.name()].insert(thisTerminal.name());
						if (targetTerminalFirstSet.find(finalTerminal) == targetTerminalFirstSet.cend())
						{
							break;
						}
					}
				}
			}
		}
	}

	for (; refreshFollowTable(followTable, followNonterminalsSet););
}

bool FollowSet::SubProc::refreshFollowTable(boolTable & followTable, const map<string, set<string>>& followNonterminalsSet)
{
	bool flag(false);

	for (pair<const string, map<string, bool>> &thisLine : followTable)
	{
		const TerminalSet &thisFollowNonterminalSet(followNonterminalsSet.find(thisLine.first)->second);
		for (const string &targetNonterminal : thisFollowNonterminalSet)
		{
			for (const pair<const string, bool> &hasFollow : followTable[targetNonterminal])
			{
				if (!thisLine.second[hasFollow.first] && hasFollow.second)
				{
					flag = true;
					thisLine.second[hasFollow.first] = true;
				}
			}
		}
	}

	return flag;
}