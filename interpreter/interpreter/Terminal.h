#pragma once

#include <deque>
#include <string>
#include <map>
#include <iostream>
#include "basic.h"

using namespace std;

class Terminal
{
	friend class Grammar;

public:
	using Production = struct strProduction
	{
		size_t size() const { return terminals.size(); }
		string &operator[](const size_t i) { return terminals[i]; }
		const string &operator[](const size_t i) const { return terminals[i]; }
		ostream &operator<<(ostream &os) const;
		string toString(void) const;

		deque<string> terminals;
		bool isFinal;
	};

public:
	Terminal(string _name = string(), const bool _type = terminal);
	Terminal(const Terminal &ano);
	void addProduction(const string &strProduction, const map<string, Terminal> &terminals);
	void addProduction(Production &&production);
	void setProductions(vector<Production> &&productions);

	bool isNonTerminal() const { return _type; };
	bool hasFinalProduction() const;

	size_t size() const;
	Production &operator[](const size_t i);
	ostream &operator<<(ostream &os) const;
	string toString(void) const;
	void clear();

	const string &name() const;
	const vector<Production> &productions() const;
	const TerminalSet &first() const;
	const TerminalSet &follow() const;
	const TerminalSet &last() const;
	const TerminalSet &firstVt() const;
	const TerminalSet &lastVt() const;

private:
	void setFirst(const map<string, bool> &firstFlag);
	void setFollow(const map<string, bool> &followFlag);
	void setLast(const map<string, bool> &lastFlag);
	void setFirstVt(const map<string, bool> &firstVtFlag);
	void setLastVt(const map<string, bool> &lastVtFlag);

private:
	bool _type;
	string _name;
	vector<Production> _productions;
	TerminalSet _first;
	TerminalSet _follow;
	TerminalSet _last;
	TerminalSet _firstVt;
	TerminalSet _lastVt;
};

class NonTerminal: public Terminal
{
public:
	NonTerminal(string _name) :Terminal(_name, nonTerminal) {};
};

Terminal::Production makeProduction(const string &strProduction, const map<string, Terminal> &terminals);
