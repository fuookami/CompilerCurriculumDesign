#pragma once

#include "public.h"
#include "LexicalAnalysis.h"
#include <iostream>

using namespace std;

class LR1
{
	friend class SingleTon<LR1>;
public:
	enum relation {
		go,
		reduce,
		accept
	};

	// LR1��Ŀ
	using LR1Item = struct strLR1Item
	{
		size_t size() const { return terminals.size(); }
		string &operator[](const size_t i) { return terminals[i]; }
		const string &operator[](const size_t i) const { return terminals[i]; }
		ostream &operator<<(ostream &os) const;
		bool operator<(const strLR1Item &ano) const;
		bool operator==(const strLR1Item &ano) const;
		string toString(void) const;

		void wirteToFile(ostream &os) const;
		void readFromFile(istream &is);

		size_t point; // ��λ��
		string next; // ��ǰ������
		deque<string> terminals; // ����ʽ���Ŵ�
		bool isFinal; // �Ƿ��ǿղ���ʽ
	};

	using LR1ItemSet = set<pair<string, LR1Item>>;
	using LR1ItemAndClouserSet = struct strLR1ItemSet // LR1��Ŀ��
	{
		bool operator==(const strLR1ItemSet &ano) const;
		LR1ItemSet itemSet; // ��ǰ��
		LR1ItemSet clousers; // clouser��
	};
	using LR1Collection = vector<LR1ItemAndClouserSet>; // LR1��Ŀ����

public:
	LR1 &operator=(const Grammar &_g);
	LR1 &operator=(const Grammar &&_g);
	LR1 &operator=(const LR1 &ano);
	LR1 &operator=(const LR1 &&ano);

	LR1 &setGrammar(const Grammar &_g);
	LR1 &setGrammar(const Grammar &&_g);

	void writeToFile(ostream &os) const;
	void readFromFile(istream &is);

	// ����LR1�﷨������ɨ��ʷ���Ԫ�����ɳ����﷨��
	SyntaxTree LR1::analysis(const vector<Token>& tokens);

private:
	LR1() {};
	LR1(const Grammar &&_g);
	LR1(const Grammar &_g);
	LR1(const LR1 &ano);
	LR1(const LR1 &&_ano);

private:
	Grammar g;
	LR1Collection c; // LR1��Ŀ����
	map<int, map<string, pair<int, int>>> gotoRelation; // LR1�﷨������

private:
	void init(void);
};

namespace LR1CollectionNS
{
	// ����LR1��Ŀ������﷨������
	LR1::LR1Collection generateLR1Collection(map<int, map<string, pair<int, int>>> &gotoRelation, Grammar &g);

	namespace SubProc
	{
		// ��ʼ��LR1��Ŀ����
		LR1::LR1Collection initLR1Collection(Grammar &g);
		map<string, bool> generateGotoLine(const map<string, Terminal> &terminals);

		// ����LR1��Ŀ����clouser��
		void generateClouserItems(LR1::LR1ItemAndClouserSet &itemSet, const map<string, Terminal> &terminals);
		void refreshClouserItems(LR1::LR1ItemSet &clousers, LR1::LR1ItemSet &items, const map<string, Terminal> &terminals);
	
		// ����LR1��Ŀ����goto��
		LR1::LR1ItemAndClouserSet generateGotoItemSet(const LR1::LR1ItemAndClouserSet &itemSet, const string &nextTerminal,
			const map<string, Terminal> &terminals);
		void refreshGotoItems(LR1::LR1ItemAndClouserSet &gotoItemSet, const LR1::LR1ItemSet &thisItems, const string &nextTerminal,
			const map<string, Terminal> &terminals);
	}
}