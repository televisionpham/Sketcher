#include "stdafx.h"
#include "Rectangle.h"

IMPLEMENT_SERIAL(CRectangle, CElement, VERSION_NUMBER)

CRectangle::CRectangle()
{
}


CRectangle::~CRectangle()
{
}

void CRectangle::Draw(CDC * pDC, std::shared_ptr<CElement> pElement)
{
	CPen aPen;
	CreatePen(aPen, pElement);
	CPen* pOldPen{ pDC->SelectObject(&aPen) };
	CBrush* pOldBrush{ dynamic_cast<CBrush*>(pDC->SelectStockObject(NULL_BRUSH)) };
	pDC->Rectangle(m_StartPoint.x, m_StartPoint.y,
		m_BottomRight.x, m_BottomRight.y);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void CRectangle::Move(const CSize & aSize)
{
	m_StartPoint += aSize;
	m_BottomRight += aSize;
	m_EnclosingRect += aSize;
}

CRectangle::CRectangle(const CPoint & start, const CPoint & end, COLORREF color, int penWidth):
	CElement{start, color, penWidth}
{
	m_StartPoint = CPoint{ (std::min)(start.x, end.x), (std::min)(start.y, end.y) };
	m_BottomRight = CPoint{ (std::max)(start.x, end.x), (std::max)(start.y, end.y) };
	if (m_BottomRight.x - m_StartPoint.x < 2)
	{
		m_BottomRight.x = m_StartPoint.x + 2;
	}
	if (m_BottomRight.y - m_StartPoint.y < 2)
	{
		m_BottomRight.y = m_StartPoint.y + 2;
	}

	m_EnclosingRect = CRect{ m_StartPoint, m_BottomRight };
	int width{ penWidth == 0 ? 1 : penWidth };
	m_EnclosingRect.InflateRect(width, width);
}


void CRectangle::Serialize(CArchive& ar)
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
