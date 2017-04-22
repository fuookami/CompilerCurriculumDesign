#include "basic.h"
#include "Interpreter.h"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
	ifstream fin(argv[1]);
	Interpreter *pInterpreter(SingleTon<Interpreter>::instance());
	pInterpreter->Run(fin);

	system("pause");

	return 0;
}