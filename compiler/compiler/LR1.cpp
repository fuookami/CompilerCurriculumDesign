#include "LR1.h"
#include <sstream>
#include <stack>
#include <algorithm>

LR1::LR1(const Grammar && _g)
	:g(_g)
{
	init();
}

LR1::LR1(const Grammar & _g)
	:g(_g)
{
	init();
}

LR1::LR1(const LR1 & ano) :
	g(ano.g), c(ano.c), gotoRelation(ano.gotoRelation)
{
}

LR1::LR1(const LR1 && ano) :
	g(move(ano.g)), c(move(ano.c)), gotoRelation(move(ano.gotoRelation))
{
}

LR1 &LR1::operator=(const Grammar &_g)
{
	this->LR1::LR1(_g);
	return *this;
}

LR1 &LR1::operator=(const Grammar &&_g)
{
	this->LR1::LR1(_g);
	return *this;
}

LR1 & LR1::operator=(const LR1 & ano)
{
	this->LR1::LR1(ano);
	return *this;
}

LR1 & LR1::operator=(const LR1 && ano)
{
	this->LR1::LR1(ano);
	return *this;
}

LR1 & LR1::setGrammar(const Grammar & _g)
{
	this->LR1::LR1(_g);
	return *this;
}

LR1 & LR1::setGrammar(const Grammar && _g)
{
	this->LR1::LR1(_g);
	return *this;
}

void LR1::writeToFile(ostream & os) const
{
	os << gotoRelation.size() << endl;
	for (const pair<const int, map<string, pair<int, int>>> &thisRelation : gotoRelation)
	{
		os << thisRelation.second.size() << endl;
		for (const pair<const string, pair<int, int>> &thisLine : thisRelation.second)
		{
			os << thisLine.first << ' ' << thisLine.second.first << ' ' << thisLine.second.second << endl;
		}
	}

	os << c.size() << ' ';
	for (const LR1ItemAndClouserSet &thisItemSet : c)
	{
		static auto LR1ItemSetPrintFun = [&os](const pair<const string, LR1Item> &pairItem)
		{
			os << pairItem.first << ' ';
			pairItem.second.wirteToFile(os);
			os << endl;
		};

		os << thisItemSet.itemSet.size() << endl;
		for_each(thisItemSet.itemSet.cbegin(), thisItemSet.itemSet.cend(), LR1ItemSetPrintFun);
		os << thisItemSet.clousers.size() << endl;
		for_each(thisItemSet.clousers.cbegin(), thisItemSet.clousers.cend(), LR1ItemSetPrintFun);
	}
}

void LR1::readFromFile(istream &is)
{
	size_t relationNum(0);
	is >> relationNum;
	for (size_t i(0); i != relationNum; ++i)
	{
		gotoRelation.insert(make_pair(i, map<string, pair<int, int>>()));
		map<string, pair<int, int>> &thisRelation(gotoRelation[i]);

		size_t thisRelationLineNum;
		is >> thisRelationLineNum;
		for (size_t j(0); j != thisRelationLineNum; ++j)
		{
			string terminal;
			size_t code, state;
			is >> terminal >> code >> state;
			thisRelation.insert(make_pair(move(terminal), make_pair(code, state)));
		}
	}
	getline(is, string());

	size_t LR1ItemSetNum;
	is >> LR1ItemSetNum;
	for (size_t i(0); i != LR1ItemSetNum; ++i)
	{
		static auto LR1ItemSetReadFun = [&is] () -> LR1ItemSet
		{
			LR1ItemSet thisSet;
			size_t thisLR1ItemSetItemNum;
			is >> thisLR1ItemSetItemNum;
			for (size_t j(0); j != thisLR1ItemSetItemNum; ++j)
			{
				static LR1Item tempItem;
				string nonterminal;
				is >> nonterminal;
				tempItem.readFromFile(is);
				thisSet.insert(make_pair(move(nonterminal), tempItem));
			}

			return move(thisSet);
		};

		LR1ItemAndClouserSet thisItemSet;
		thisItemSet.itemSet = LR1ItemSetReadFun();
		thisItemSet.clousers = LR1ItemSetReadFun();
		c.push_back(move(thisItemSet));
	}
}

SyntaxTree LR1::analysis(const vector<Token>& tokens)
{
	vector<size_t> statementCode;
	statementCode.push_back(0);
	size_t currPos(0);
	deque<Token> terminals;

	while (true)
	{
		const Token &currToken(tokens[currPos]);
		auto it(gotoRelation[statementCode.back()].find(currToken.lexeme));
		if (it == gotoRelation[statementCode.back()].cend())
		{
			ostringstream sout;
			sout << "Syntax error: In line " << currToken.line << endl;
			throw logic_error(sout.str());
		}
		else
		{
			const pair<const int, int> &thisRelation(it->second);
			if (thisRelation.first == LR1::relation::go)
			{
				statementCode.push_back(thisRelation.second);
				terminals.push_back(currToken);
				++currPos;
			}
			else if (thisRelation.first == LR1::relation::reduce)
			{
				const pair<const string, LR1Item> &pairLR1Item(*c[statementCode.back()].itemSet.cbegin());
				size_t thisTerminalsNum(pairLR1Item.second.terminals.size());
				Token tempToken(pairLR1Item.first, terminals[terminals.size() - thisTerminalsNum].line, TokenTag::phrase);

				move(terminals.end() - thisTerminalsNum, terminals.end(), back_inserter(tempToken.childs));
				terminals.erase(terminals.end() - thisTerminalsNum, terminals.end());
				terminals.push_back(move(tempToken));
				statementCode.erase(statementCode.end() - thisTerminalsNum, statementCode.end());

				auto tempIt(gotoRelation[statementCode.back()].find(pairLR1Item.first));
				if (tempIt == gotoRelation[statementCode.back()].cend())
				{
					ostringstream sout;
					sout << "Syntax error: Unexpected start symbol." << endl;
					throw logic_error(sout.str());
				}
				statementCode.push_back(tempIt->second.second);
			}
			else if (thisRelation.first == LR1::relation::accept)
			{
				break;
			}
		}
	}

	if (terminals.size() != 1)
	{
		ostringstream sout;
		sout << "Syntax error: Unexpected start symbol." << endl;
		throw logic_error(sout.str());
	}

	return move(terminals[0]);
}

void LR1::init(void)
{
	// init augmented grammar
	string newStartNonterminalName("_S");
	map<string, Terminal> &terminals(g.terminals());
	terminals.insert(make_pair(newStartNonterminalName, NonTerminal(newStartNonterminalName)));
	terminals[newStartNonterminalName].addProduction(g.startNonterminalName(), terminals);
	g.setStartNonterminal(newStartNonterminalName);
	g.generateTerminalSetAndNonterminalSet();
	
	FirstSet::generateFirstSet(g);
	FollowSet::generateFollowSet(g);
	c = LR1CollectionNS::generateLR1Collection(gotoRelation, g);
}