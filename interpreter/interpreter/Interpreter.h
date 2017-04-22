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
	// 执行
	void Run(istream &is);
private:
	Interpreter() {};
	// 初始化
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
	// 当前指令位置
	size_t codePointer;
	// 内存单元栈
	vector<vector<long>> DataStack;
	// 数据栈
	vector<vector<long>> RuntimeStack;
	// 目标文件指令表
	vector<PCode> PCodes;
};