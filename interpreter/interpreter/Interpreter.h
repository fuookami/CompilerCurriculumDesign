#pragma once

#include "basic.h"
#include <iostream>

static const vector<string> Code = { "LIT","OPR","LOD","STO","CAL","INT","JMP","JPC","RED","WRT" };

using PCode = struct strPCode
{
	string toString() const;

	unsigned char OperationCode;
	unsigned char StackDeepth;
	unsigned op;
};

class Interpreter
{
	friend class SingleTon<Interpreter>;

public:
	// ִ��
	void Run(istream &is);
private:
	Interpreter() {};
	// ��ʼ��
	void init(istream &is);

private:
	void Operation_LIT(const PCode &pCode);
	void Operation_OPR(const PCode &pCode);
	void Operation_LOD(const PCode &pCode);
	void Operation_STO(const PCode &pCode);
	void Operation_CAL(const PCode &pCode);
	void Operation_INT(const PCode &pCode);
	void Operation_JMP(const PCode &pCode);
	void Operation_JPC(const PCode &pCode);
	void Operation_RED(const PCode &pCode);
	void Operation_WRT(const PCode &pCode);

private:
	// ��ǰָ��λ��
	size_t codePointer;
	// �ڴ浥Ԫջ
	vector<vector<long>> DataStack;
	// ����ջ
	vector<vector<long>> RuntimeStack;
	// Ŀ���ļ�ָ���
	vector<PCode> PCodes;
};