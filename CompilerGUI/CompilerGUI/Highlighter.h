#pragma once

#include <QSyntaxHighlighter>

class HighLighter :public QSyntaxHighlighter
{
public:
	HighLighter(QTextDocument *parent);

	void highlightBlock(const QString &text);
};