#pragma once
#include "Element.h"
#include <algorithm>
class CRectangle :
	public CElement
{
public:	
	virtual ~CRectangle();
	virtual void Draw(CDC* pDC) override;
	CRectangle(const CPoint& start, const CPoint& end, COLORREF color);
protected:
	CRectangle();
	CPoint m_BottomRight;
};

