#include "LR1.h"
#include <algorithm>
#include <iterator>
#include <sstream>

ostream & LR1::strLR1Item::operator<<(ostream & os) const
{
	if (this->isFinal)
	{
		os << "`";
	}
	else
	{
		for (size_t i(0), j(point); i != j; ++i)
		{
			os << terminals[i];
		}
		os << "`";
		for (size_t i(point), j(terminals.size()); i != j; ++i)
		{
			os << terminals[i];
		}
	}
	return os;
}

bool LR1::strLR1Item::operator<(const strLR1Item & ano) const
{
	if (isFinal || ano.isFinal)
		return false;
	else
	{
		if (point != ano.point)
			return point < ano.point;
		else if (terminals != ano.terminals)
			return terminals < ano.terminals;
		else if (next != ano.next)
			return next < ano.next;
		else
			return false;
	}
}

bool LR1::strLR1Item::operator==(const strLR1Item & ano) const
{
	return isFinal == ano.isFinal && point == ano.point 
		&& terminals == ano.terminals && next == ano.next;
}

string LR1::strLR1Item::toString(void) const
{
	ostringstream sout;
	if (this->isFinal)
	{
		sout << "`";
	}
	else
	{
		for (size_t i(0), j(point); i != j; ++i)
		{
			sout << terminals[i];
		}
		sout << "`";
		for (size_t i(point), j(terminals.size()); i != j; ++i)
		{
			sout << terminals[i];
		}
	}
	return sout.str();
}

void LR1::strLR1Item::wirteToFile(ostream & os) const
{
	os << point << ' ' << next << ' ' << terminals.size() << ' ';
	for (size_t i(0), j(terminals.size()); i != j; ++i)
	{
		os << terminals[i] << ' ';
	}
	os << (isFinal ? 1 : 0) << ' ';
}

void LR1::strLR1Item::readFromFile(istream & is)
{
	terminals.clear();
	size_t terminalsSize;
	unsigned uIsFinal;

	is >> point >> next >> terminalsSize;
	terminals.insert(terminals.end(), terminalsSize, string());
	for (size_t i(0); i != terminalsSize; ++i)
		is >> terminals[i];
	is >> uIsFinal;
	isFinal = uIsFinal == 1;
}

bool LR1::strLR1ItemSet::operator==(const strLR1ItemSet &ano) const
{
	return itemSet == ano.itemSet && clousers == ano.clousers;
}

LR1::LR1Collection LR1CollectionNS::generateLR1Collection(map<int, map<string, pair<int, int>>> &gotoRelation, Grammar &g)
{
	LR1::LR1Collection c = SubProc::initLR1Collection(g);
	const map<string, Terminal> &terminals(g.terminals());
	map<string, bool> gotoLine(SubProc::generateGotoLine(terminals));
	vector<map<string, bool>> gotoTable;

	size_t lastSize(0);
	while (c.size() != lastSize)
	{
		gotoTable.insert(gotoTable.end(), c.size() - lastSize, gotoLine);
		for (size_t i(lastSize), j(c.size()); i != j; ++i)
		{
			gotoRelation.insert(make_pair(i, map<string, pair<int, int>>()));
		}
		size_t i = lastSize;
		lastSize = c.size();

		for (size_t j(lastSize); i != j; ++i)
		{
			for (const pair<const string, Terminal> &pairTerminal : terminals)
			{
				if (!gotoTable[i][pairTerminal.first] && pairTerminal.first != g.startNonterminalName())
				{
					LR1::LR1ItemAndClouserSet tempGotoItemSet(SubProc::generateGotoItemSet(
						c[i], pairTerminal.first, terminals));
					if (tempGotoItemSet.itemSet.empty() && c[i].itemSet.begin()->second.point == c[i].itemSet.begin()->second.size())
					{
						if (c[i].itemSet.begin()->first == g.startNonterminalName())
						{
							gotoRelation[i].insert(make_pair(finalTerminal, make_pair(2, 0)));
						}
						else
						{
							for (const pair<string, LR1::LR1Item> &pairItem : c[i].itemSet)
							{
								const string &next(pairItem.second.terminals.back());
								gotoRelation[i].insert(make_pair(pairItem.second.next, make_pair(1, gotoRelation[0][next].second)));
							}
						}
					}
					else if (!tempGotoItemSet.itemSet.empty())
					{
						auto itItem(find(c.begin(), c.end(), tempGotoItemSet));
						gotoTable[i][pairTerminal.first] = true;

						if (itItem == c.end())
						{
							c.push_back(tempGotoItemSet);
							gotoRelation[i].insert(make_pair(pairTerminal.first, make_pair(0, c.size() - 1)));
						}
						else
						{
							pair<string, pair<int, int>> newRelation(make_pair(pairTerminal.first, make_pair(0, itItem - c.begin())));
							map<string, pair<int, int>> &thisRelations(gotoRelation[i]);
							if (thisRelations.find(newRelation.first) == thisRelations.end())
								thisRelations.insert(move(newRelation));
						}
					}
				}
			}
		}
	}

	return move(c);
}

