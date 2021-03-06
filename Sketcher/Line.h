#pragma once
#include "Element.h"
class CLine :
	public CElement
{
	DECLARE_SERIAL(CLine)
public:	
	virtual ~CLine();
	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement = nullptr) override;	
	virtual void Move(const CSize& aSize) override;
	CLine(const CPoint& start, const CPoint& end, COLORREF color, int penWidth);
protected:
	CPoint m_EndPoint;
protected:
	CLine();
public:
	virtual void Serialize(CArchive& ar) override;
};

