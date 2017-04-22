#include "MainWid.h"
#include <QtWidgets\QFileDialog>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <utility>
#include <regex>

MainWid::MainWid(QMainWindow * parent):
	QMainWindow(parent), pLexer(SingleTon<LexicalAnalysis>::instance()),
	pLR1(SingleTon<LR1>::instance()), pTran(SingleTon<PL0Translator>::instance())
{
	init();

	ui = new Ui_PL0ComplierMainWid;
	ui->setupUi(this);

	ui->Run->setEnabled(false);
	ui->Compile->setEnabled(false);
	HighLighter *highligher = new HighLighter(ui->CodeEdit->document());

	connect(ui->SearchFile, SIGNAL(clicked()), this, SLOT(SelectFile()));
	connect(ui->Compile, SIGNAL(clicked()), this, SLOT(Compile()));
	connect(ui->Run, SIGNAL(clicked()), this, SLOT(Run()));
}

void MainWid::SelectFile()
{
	currFileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"), " ", QString::fromLocal8Bit("PL0代码文件(*.pl0);;文本文档(*.txt);;所有文档(*.*)"));
	ui->FileUrl->setText(currFileName);
	ui->Compile->setEnabled(true);
	ui->Run->setEnabled(false);
	ui->Info->clear();

	ifstream fin(currFileName.toLocal8Bit());
	stringstream sout;
	sout << fin.rdbuf();
	ui->CodeEdit->setText(QString::fromStdString(sout.str()));
	fin.close();
}

void MainWid::Compile()
{
	try
	{
		ofstream fout(currFileName.toLocal8Bit());
		fout << ui->CodeEdit->toPlainText().toStdString();
		fout.close();

		QString tempQstring;
		ifstream fin(currFileName.toLocal8Bit());
		auto lexicalUnits(pLexer->scan(fin));
		fin.close();

		lexicalUnits.push_back(Token(finalTerminal, lexicalUnits.back().line + 1, TokenTag::keyword));
		tempQstring += QString::fromLocal8Bit("Lexical analyze successfully.\n");
		auto syntaxTree(pLR1->analysis(lexicalUnits));
		tempQstring += QString::fromLocal8Bit("Syntax analyze successfully.\n");
		auto target(pTran->Translate(syntaxTree));
		tempQstring += QString::fromLocal8Bit("Translate successfully.\n");
		
		regex fileSuffixExpression("\\.\\w*");
		currTargetName = regex_replace(string(currFileName.toLocal8Bit()), fileSuffixExpression, "$1.asm");
		fout.open(currTargetName, ofstream::binary);
		for (size_t i(0), j(target.size()); i != j; ++i)
		{
			fout.write(reinterpret_cast<char *>(&get<0>(target[i])), 1);
			fout.write(reinterpret_cast<char *>(&get<1>(target[i])), 1);
			fout.write(reinterpret_cast<char *>(&get<2>(target[i])), 4);
		}
		fout.close();

		ui->Info->setText(tempQstring);
		ui->Run->setEnabled(true);
	}
	catch (logic_error &e)
	{
		ui->Info->setText(QString::fromLocal8Bit(e.what()));
	}
}

void MainWid::Run()
{
	Compile();
	string cmd("src\\interpreter.exe ");
	cmd += currTargetName;
	system(cmd.c_str());
}

void MainWid::init()
{
	ifstream fin("src\\syntax.txt");
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
		pLR1->setGrammar(std::move(grammar));

		ofstream fout("src\\syntax.txt");
		pLR1->writeToFile(fout);
		fout.close();
	}
	else
	{
		pLR1->readFromFile(fin);
		fin.close();
	}
}