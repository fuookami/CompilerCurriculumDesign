#include "LexicalAnalysis.h"
#include <sstream>
#include <regex>

LexicalAnalysis::LexicalAnalysis()
{
	for (const string &keyWord : KeyWords)
	{
		regexs.push_back(shared_ptr<regex>(new regex(keyWord)));
	}
	regexs.push_back(shared_ptr<regex>(new regex(string("[a-zA-Z][a-zA-Z0-9]*"))));
	regexs.push_back(shared_ptr<regex>(new regex(string("0|([1-9][0-9]*)"))));
}

vector<Token> LexicalAnalysis::scan(istream &is) const
{
	vector<Token> tokens;

	size_t line(0);
	string lineTemp;
	while (getline(is, lineTemp))
	{
		++line;
		for (string::const_iterator currIt(lineTemp.cbegin()), edIt(lineTemp.cend()); currIt != edIt; )
		{
			if (isspace(*currIt))
			{
				++currIt;
				continue;
			}
			size_t i(0);
			bool flag(false);
			for (size_t j(regexs.size()); i != j; ++i)
			{
				smatch cm;
				if (regex_search(currIt, edIt, cm, *(regexs[i]), regex_constants::match_continuous))
				{
					if (i < j - 2)
					{
						tokens.push_back(Token(cm[0].str(), line, TokenTag::keyword));
					}
					else if(i == j - 2)
					{
						static Token tempToken(string("id"), line, TokenTag::keyword);
						tempToken.childs.assign({ Token(cm[0].str(), line, TokenTag::id) });
						tokens.push_back(tempToken);
					}
					else
					{
						static Token tempToken(string("integer"), line, TokenTag::keyword);
						tempToken.childs.assign({ Token(cm[0].str(), line, TokenTag::integer) });
						tokens.push_back(tempToken);
					}
					currIt += cm[0].str().size();
					flag = true;
					break;
				}
			}
			if(flag)
				continue;

			ostringstream sout;
			sout << "In line " << line << ": There are unknown or illegal identifier." << endl;
			sout << "It may be in the list: ";
			string tempString(currIt, edIt);
			vector<string> identifiers;
			split(identifiers, tempString);
			sout << identifiers[0];
			for (size_t i(1), j(identifiers.size()); i != j; ++i)
				sout << ", " << identifiers[i];
			sout << endl;
			throw logic_error(sout.str());
		}
	}

	return move(tokens);
}