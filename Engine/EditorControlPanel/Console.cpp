#include "pch.h"
#include "Console.h"
#include <string.h>
#define MAX_LINE_LEN 512

void Console::Init(LineEnteredCallbackFn lineEnteredFn)
{
	SetEventMask(GetEventMask() | ENM_SELCHANGE);
	m_fpLineEntered = lineEnteredFn;
}

void Console::OutputText(const char* text)
{
	int lc = GetLineCount();

	// set cursor back to start of last line
	int lastLineStart = LineIndex(lc - 1);
	SetSel(lastLineStart, lastLineStart);

	// convert incoming string to wchar_t and add cr and newline
	size_t newStrLen = strlen(text);
	wchar_t* pNewStrAsWS = new wchar_t[newStrLen + 3]; // add new line and cr
	mbstowcs(pNewStrAsWS, text, newStrLen);
	pNewStrAsWS[newStrLen] = L'\r';
	pNewStrAsWS[newStrLen + 1] = L'\n';
	pNewStrAsWS[newStrLen + 2] = L'\0';

	// replace last line with the new text (+ newline)
	ReplaceSel(pNewStrAsWS, FALSE);
	delete[] pNewStrAsWS;

	SetSel(GetTextLength(), GetTextLength());

}


void Console::HandleSelchangedMsg(SELCHANGE* pSelChange)
{
	// only allow the last line to be edited
	int lc = GetLineCount();
	long line = LineFromChar(pSelChange->chrg.cpMin);
	SetReadOnly(line != (lc - 1));
}

void Console::OnEnter()
{
	// don't allow the last line to be "split in half" when enter is pressed
	SetSel(GetTextLength(), GetTextLength());

	int lc = GetLineCount();
	wchar_t buf[MAX_LINE_LEN];
	int num = GetLine(lc - 1, buf, MAX_LINE_LEN);
	buf[num] = L'\0';
	buf[num - 1] = L'\0'; // get rid of carriage return character at end

	size_t buflen = wcslen(buf);
	char* cBuf = new char[buflen + 1];
	memset(cBuf, 0, sizeof(char) * (buflen + 1));
	wcstombs(cBuf, buf, buflen);
	ASSERT(m_fpLineEntered);
	m_fpLineEntered(cBuf);
	delete cBuf;
}
