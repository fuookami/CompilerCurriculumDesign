#include "public.h"
#include <algorithm>

deque<string> PublicFun::splitExpr(const string & expr, const Grammar & g)
{
	const TerminalSet &terminalSet(g.terminalSet());
	deque<string> exprs;
	string::const_iterator p(expr.cbegin());
	string::const_iterator q(expr.cbegin() + 1);
	for (string::const_iterator edIt(expr.cend()); p != edIt; ++q)
	{
		string temp(p, q);
		if (terminalSet.find(temp) != terminalSet.cend())
		{
			exprs.push_back(temp);
			p = q;
		}
		if (q == edIt)
			break;
	}
	if (p != expr.cend())
	{
		return move(deque<string>());
	}
	else
	{
		return move(exprs);
	}
}

ostream & PublicFun::printAnalyzeTableToHtml(ostream & os, const AnalyzeTable &table)
{
	os << "<table><tbody><tr>";
	os << "<td style = 'width: 8em; padding: .5em; text-align: center;'>∑÷Œˆ’ª</td>";
	os << "<td style = 'width: 8em; padding: .5em; text-align: center;'> ‰»Î¥Æ</td>";
	os << "<td style = 'width: 16em; padding: .5em; text-align: center;'>∏Ω◊¢</td>";
	os << "</tr>";
	for (const AnalyzeLine &line : table)
	{
		os << "<tr><td style = 'width: 8em; padding: .5em; text-align: center;'>" << finalTerminal;
		const deque<string> terminals(get<0>(line));
		const deque<string> exprs(get<1>(line));
		const string remark(get<2>(line));
		for_each(terminals.cbegin(), terminals.cend(), [&os](const string &terminal) {os << terminal; });
		os << "</td><td style = 'width: 8em; padding: .5em; text-align: center;'>";
		for_each(exprs.cbegin(), exprs.cend(), [&os](const string &expr) {os << expr; });
		os << finalTerminal << "</td><td style = 'width: 16em; padding: .5em; text-align: center;'>";
		os << remark << "</td></tr>";
	}
	os << "</tbody></table>";
	return os;
}