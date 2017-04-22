#pragma once
#include "Translator.h"
#include <sstream>
#include <iterator>
#include <algorithm>

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::Translate(const SyntaxTree &syntaxTree)
{
	init();
	auto codes = TranslateProg(syntaxTree);
	for (size_t i(0), j(codes.size()); i != j; ++i)
	{
		unsigned char operationCode(get<0>(codes[i]));
		if (operationCode == OperationCode::JMP || operationCode == OperationCode::JPC)
		{
			get<2>(codes[i]) += i;
		}
	}
	return move(codes);
}

void PL0Translator::init()
{
	symbolTree.nodeName.clear();
	symbolTree.deepth = 0;
	symbolTree.thisSymbolTable.clear();
	symbolTree.childs.clear();
	symbolTree.parent = nullptr;
}

SymbolTree * PL0Translator::findSymbol(SymbolTree & thisSymbolTree, const string & symbol)
{
	for (SymbolTree *pCurr(&thisSymbolTree); pCurr != nullptr; pCurr = pCurr->parent)
	{
		if (pCurr->thisSymbolTable.find(symbol) != pCurr->thisSymbolTable.cend())
			return pCurr;
	}
	return nullptr;
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateProg(const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes;
	symbolTree.nodeName = syntaxTree.childs[1].childs[0].lexeme;
	for (const SyntaxTree &child : syntaxTree.childs[3].childs)
	{
		if (child.tag == TokenTag::phrase)
		{
			if (child.lexeme.compare("condecl") == 0)
			{
				TranslateConstv(symbolTree, child.childs[1]);
			}
			else if (child.lexeme.compare("vardecl") == 0)
			{
				TranslateID(symbolTree, child.childs[1]);
			}
			else if (child.lexeme.compare("proc") == 0)
			{
				const SyntaxTree *thisSyntaxTree(&child);
				const string &name(thisSyntaxTree->childs[1].childs[0].lexeme);
				auto findIt(find_if(symbolTree.childs.cbegin(), symbolTree.childs.cend(), [&name]
				(const SymbolTree &thisTree)->bool {return thisTree.nodeName == name; }));
				if (findIt != symbolTree.childs.cend())
				{
					ostringstream sout;
					sout << "In line " << thisSyntaxTree->line << ": repeated identifier \'" << name << "\'." << endl;
					throw logic_error(sout.str());
				}

				auto it(symbolTree.childs.insert(symbolTree.childs.end(),
					SymbolTree({ child.childs[1].childs[0].lexeme, 1, &symbolTree })));
				it->line = codes.size() + 1;
				vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateProc(*it, child));
				tempCodes.push_back(make_tuple(OperationCode::OPR, 0, 0));
				move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));

				if (thisSyntaxTree->childs.size() != 5)
				{
					thisSyntaxTree = &(thisSyntaxTree->childs[5]);

					while (true)
					{
						const SyntaxTree *thisProc(&thisSyntaxTree->childs[0]);
						const string &name(thisProc->childs[1].childs[0].lexeme);
						auto findIt(find_if(symbolTree.childs.cbegin(), symbolTree.childs.cend(), [&name]
						(const SymbolTree &thisTree)->bool {return thisTree.nodeName == name; }));
						if (findIt != symbolTree.childs.cend())
						{
							ostringstream sout;
							sout << "In line " << thisProc->line << ": repeated identifier \'" << name << "\'." << endl;
							throw logic_error(sout.str());
						}

						auto it(symbolTree.childs.insert(symbolTree.childs.end(),
							SymbolTree({ thisProc->childs[1].childs[0].lexeme, 1, &symbolTree })));
						it->line = codes.size() + 1;
						vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateProc(*it, *thisProc));
						tempCodes.push_back(make_tuple(OperationCode::OPR, 0, 0));
						move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));

						if (thisSyntaxTree->childs.size() == 1)
							break;
						thisSyntaxTree = &(thisSyntaxTree->childs[1]);
					}
				}
			}
			else if (child.lexeme.compare("body") == 0)
			{
				codes.insert(codes.cbegin(), make_tuple(OperationCode::JMP, 0, codes.size() + 1));
				vector<unsigned> values(symbolTree.thisSymbolTable.size(), 0);
				size_t count(0);

				for (const pair<const string, SymbolInfo> &pairSymbol : symbolTree.thisSymbolTable)
				{
					if (pairSymbol.second.tag == SymbolTag::constant)
					{
						values[pairSymbol.second.addr] = pairSymbol.second.value;
						++count;
					}
				}
				codes.push_back(make_tuple(OperationCode::INT, 0, values.size()));
				for (size_t i(0); i != count; ++i)
				{
					codes.push_back(make_tuple(OperationCode::LIT, 0, values[i]));
					codes.push_back(make_tuple(OperationCode::STO, 0, i));
				}

				if (child.childs.size() == 3)
				{
					vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateBody(symbolTree, child));
					move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));
				}
			}
		}
	}
	codes.push_back(make_tuple(OperationCode::OPR, 0, 0));
	return move(codes);
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateProc(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes;
	if (syntaxTree.childs[2].childs.size() == 3)
	{
		TranslateID(thisSymbolTree, syntaxTree.childs[2].childs[1]);
	}
	thisSymbolTree.argNum = thisSymbolTree.thisSymbolTable.size();
	for (const SyntaxTree &child : syntaxTree.childs[4].childs)
	{
		if (child.tag == TokenTag::phrase)
		{
			if (child.lexeme.compare("condecl") == 0)
			{
				TranslateConstv(thisSymbolTree, child.childs[1]);
			}
			else if (child.lexeme.compare("vardecl") == 0)
			{
				TranslateID(thisSymbolTree, child.childs[1]);
			}
			else if (child.lexeme.compare("proc") == 0)
			{
				const SyntaxTree *thisSyntaxTree(&child);
				const string &name(thisSyntaxTree->childs[1].childs[0].lexeme);
				auto findIt(find_if(thisSymbolTree.childs.cbegin(), thisSymbolTree.childs.cend(), [&name]
				(const SymbolTree &thisTree)->bool {return thisTree.nodeName == name; }));
				if (findIt != thisSymbolTree.childs.cend())
				{
					ostringstream sout;
					sout << "In line " << thisSyntaxTree->line << ": repeated identifier \'" << name << "\'." << endl;
					throw logic_error(sout.str());
				}

				auto it(thisSymbolTree.childs.insert(thisSymbolTree.childs.end(),
					SymbolTree({ child.childs[1].childs[0].lexeme, thisSymbolTree.deepth + 1, &thisSymbolTree })));
				it->line = it->parent->line + codes.size() + 1;
				vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateProc(*it, child));
				tempCodes.push_back(make_tuple(OperationCode::OPR, 0, 0));
				move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));

				if (thisSyntaxTree->childs.size() != 5)
				{
					thisSyntaxTree = &(thisSyntaxTree->childs[5]);

					while (true)
					{
						const SyntaxTree *thisProc(&thisSyntaxTree->childs[0]);
						const string &name(thisProc->childs[1].childs[0].lexeme);
						auto findIt(find_if(thisSymbolTree.childs.cbegin(), thisSymbolTree.childs.cend(), [&name]
						(const SymbolTree &thisTree)->bool {return thisTree.nodeName == name; }));
						if (findIt != thisSymbolTree.childs.cend())
						{
							ostringstream sout;
							sout << "In line " << thisProc->line << ": repeated identifier \'" << name << "\'." << endl;
							throw logic_error(sout.str());
						}

						auto it(thisSymbolTree.childs.insert(thisSymbolTree.childs.end(),
							SymbolTree({ thisProc->childs[1].childs[0].lexeme, thisSymbolTree.deepth + 1, &thisSymbolTree })));
						it->line = it->parent->line + codes.size() + 1;
						vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateProc(*it, *thisProc));
						tempCodes.push_back(make_tuple(OperationCode::OPR, 0, 0));
						move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));

						if (thisSyntaxTree->childs.size() == 1)
							break;
						thisSyntaxTree = &(thisSyntaxTree->childs[1]);
					}
				}
			}
			else if (child.lexeme.compare("body") == 0)
			{
				codes.insert(codes.cbegin(), make_tuple(OperationCode::JMP, 0, codes.size() + 1));

				if (child.childs.size() == 3)
				{
					vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateBody(thisSymbolTree, child));
					move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));
				}
			}
		}
	}
	return move(codes);
}

