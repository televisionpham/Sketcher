#pragma once

class CPrintData
{
public:
	UINT printWidth{ 1000 };
	UINT printLength{ 1000 };
	UINT m_nWidths;
	UINT m_nLengths;
	CPoint m_DocRefPoint;
	CString m_DocTitle;
};