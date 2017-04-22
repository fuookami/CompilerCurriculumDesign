#include "Highlighter.h"

HighLighter::HighLighter(QTextDocument * parent)
	:QSyntaxHighlighter(parent)
{
}

void HighLighter::highlightBlock(const QString & text)
{
	//keywords
	QTextCharFormat keywordFormat;
	keywordFormat.setForeground(Qt::blue);
	keywordFormat.setFontWeight(QFont::Bold);
	QRegExp keywordExpression("\\b(program|const|var|procedure|begin|end|if|then|while|do|call|write|odd|read|else)\\b");

	//comments
	QTextCharFormat commentFormat;
	commentFormat.setForeground(Qt::darkGreen);
	QRegExp startExpression("/\\*");
	QRegExp endExpression("\\*/");
	QRegExp lineCommentExpression("//");

	setCurrentBlockState(0);

	int keywordIndex = text.indexOf(keywordExpression);
	while (keywordIndex >= 0)
	{
		int length = keywordExpression.matchedLength();
		setFormat(keywordIndex, length, keywordFormat);
		keywordIndex = text.indexOf(keywordExpression, keywordIndex + length);
	}

	int startIndex = 0;
	int lineCommentIndex = text.indexOf(lineCommentExpression);
	if (previousBlockState() != 1)
	{
		startIndex = text.indexOf(startExpression);
	}
	if (startIndex == -1 && lineCommentIndex != -1)
	{
		setFormat(lineCommentIndex, text.length() - lineCommentIndex, commentFormat);
	}
	while (startIndex >= 0)
	{
		if (lineCommentIndex != -1 && startIndex > lineCommentIndex)
		{
			setFormat(lineCommentIndex, text.length() - lineCommentIndex, commentFormat);
			break;
		}
		int endIndex = text.indexOf(endExpression, startIndex);
		int commentLength;
		if (endIndex == -1)
		{
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		}
		else
		{
			commentLength = endIndex - startIndex
				+ endExpression.matchedLength();
		}
		setFormat(startIndex, commentLength, commentFormat);
		startIndex = text.indexOf(startExpression,
			startIndex + commentLength);
		lineCommentIndex = text.indexOf(lineCommentExpression, startIndex);
	}
}