void PL0Translator::TranslateConstv(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	const SyntaxTree *thisSyntaxTree(&syntaxTree);
	while (true)
	{
		const SyntaxTree *thisConstv(&thisSyntaxTree->childs[0]);
		const string &name(thisConstv->childs[0].childs[0].lexeme);

		if (thisSymbolTree.thisSymbolTable.find(name) != thisSymbolTree.thisSymbolTable.cend())
		{
			ostringstream sout;
			sout << "In line " << thisSyntaxTree->line << ": repeated identifier \'" << name << "\'." << endl;
			throw logic_error(sout.str());
		}

		const unsigned value(stoul(thisConstv->childs[2].childs[0].lexeme));
		thisSymbolTree.thisSymbolTable.insert(make_pair(name, SymbolInfo{ SymbolTag::constant, thisSymbolTree.thisSymbolTable.size(), value }));

		if (thisSyntaxTree->childs.size() == 1)
			break;
		thisSyntaxTree = &thisSyntaxTree->childs[1];
	}
}

void PL0Translator::TranslateID(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	const SyntaxTree *thisSyntaxTree(&syntaxTree);
	while (true)
	{
		const SyntaxTree *thisId(&thisSyntaxTree->childs[0]);
		const string &name(thisId->childs[0].lexeme);

		if (thisSymbolTree.thisSymbolTable.find(name) != thisSymbolTree.thisSymbolTable.cend())
		{
			ostringstream sout;
			sout << "In line " << thisSyntaxTree->line << ": repeated identifier \'" << name << "\'." << endl;
			throw logic_error(sout.str());
		}

		thisSymbolTree.thisSymbolTable.insert(make_pair(name, SymbolInfo{ SymbolTag::variate, thisSymbolTree.thisSymbolTable.size(), 0 }));

		if (thisSyntaxTree->childs.size() == 1)
			break;
		thisSyntaxTree = &thisSyntaxTree->childs[2];
	}
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateBody(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	if (syntaxTree.childs.size() == 2)
		return vector<tuple<unsigned char, unsigned char, unsigned>>();
	else
	{
		vector<tuple<unsigned char, unsigned char, unsigned>> codes;
		const SyntaxTree *thisSyntaxTree(&syntaxTree.childs[1]);
		while (true)
		{
			const SyntaxTree *thisStatement(&thisSyntaxTree->childs[0]);

			vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateStatement(thisSymbolTree, *thisStatement));
			move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));

			if (thisSyntaxTree->childs.size() == 1)
				break;
			thisSyntaxTree = &thisSyntaxTree->childs[1];
		}
		return move(codes);
	}
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateStatement(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	if (syntaxTree.childs[0].lexeme.compare("id") == 0)
	{
		return TranslateAssignment(thisSymbolTree, syntaxTree);
	}
	else if (syntaxTree.childs[0].lexeme.compare("if") == 0)
	{
		if (syntaxTree.childs.size() == 4)
			return TranslateIf(thisSymbolTree, syntaxTree);
		else
			return TranslateIfElse(thisSymbolTree, syntaxTree);
	}
	else if (syntaxTree.childs[0].lexeme.compare("while") == 0)
	{
		return TranslateWhileDo(thisSymbolTree, syntaxTree);
	}
	else if (syntaxTree.childs[0].lexeme.compare("call") == 0)
	{
		return TranslateCall(thisSymbolTree, syntaxTree);
	}
	else if (syntaxTree.childs[0].lexeme.compare("body") == 0)
	{
		return TranslateBody(thisSymbolTree, syntaxTree.childs[0]);
	}
	else if (syntaxTree.childs[0].lexeme.compare("read") == 0)
	{
		return TranslateRead(thisSymbolTree, syntaxTree);
	}
	else if (syntaxTree.childs[0].lexeme.compare("write") == 0)
	{
		return TranslateWrite(thisSymbolTree, syntaxTree);
	}
	else
	{
		return vector<tuple<unsigned char, unsigned char, unsigned>>();
	}
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateAssignment(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes;
	const string &name(syntaxTree.childs[0].childs[0].lexeme);
	SymbolTree *pCurr(&thisSymbolTree);

	if (pCurr->thisSymbolTable[name].tag == SymbolTag::constant)
	{
		ostringstream sout;
		sout << "In line " << syntaxTree.line << ": Semantic error, constant \'" << name << "\' cannot be assigned." << endl;
		throw logic_error(sout.str());
	}

	codes = TranslateExp(thisSymbolTree, syntaxTree.childs[2]);
	codes.push_back(make_tuple(OperationCode::STO, pCurr->deepth, pCurr->thisSymbolTable[name].addr));

	return move(codes);
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateIf(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes(TranslateLexp(thisSymbolTree, syntaxTree.childs[1]));
	vector<tuple<unsigned char, unsigned char, unsigned>> temp_codes(TranslateStatement(thisSymbolTree, syntaxTree.childs[3]));

	codes.push_back(make_tuple(OperationCode::JPC, thisSymbolTree.deepth, temp_codes.size() + 1));
	move(temp_codes.begin(), temp_codes.end(), back_inserter(codes));

	return move(codes);
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateIfElse(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes(TranslateLexp(thisSymbolTree, syntaxTree.childs[1]));
	vector<tuple<unsigned char, unsigned char, unsigned>> temp_codes1(TranslateStatement(thisSymbolTree, syntaxTree.childs[3]));
	vector<tuple<unsigned char, unsigned char, unsigned>> temp_codes2(TranslateStatement(thisSymbolTree, syntaxTree.childs[5]));

	codes.push_back(make_tuple(OperationCode::JPC, thisSymbolTree.deepth, temp_codes1.size() + 1));
	move(temp_codes1.begin(), temp_codes1.end(), back_inserter(codes));
	move(temp_codes2.begin(), temp_codes2.end(), back_inserter(codes));

	return move(codes);
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateWhileDo(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes(TranslateLexp(thisSymbolTree, syntaxTree.childs[1]));
	vector<tuple<unsigned char, unsigned char, unsigned>> temp_codes1(TranslateStatement(thisSymbolTree, syntaxTree.childs[3]));

	codes.push_back(make_tuple(OperationCode::JPC, thisSymbolTree.deepth, temp_codes1.size() + 2));
	move(temp_codes1.begin(), temp_codes1.end(), back_inserter(codes));
	codes.push_back(make_tuple(OperationCode::JMP, 0, -long(codes.size())));

	return move(codes);
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateCall(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes;
	const string &funName(syntaxTree.childs[1].childs[0].lexeme);
	auto it(find_if(thisSymbolTree.childs.cbegin(), thisSymbolTree.childs.cend(), [&funName]
	(const SymbolTree &thisSymbolTree)->bool {return thisSymbolTree.nodeName == funName; }));

	if (it == thisSymbolTree.childs.cend())
	{
		ostringstream sout;
		sout << "In line " << syntaxTree.childs[0].line << ": Unexpected procedure identifier \'" << funName << "\'." << endl;
		throw logic_error(sout.str());
	}
	else
	{
		if (syntaxTree.childs.size() == 3)
		{
			codes.push_back(make_tuple(OperationCode::INT, 0, it->thisSymbolTable.size()));
			vector<unsigned> values(it->thisSymbolTable.size(), 0);
			size_t count(0);

			for (const pair<const string, SymbolInfo> &pairSymbol : symbolTree.thisSymbolTable)
			{
				if (pairSymbol.second.tag == SymbolTag::constant)
				{
					values[pairSymbol.second.addr] = pairSymbol.second.value;
					++count;
				}
			}
			for (size_t i(0); i != count; ++i)
			{
				codes.push_back(make_tuple(OperationCode::LIT, 0, values[i]));
				codes.push_back(make_tuple(OperationCode::STO, it->deepth, i));
			}
			codes.push_back(make_tuple(OperationCode::CAL, it->deepth, it->line));
		}
		else if (syntaxTree.childs.size() == 6)
		{
			size_t argNum(0);
			const SyntaxTree *thisSyntaxTree(&syntaxTree.childs[3]);

			while (true)
			{
				const SyntaxTree *thisExp(&thisSyntaxTree->childs[0]);

				vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateExp(thisSymbolTree, *thisExp));
				move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));
				++argNum;

				if (thisSyntaxTree->childs.size() == 1)
					break;
				thisSyntaxTree = &thisSyntaxTree->childs[2];
			}
			if (argNum != it->argNum)
			{
				ostringstream sout;
				if (argNum > it->argNum)
					sout << "In line " << syntaxTree.childs[0].line << ": Too many arguments in procedure \'" << funName << "\'." << endl;
				else
					sout << "In line " << syntaxTree.childs[0].line << ": Not enough arguments in procedure \'" << funName << "\'." << endl;
				throw logic_error(sout.str());
			}
			else
			{
				codes.push_back(make_tuple(OperationCode::INT, 0, it->thisSymbolTable.size()));
				for (int i(argNum - 1); i >= 0; --i)
				{
					codes.push_back(make_tuple(OperationCode::STO, it->deepth, i));
				}
				vector<unsigned> values(it->thisSymbolTable.size(), 0);
				size_t count(0);

				for (const pair<const string, SymbolInfo> &pairSymbol : symbolTree.thisSymbolTable)
				{
					if (pairSymbol.second.tag == SymbolTag::constant)
					{
						values[pairSymbol.second.addr] = pairSymbol.second.value;
						++count;
					}
				}
				for (size_t i(0); i != count; ++i)
				{
					codes.push_back(make_tuple(OperationCode::LIT, 0, values[i]));
					codes.push_back(make_tuple(OperationCode::STO, it->deepth, i));
				}

				codes.push_back(make_tuple(OperationCode::CAL, it->deepth, it->line));
			}
		}
		return move(codes);
	}
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateRead(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes;
	const SyntaxTree *thisSyntaxTree(&syntaxTree.childs[2]);

	while (true)
	{
		const SyntaxTree *thisId(&thisSyntaxTree->childs[0]);
		SymbolTree *pCurr(findSymbol(thisSymbolTree, thisId->childs[0].lexeme));
		if (pCurr != nullptr)
		{
			codes.push_back(make_tuple(OperationCode::RED, thisSymbolTree.deepth, pCurr->thisSymbolTable[thisId->childs[0].lexeme].addr));
		}
		else
		{
			ostringstream sout;
			sout << "In line " << thisId->line << ": Unexpected identifier \'" << thisId->childs[0].lexeme << "\'." << endl;
			throw logic_error(sout.str());
		}

		if (thisSyntaxTree->childs.size() == 1)
			break;
		thisSyntaxTree = &thisSyntaxTree->childs[2];
	}

	return move(codes);
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateWrite(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes;
	const SyntaxTree *thisSyntaxTree(&syntaxTree.childs[2]);
	size_t expNum(0);

	while (true)
	{
		const SyntaxTree *thisExp(&thisSyntaxTree->childs[0]);
		vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateExp(thisSymbolTree, *thisExp));
		move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));
		++expNum;

		if (thisSyntaxTree->childs.size() == 1)
			break;
		thisSyntaxTree = &thisSyntaxTree->childs[2];
	}

	for (int i(expNum - 1); i >= 0; --i)
	{
		codes.push_back(make_tuple(OperationCode::WRT, 0, 0));
	}

	return move(codes);
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateLexp(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes;

	if (syntaxTree.childs.size() == 3)
	{
		vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes1(TranslateExp(thisSymbolTree, syntaxTree.childs[0]));
		vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes2(TranslateExp(thisSymbolTree, syntaxTree.childs[2]));
		move(tempCodes1.begin(), tempCodes1.end(), back_inserter(codes));
		move(tempCodes2.begin(), tempCodes2.end(), back_inserter(codes));

		const string &op(syntaxTree.childs[1].childs[0].lexeme);
		if (op.compare("=") == 0)
			codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::equal));
		else if (op.compare("<>") == 0)
			codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::notequal));
		else if (op.compare("<") == 0)
			codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::less));
		else if (op.compare("<=") == 0)
			codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::lessEqual));
		else if (op.compare(">") == 0)
			codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::greater));
		else if (op.compare(">=") == 0)
			codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::greatreEqual));

	}
	else if (syntaxTree.childs.size() == 2)
	{
		codes = TranslateExp(thisSymbolTree, syntaxTree.childs[1]);
		codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::not));
	}
	else if (syntaxTree.childs.size() == 1)
	{
		codes = TranslateExp(thisSymbolTree, syntaxTree.childs[0]);
	}

	return move(codes);
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateExp(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes;

	if (syntaxTree.childs.size() == 1)
	{
		codes = TranslateTerm(thisSymbolTree, syntaxTree.childs[0]);
	}
	else if (syntaxTree.childs.size() == 2 && syntaxTree.childs[0].lexeme.compare("term") != 0)
	{
		codes = TranslateTerm(thisSymbolTree, syntaxTree.childs[1]);
		if (syntaxTree.childs[0].lexeme.compare("-") == 0)
		{
			codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::negative));
		}
	}
	else
	{
		const SyntaxTree *thisSyntaxTree(nullptr);

		if (syntaxTree.childs.size() == 2)
		{
			thisSyntaxTree = &syntaxTree.childs[1];
			codes = TranslateTerm(thisSymbolTree, syntaxTree.childs[0]);

		}
		else
		{
			thisSyntaxTree = &syntaxTree.childs[2];
			codes = TranslateTerm(thisSymbolTree, syntaxTree.childs[1]);
			if (syntaxTree.childs[0].lexeme.compare("-") == 0)
			{
				codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::negative));
			}
		}

		while (true)
		{
			const SyntaxTree *thisTerm(&thisSyntaxTree->childs[1]);
			vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateTerm(thisSymbolTree, *thisTerm));
			move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));

			if (thisSyntaxTree->childs[0].childs[0].lexeme.compare("+"))
			{
				codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::addition));
			}
			else
			{
				codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::subtraction));
			}

			if (thisSyntaxTree->childs.size() == 2)
				break;
			thisSyntaxTree = &thisSyntaxTree->childs[2];
		}
	}

	return move(codes);
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateTerm(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes(TranslateFactor(thisSymbolTree, syntaxTree.childs[0]));

	if (syntaxTree.childs.size() != 1)
	{
		const SyntaxTree *thisSyntaxTree(&syntaxTree.childs[1]);

		while (true)
		{
			const SyntaxTree *thisFactor(&thisSyntaxTree->childs[1]);
			vector<tuple<unsigned char, unsigned char, unsigned>> tempCodes(TranslateFactor(thisSymbolTree, *thisFactor));
			move(tempCodes.begin(), tempCodes.end(), back_inserter(codes));

			if (thisSyntaxTree->childs[0].childs[0].lexeme.compare("*"))
			{
				codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::multiplication));
			}
			else
			{
				codes.push_back(make_tuple(OperationCode::OPR, thisSymbolTree.deepth, OperatorCode::divison));
			}

			if (thisSyntaxTree->childs.size() == 2)
				break;
			thisSyntaxTree = &thisSyntaxTree->childs[2];
		}
	}
	return move(codes);
}