LR1::LR1Collection LR1CollectionNS::SubProc::initLR1Collection(Grammar &g)
{
	LR1::LR1Collection c;
	const map<string, Terminal> &terminals(g.terminals());
	const Terminal &startNonterminal(terminals.find(g.startNonterminalName())->second);
	const vector<Terminal::Production> &productions(startNonterminal.productions());

	LR1::LR1ItemAndClouserSet I0;
	I0.itemSet.insert(make_pair(startNonterminal.name(),
		LR1::LR1Item({ 0, finalTerminal, productions[0].terminals, false })));
	SubProc::generateClouserItems(I0, terminals);
	back_inserter(c) = move(I0);

	return move(c);
}

map<string, bool> LR1CollectionNS::SubProc::generateGotoLine(const map<string, Terminal>& terminals)
{
	map<string, bool> gotoLine;
	for (const pair<const string, Terminal> &pairTerminal : terminals)
	{
		gotoLine.insert(make_pair(pairTerminal.first, false));
	}

	return move(gotoLine);
}

void LR1CollectionNS::SubProc::generateClouserItems(LR1::LR1ItemAndClouserSet &itemSet, const map<string, Terminal> &terminals)
{
	LR1::LR1ItemSet &clousers(itemSet.clousers);
	size_t lastSize(-1);
	while (clousers.size() != lastSize)
	{
		lastSize = clousers.size();
		refreshClouserItems(clousers, itemSet.itemSet, terminals);
		refreshClouserItems(clousers, itemSet.clousers, terminals);
	}
}

void LR1CollectionNS::SubProc::refreshClouserItems(LR1::LR1ItemSet &clousers, LR1::LR1ItemSet & items, const map<string, Terminal>& terminals)
{
	LR1::LR1ItemSet newClousers;
	for (const pair<string, LR1::LR1Item> &pairThisItem : items)
	{
		const LR1::LR1Item &thisItem(pairThisItem.second);
		if (thisItem.isFinal || thisItem.point == thisItem.size())
			continue;

		const Terminal &thisTerminal(terminals.find(thisItem[thisItem.point])->second);
		if (thisTerminal.isNonTerminal())
		{
			TerminalSet nextSet;
			if (thisItem.point == thisItem.size() - 1 || terminals.find(thisItem[thisItem.point + 1])->second.hasFinalProduction())
			{
				if (thisItem.next == finalTerminal)
					nextSet.insert(finalTerminal);
				else
				{
					const TerminalSet &nextFirst(terminals.find(thisItem.next)->second.first());
					nextSet.insert(nextFirst.cbegin(), nextFirst.cend());
				}
			}
			if (thisItem.point != thisItem.size() - 1)
			{
				const TerminalSet &nextTerminalFirst(terminals.find(thisItem[thisItem.point + 1])->second.first());
				nextSet.insert(nextTerminalFirst.cbegin(), nextTerminalFirst.cend());
			}
			if (thisTerminal.isNonTerminal())
			{
				const vector<Terminal::Production> &thisProductions(thisTerminal.productions());
				for (const Terminal::Production &thisProduction : thisProductions)
				{
					for (const string &nextTerminal : nextSet)
					{
						newClousers.insert(make_pair(thisTerminal.name(), LR1::LR1Item(
						{ 0, nextTerminal, thisProduction.terminals, thisProduction.isFinal }
						)));
					}
				}
			}
		}
	}

	clousers.insert(newClousers.cbegin(), newClousers.cend());
}

LR1::LR1ItemAndClouserSet LR1CollectionNS::SubProc::generateGotoItemSet(const LR1::LR1ItemAndClouserSet &itemSet,
	const string &nextTerminal, const map<string, Terminal> &terminals)
{
	LR1::LR1ItemAndClouserSet gotoItemSet;
	refreshGotoItems(gotoItemSet, itemSet.itemSet, nextTerminal, terminals);
	refreshGotoItems(gotoItemSet, itemSet.clousers, nextTerminal, terminals);

	generateClouserItems(gotoItemSet, terminals);
	return move(gotoItemSet);
}

void LR1CollectionNS::SubProc::refreshGotoItems(LR1::LR1ItemAndClouserSet & gotoItemSet,
	const LR1::LR1ItemSet & thisItems, const string & nextTerminal,
	const map<string, Terminal>& terminals)
{
	for (const pair<string, LR1::LR1Item> &pairThisItem : thisItems)
	{
		if (!pairThisItem.second.isFinal && pairThisItem.second.point != pairThisItem.second.size()
			&& pairThisItem.second[pairThisItem.second.point] == nextTerminal)
		{
			pair<string, LR1::LR1Item> tempItem(pairThisItem);
			++tempItem.second.point;
			gotoItemSet.itemSet.insert(move(tempItem));
		}
	}
}