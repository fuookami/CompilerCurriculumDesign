#pragma once

#include "ui_MainWid.h"
#include "LexicalAnalysis.h"
#include "LR1.h"
#include "Translator.h"
#include "Highlighter.h"
#include <QtWidgets\QMainWindow>

class MainWid : public QMainWindow
{
	Q_OBJECT;

public:
	MainWid(QMainWindow *parent = nullptr);

private slots:
	void SelectFile();
	void Compile();
	void Run();

private:
	void init();

private:
	Ui_PL0ComplierMainWid *ui;
	LexicalAnalysis *pLexer;
	LR1 *pLR1;
	PL0Translator *pTran;

private:
	QString currFileName;
	string currTargetName;
};