#include "basic.h"
#include "Terminal.h"

void split(vector<string> &dest, const string &source, const string str)
{
	size_t las(0), pos(0);
	for (; (pos = source.find(str, pos)) != string::npos; )
	{
		if (las != pos)
		{
			dest.push_back(string(source, las, pos - las));
		}
		pos += str.size();
		las = pos;
	}
	string temp(string(source, las, source.size() - las));
	if (!temp.empty() && temp != str)
	{
		dest.push_back(move(temp));
	}
}

void getTerminalAndNonterminalSet(const map<string, Terminal> &terminals, TerminalSet & terminalSet, TerminalSet & nonterminalSet)
{
	for (const pair<const string, Terminal> &PairTerminal : terminals)
	{
		if (PairTerminal.second.isNonTerminal())
		{
			nonterminalSet.insert(PairTerminal.first);
		}
		else
		{
			terminalSet.insert(PairTerminal.first);
		}
	}
}