vector<tuple<unsigned char, unsigned char, unsigned>> PL0Translator::TranslateFactor(SymbolTree & thisSymbolTree, const SyntaxTree & syntaxTree)
{
	vector<tuple<unsigned char, unsigned char, unsigned>> codes;
	if (syntaxTree.childs[0].lexeme.compare("id") == 0)
	{
		const string &name(syntaxTree.childs[0].childs[0].lexeme);
		SymbolTree *pCurr(findSymbol(thisSymbolTree, name));
		if (pCurr != nullptr)
		{
			codes.push_back(make_tuple(OperationCode::LOD, pCurr->deepth, pCurr->thisSymbolTable[name].addr));
		}
		else
		{
			ostringstream sout;
			sout << "In line " << syntaxTree.childs[0].line << ": Unexpected identifier \'" << name << "\'." << endl;
			throw logic_error(sout.str());
		}
		return move(codes);
	}
	else if (syntaxTree.childs[0].lexeme.compare("integer") == 0)
	{
		const unsigned name(stoul(syntaxTree.childs[0].childs[0].lexeme));
		codes.push_back(make_tuple(OperationCode::LIT, 0, name));
		return move(codes);
	}
	else if (syntaxTree.childs[0].lexeme.compare("(") == 0)
	{
		return TranslateExp(thisSymbolTree, syntaxTree.childs[1]);
	}
	else
	{
		ostringstream sout;
		sout << "Unknown Error In line " << syntaxTree.line << "." << endl;
		throw logic_error(sout.str());
	}
}