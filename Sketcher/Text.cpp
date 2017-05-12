#include "stdafx.h"
#include "Text.h"


CText::CText()
{
}


CText::CText(const CPoint & start, const CPoint & end, const CString & aString, COLORREF color)
{
	m_String = aString;
	m_EnclosingRect = CRect{ start, end };
	m_EnclosingRect.NormalizeRect();
	m_EnclosingRect.InflateRect(m_PenWidth, m_PenWidth);
}

void CText::Draw(CDC * pDC, std::shared_ptr<CElement> pElement)
{
	pDC->SetTextColor(this == pElement.get() ? SELECT_COLOR : m_Color);
	pDC->SetBkMode(TRANSPARENT);
	pDC->ExtTextOut(m_StartPoint.x, m_StartPoint.y, 0, nullptr, m_String, nullptr);
}

void CText::Move(const CSize & aSize)
{
	m_EnclosingRect += aSize;
	m_StartPoint += aSize;
}

CText::~CText()
{
}
