#include "stdafx.h"
#include "Curve.h"
#include <algorithm>

CCurve::CCurve()
{
}


CCurve::~CCurve()
{
}

void CCurve::Draw(CDC * pDC, std::shared_ptr<CElement> pElement)
{
	CPen aPen;
	CreatePen(aPen, pElement);
	CPen* pOldPen{ pDC->SelectObject(&aPen) };
	pDC->MoveTo(m_StartPoint);
	for (const auto& point : m_Points)
	{
		pDC->LineTo(point);
	}
	pDC->SelectObject(pOldPen);
}

void CCurve::Move(const CSize & aSize)
{
	m_EnclosingRect += aSize;
	m_StartPoint += aSize;
	for (auto& p : m_Points)
	{
		p += aSize;
	}
}

CCurve::CCurve(const CPoint & first, const CPoint & second, COLORREF color, int penWidth):
	CElement {first, color, penWidth}
{
	m_Points.push_back(second);
	m_EnclosingRect = CRect{
		(std::min)(first.x, second.x), (std::min)(first.y, second.y),
		(std::max)(first.x, second.x), (std::max)(first.y, second.y)
	};
	int width{ penWidth == 0 ? 1 : penWidth };
	m_EnclosingRect.InflateRect(width, width);
}

void CCurve::AddSegment(const CPoint & point)
{
	m_Points.push_back(point);
	int width{ m_PenWidth == 0 ? 1 : m_PenWidth };
	m_EnclosingRect.DeflateRect(width, width);
	m_EnclosingRect = CRect{
		(std::min)(m_EnclosingRect.left, point.x), (std::min)(m_EnclosingRect.top, point.y),
		(std::max)(m_EnclosingRect.right, point.x), (std::max)(m_EnclosingRect.bottom, point.y)
	};
	m_EnclosingRect.InflateRect(width, width);
}
