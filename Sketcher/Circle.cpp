#include "stdafx.h"
#include "Circle.h"
#include <cmath>

IMPLEMENT_SERIAL(CCircle, CElement, VERSION_NUMBER)

CCircle::CCircle()
{
}


CCircle::~CCircle()
{
}

void CCircle::Draw(CDC * pDC, std::shared_ptr<CElement> pElement)
{
	CPen aPen;
	CreatePen(aPen, pElement);
	CPen* pOldPen { pDC->SelectObject(&aPen) };
	CBrush* pOldBrush { dynamic_cast<CBrush*>(pDC->SelectStockObject(NULL_BRUSH)) };
	pDC->Ellipse(m_StartPoint.x, m_StartPoint.y, m_BottomRight.x, m_BottomRight.y);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void CCircle::Move(const CSize & aSize)
{
	m_StartPoint += aSize;
	m_BottomRight += aSize;
	m_EnclosingRect += aSize;
}

CCircle::CCircle(const CPoint & start, const CPoint & end, COLORREF color, int penWidth) :
	CElement{ start, color, penWidth }
{
	long radius{ static_cast<long>(sqrt(
		static_cast<double>((end.x - start.x) * (end.x - start.x) +
			(end.y - start.y) * (end.y - start.y)))) };
	if (radius < 1L)
	{
		radius = 1L;
	}
	m_StartPoint = CPoint{ start.x - radius, start.y - radius };
	m_BottomRight = CPoint{ start.x + radius, start.y + radius };
	m_EnclosingRect = CRect{ m_StartPoint.x, m_StartPoint.y,
		m_BottomRight.x, m_BottomRight.y };

	int width{ penWidth == 0 ? 1 : penWidth };
	m_EnclosingRect.InflateRect(width, width);
}


void CCircle::Serialize(CArchive& ar)
{
	CElement::Serialize(ar);
	if (ar.IsStoring())
	{	// storing code
		ar << m_BottomRight;
	}
	else
	{	// loading code
		ar >> m_BottomRight;
	}
}
