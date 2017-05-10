#pragma once
#include "Element.h"
class CCircle :
	public CElement
{
public:	
	virtual ~CCircle();
	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement = nullptr) override;
	virtual void Move(const CSize& aSize) override;
	CCircle(const CPoint& start, const CPoint& end, COLORREF color);
protected:
	CPoint m_BottomRight;
	CCircle();
};

