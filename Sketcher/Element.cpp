#include "stdafx.h"
#include "Element.h"


CElement::CElement()
	: m_StartPoint {}
{
}

CElement::CElement(const CPoint & start, COLORREF color, int penWidth):
	m_StartPoint{ start }, m_PenWidth{ penWidth }, m_Color{ color }
{
}

void CElement::CreatePen(CPen & aPen)
{
	if (!aPen.CreatePen(PS_SOLID, m_PenWidth, m_Color))
	{
		AfxMessageBox(_T("Pen creation failed."), MB_OK);
		AfxAbort();
	}
}


CElement::~CElement()
{
}
