#include "stdafx.h"
#include "Element.h"

IMPLEMENT_SERIAL(CElement, CObject, VERSION_NUMBER)

CElement::CElement()
	: m_StartPoint {}
{
}

CElement::CElement(const CPoint & start, COLORREF color, int penWidth):
	m_StartPoint{ start }, m_PenWidth{ penWidth }, m_Color{ color }
{
}

void CElement::CreatePen(CPen & aPen, std::shared_ptr<CElement> pElement)
{
	if (!aPen.CreatePen(PS_SOLID, m_PenWidth, this == pElement.get() ? SELECT_COLOR : m_Color))
	{
		AfxMessageBox(_T("Pen creation failed."), MB_OK);
		AfxAbort();
	}
}


CElement::~CElement()
{
}


void CElement::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{	// storing code
		ar << m_StartPoint
			<< m_PenWidth
			<< m_Color
			<< m_EnclosingRect;
	}
	else
	{	// loading code
		ar >> m_StartPoint
			>> m_PenWidth
			>> m_Color
			>> m_EnclosingRect;
	}
}
