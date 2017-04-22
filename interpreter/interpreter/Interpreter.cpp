#include "Interpreter.h"
#include <iomanip>
#include <sstream>

void Interpreter::Run(istream & is)
{
	init(is);

	do 
	{
		const PCode &thisCode = PCodes[codePointer];
		cout << setw(3) << setfill(' ') << codePointer << ' ' << thisCode.toString();
		switch (thisCode.OperationCode)
		{
		case OperationCode::LIT:
			Operation_LIT(thisCode);
			break;
		case OperationCode::OPR:
			Operation_OPR(thisCode);
			break;
		case OperationCode::LOD:
			Operation_LOD(thisCode);
			break;
		case OperationCode::STO:
			Operation_STO(thisCode);
			break;
		case OperationCode::CAL:
			Operation_CAL(thisCode);
			break;
		case OperationCode::INT:
			Operation_INT(thisCode);
			break;
		case OperationCode::JMP:
			Operation_JMP(thisCode);
			break;
		case OperationCode::JPC:
			Operation_JPC(thisCode);
			break;
		case OperationCode::RED:
			Operation_RED(thisCode);
			break;
		case OperationCode::WRT:
			Operation_WRT(thisCode);
			break;
		default:
			break;
		}
	} while (!RuntimeStack.empty());

}

void Interpreter::init(istream & is)
{
	RuntimeStack.clear();
	RuntimeStack.push_back(vector<long>());
	DataStack.clear();
	PCodes.clear();
	codePointer = 0;

	while (is)
	{
		PCode thisPCode;
		is.read(reinterpret_cast<char *>(&thisPCode.OperationCode), 1);
		is.read(reinterpret_cast<char *>(&thisPCode.StackDeepth), 1);
		is.read(reinterpret_cast<char *>(&thisPCode.op), 4);

		PCodes.push_back(move(thisPCode));
	}
}

void Interpreter::Operation_LIT(const PCode & pCode)
{
	RuntimeStack.back().push_back(pCode.op);
	++codePointer;
}

void Interpreter::Operation_OPR(const PCode & pCode)
{
	if (pCode.op == 0)
	{
		DataStack.pop_back();
		RuntimeStack.pop_back();
		if (!RuntimeStack.empty())
		{
			codePointer = RuntimeStack.back().back() + 1;
			RuntimeStack.back().pop_back();
		}
	}
	else
	{
		long op(RuntimeStack.back().back());
		RuntimeStack.back().pop_back();
		switch (pCode.op)
		{
		case OperatorCode::addition:
			RuntimeStack.back().back() += op;
			break;
		case OperatorCode::subtraction:
			RuntimeStack.back().back() -= op;
			break;
		case OperatorCode::multiplication:
			RuntimeStack.back().back() *= op;
			break;
		case OperatorCode::divison:
			RuntimeStack.back().back() /= op;
			break;
		case OperatorCode::equal:
			RuntimeStack.back().back() = (RuntimeStack.back().back() == op) ? 1 : 0;
			break;
		case OperatorCode::notequal:
			RuntimeStack.back().back() = (RuntimeStack.back().back() != op) ? 1 : 0;
			break;
		case OperatorCode::less:
			RuntimeStack.back().back() = (RuntimeStack.back().back() < op) ? 1 : 0;
			break;
		case OperatorCode::lessEqual:
			RuntimeStack.back().back() = (RuntimeStack.back().back() <= op) ? 1 : 0;
			break;
		case OperatorCode::greater:
			RuntimeStack.back().back() = (RuntimeStack.back().back() > op) ? 1 : 0;
			break;
		case OperatorCode::greatreEqual:
			RuntimeStack.back().back() = (RuntimeStack.back().back() >= op) ? 1 : 0;
			break;
		case OperatorCode::negative:
			RuntimeStack.back().push_back(-op);
			break;
		case OperatorCode::not:
			RuntimeStack.back().back() = (RuntimeStack.back().back() == 0) ? 1 : 0;
			break;
		default:
			break;
		}
		++codePointer;
	}
}

void Interpreter::Operation_LOD(const PCode & pCode)
{
	RuntimeStack.back().push_back(DataStack[pCode.StackDeepth][pCode.op]);
	++codePointer;
}

void Interpreter::Operation_STO(const PCode & pCode)
{
	DataStack[pCode.StackDeepth][pCode.op] = RuntimeStack.back().back();
	RuntimeStack.back().pop_back();
	++codePointer;
}

void Interpreter::Operation_CAL(const PCode & pCode)
{
	RuntimeStack.back().push_back(codePointer);
	RuntimeStack.push_back(vector<long>());
}

void Interpreter::Operation_INT(const PCode & pCode)
{
	DataStack.push_back(vector<long>(pCode.op, 0));
	++codePointer;
}

void Interpreter::Operation_JMP(const PCode & pCode)
{
	codePointer = pCode.op;
}

void Interpreter::Operation_JPC(const PCode & pCode)
{
	if (RuntimeStack.back().back() == 0)
	{
		codePointer = pCode.op;
	}
	else
	{
		++codePointer;
	}
	RuntimeStack.back().pop_back();
}

void Interpreter::Operation_RED(const PCode & pCode)
{
	unsigned temp;
	cout << endl << "  >> ";
	cin >> temp;
	cout << endl;
	DataStack[pCode.StackDeepth][pCode.op] = temp;
	++codePointer;
}

void Interpreter::Operation_WRT(const PCode & pCode)
{
	cout << endl;
	cout << "  >> " <<RuntimeStack.back().back() << endl;
	cout << endl;
	RuntimeStack.back().pop_back();
	++codePointer;
}

string strPCode::toString() const
{
	ostringstream sout;
	sout << setw(2) << setfill('0') << hex << (unsigned)OperationCode << ' ';
	sout << setw(2) << setfill('0') << hex << (unsigned)StackDeepth << ' ';
	sout << setw(8) << setfill('0') << hex << op << ' ' << '\t';
	sout << Code[(unsigned)OperationCode] << ' ';
	sout << setw(4) << setfill(' ') << dec << (unsigned)StackDeepth << ' ' << op << endl;
	return sout.str();
}