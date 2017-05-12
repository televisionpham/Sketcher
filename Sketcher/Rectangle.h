#pragma once
#include "Element.h"
#include <algorithm>
class CRectangle :
	public CElement
{
public:	
	virtual ~CRectangle();
	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement = nullptr) override;
	virtual void Move(const CSize& aSize) override;
	CRectangle(const CPoint& start, const CPoint& end, COLORREF color, int penWidth);
protected:
	CRectangle();
	CPoint m_BottomRight;
};

