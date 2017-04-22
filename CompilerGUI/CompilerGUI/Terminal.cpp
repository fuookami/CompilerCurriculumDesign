#include "Terminal.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

Terminal::Terminal(string _name, const bool _type) 
	:_name(_name), _type(_type)
{
	if (!_type) // _type == terminal
	{
		_first.insert(_name);
		_last.insert(_name);
		_firstVt.insert(_name);
		_lastVt.insert(_name);
	}
}

Terminal::Terminal(const Terminal & ano)
	: _type(ano._type), _name(ano._name),
	_productions(ano._productions),
	_first(ano._first), _follow(ano._follow), _last(ano._last),
	_firstVt(ano._firstVt), _lastVt(ano._lastVt)
{
}

void Terminal::addProduction(const string &strProduction, const map<string, Terminal> &terminals)
{
	_productions.push_back(makeProduction(strProduction, terminals));
}

void Terminal::addProduction(Production && production)
{
	_productions.push_back(production);
}

void Terminal::setProductions(vector<Production>&& productions)
{
	clear();
	_productions = productions;
}

void Terminal::setFirst(const map<string, bool> &firstFlag)
{
	_first.clear();
	for (const pair<string, bool> &terminal : firstFlag)
	{
		if (terminal.second)
		{
			_first.insert(terminal.first);
		}
	}
}

void Terminal::setFollow(const map<string, bool> &followFlag)
{
	_follow.clear();
	for (const pair<string, bool> &terminal : followFlag)
	{
		if (terminal.second)
		{
			_follow.insert(terminal.first);
		}
	}
}

void Terminal::setLast(const map<string, bool>& lastFlag)
{
	_last.clear();
	for (const pair<string, bool> &terminal : lastFlag)
	{
		if (terminal.second)
		{
			_last.insert(terminal.first);
		}
	}
}

void Terminal::setFirstVt(const map<string, bool>& firstVtFlag)
{
	_firstVt.clear();
	for (const pair<string, bool> &terminal : firstVtFlag)
	{
		if (terminal.second)
		{
			_firstVt.insert(terminal.first);
		}
	}
}

void Terminal::setLastVt(const map<string, bool> &lastVtFlag)
{
	_lastVt.clear();
	for (const pair<string, bool> &terminal : lastVtFlag)
	{
		if (terminal.second)
		{
			_lastVt.insert(terminal.first);
		}
	}
}

bool Terminal::hasFinalProduction() const
{
	return find_if(_productions.cbegin(), productions().cend(),
		[](const Production &production)->bool {return production.isFinal; })
		!= productions().cend();
}

size_t Terminal::size() const
{
	return _productions.size();
}

Terminal::Production & Terminal::operator[](const size_t i)
{
	return _productions[i];
}

ostream & Terminal::operator<<(ostream & os) const
{
	os << _name << "->";
	if (_productions.size() != 0)
	{
		os << _productions[0].toString();
		for (size_t i(1), j(_productions.size()); i != j; ++i)
		{
			os << "|" << _productions[i].toString();
		}
	}
	return os;
}

string Terminal::toString(void) const
{
	ostringstream sout;
	sout << _name << "->";
	if (_productions.size() != 0)
	{
		sout << _productions[0].toString();
		for (size_t i(1), j(_productions.size()); i != j; ++i)
		{
			sout << "|" << _productions[i].toString();
		}
	}
	return move(sout.str());
}

void Terminal::clear()
{
	_productions.clear();
}

const vector<Terminal::Production>& Terminal::productions() const
{
	return _productions;
}

const string & Terminal::name() const
{
	return _name;
}

const TerminalSet & Terminal::first() const
{
	return _first;
}

const TerminalSet & Terminal::follow() const
{
	return _follow;
}

const TerminalSet & Terminal::last() const
{
	return _last;
}

const TerminalSet & Terminal::firstVt() const
{
	return _firstVt;
}

const TerminalSet & Terminal::lastVt() const
{
	return _lastVt;
}

Terminal::Production makeProduction(const string & strProduction, const map<string, Terminal>& terminals)
{
	Terminal::Production production;
	if (strProduction == finalTerminal)
	{
		production.isFinal = true;
	}
	else if(!strProduction.empty())
	{
		production.isFinal = false;
		
		vector<string> terminals;
		split(terminals, strProduction);
		move(terminals.begin(), terminals.end(), back_inserter(production.terminals));

		/*
		string _strProduction;
		for_each(strProduction.cbegin(), strProduction.cend(), [&_strProduction](const char c)
		{if (c != ' ')_strProduction.push_back(c); });
		string::const_iterator p(_strProduction.cbegin());
		string::const_iterator q(_strProduction.cbegin() + 1);
		for (string::const_iterator edIt(_strProduction.cend()); p != edIt; ++q)
		{
			string temp(p, q);
			if (terminals.find(temp) != terminals.cend())
			{
				production.terminals.push_back(temp);
				p = q;
			}
			if (q == edIt)
				break;
		}
		if (p != _strProduction.cend())
		{
			// error
		}
		*/
		if (production.terminals.empty())
		{
			// error
		}
	}
	else 
	{
		// error
	}

	return move(production);
}

ostream & Terminal::strProduction::operator<<(ostream & os) const
{
	if (this->isFinal)
	{
		os << finalTerminal;
	}
	else
	{
		for (const string &terminal : this->terminals)
		{
			os << terminal;
		}
	}
	return os;
}

string Terminal::strProduction::toString(void) const
{
	ostringstream sout;
	if (this->isFinal)
	{
		sout << finalTerminal;
	}
	else
	{
		for (const string &terminal : this->terminals)
		{
			sout << terminal;
		}
	}
	return move(sout.str());
}