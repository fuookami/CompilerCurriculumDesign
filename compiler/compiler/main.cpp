#include "LexicalAnalysis.h"
#include "LR1.h"
#include "Translator.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

using namespace std;

int main(void)
{
	ifstream fin("src\\syntax.txt");
	LR1 *pLR1(SingleTon<LR1>::instance());
	LexicalAnalysis *pLexer(SingleTon<LexicalAnalysis>::instance());
	PL0Translator *pPL0Translator(SingleTon<PL0Translator>::instance());

	if (!fin.is_open())
	{
		fin.close();
		ifstream gfin("src\\grammar.txt");
		vector<string> inputs;
		for (string temp; getline(gfin, temp);)
		{
			inputs.push_back(temp);
		}
		gfin.close();
		Grammar grammar(inputs);
		pLR1->setGrammar(move(grammar));

		ofstream fout("src\\syntax.txt");
		pLR1->writeToFile(fout);
		fout.close();
	}
	else
	{
		pLR1->readFromFile(fin);
		fin.close();
	}


	while (1)
	{
		fin.open("src\\codea.txt");
		vector<Token> lexicalUnits;
		try
		{
			lexicalUnits = pLexer->scan(fin);
		}
		catch (logic_error &e)
		{
			fin.close();
			cout << e.what();
			continue;
		}
		fin.close();

		SyntaxTree syntaxTree;
		try
		{
			lexicalUnits.push_back(Token(finalTerminal, lexicalUnits.back().line + 1, TokenTag::keyword));
			syntaxTree = pLR1->analysis(lexicalUnits);
		}
		catch (logic_error &e)
		{
			fin.close();
			cout << e.what();
			continue;
		}

		vector<tuple<unsigned char, unsigned char, unsigned>> target;
		try
		{
			target = pPL0Translator->Translate(syntaxTree);
		}
		catch (logic_error &e)
		{
			cout << e.what();
		}

		for (size_t i(0), j(target.size()); i != j; ++i)
		{
			cout << i << ' ';
			switch (get<0>(target[i]))
			{
			case LIT:
				cout << "LIT" << ' ';
				break;
			case OPR:
				cout << "OPR" << ' ';
				break;
			case LOD:
				cout << "LOD" << ' ';
				break;
			case STO:
				cout << "STO" << ' ';
				break;
			case CAL:
				cout << "CAL" << ' ';
				break;
			case INT:
				cout << "INT" << ' ';
				break;
			case JMP:
				cout << "JMP" << ' ';
				break;
			case JPC:
				cout << "JPC" << ' ';
				break;
			case RED:
				cout << "RED" << ' ';
				break;
			case WRT:
				cout << "WRT" << ' ';
				break;
			default:
				break;
			}
			cout << int(get<1>(target[i])) << ' ' << get<2>(target[i]) << endl;
		}
	}

	system("pause");
	return 0;
}