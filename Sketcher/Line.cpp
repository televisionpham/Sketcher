#include "stdafx.h"
#include "Line.h"


CLine::CLine()
{
}


CLine::~CLine()
{
}

void CLine::Draw(CDC * pDC)
{
	CPen aPen;
	CreatePen(aPen);
	CPen* pOldPen{ pDC->SelectObject(&aPen) };
	pDC->MoveTo(m_StartPoint);
	pDC->LineTo(m_EndPoint);
	pDC->SelectObject(pOldPen);
}

CLine::CLine(const CPoint & start, const CPoint & end, COLORREF color):
	CElement { start, color }, m_EndPoint{ end }
{
	m_EnclosingRect = CRect{ start, end };
	m_EnclosingRect.NormalizeRect();
	m_EnclosingRect.InflateRect(m_PenWidth, m_PenWidth);
}
