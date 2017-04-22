#pragma once

#include <vector>
#include <map>
#include <set>
#include <memory>

using namespace std;

class Terminal;
class Grammar;

void split(vector<string> &dest, const string &source, const string str = string(" "));

using TerminalSet = set<string>;

static const string finalTerminal = "$";
enum type
{
	terminal,
	nonTerminal
};

// 指令码表
enum OperationCode
{
	LIT,
	OPR,
	LOD,
	STO,
	CAL,
	INT,
	JMP,
	JPC,
	RED,
	WRT
};

// 操作符号表
enum OperatorCode
{
	addition = 1,
	subtraction,
	multiplication,
	divison,
	equal,
	notequal,
	less,
	lessEqual,
	greater,
	greatreEqual,
	negative,
	not
};

// 单例
template <class T>
class SingleTon
{
public:
	static T *instance(void);
protected:
	SingleTon(void) {};
	SingleTon(const SingleTon &) {};
	~SingleTon(void) {};
	SingleTon &operator=(const SingleTon &) {};
private:
	static std::shared_ptr<T> m_pInstance;
};

template <class T>
std::shared_ptr<T> SingleTon<T>::m_pInstance = nullptr;

template <class T>
T *SingleTon<T>::instance(void)
{
	if (m_pInstance == nullptr)
	{
		m_pInstance.reset(new T());
	}
	return m_pInstance.get();
}