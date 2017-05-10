#pragma once
#include "afx.h"
#include "atltypes.h"
#include <memory>

static const COLORREF SELECT_COLOR{ RGB(255,0,180) };

class CElement :
	public CObject
{
public:	
	virtual ~CElement();
	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement=nullptr) {}
	virtual void Move(const CSize& size){}
	const CRect& GetEnclosingRect() const { return m_EnclosingRect; }
protected:
	CPoint m_StartPoint;
	int m_PenWidth;
	COLORREF m_Color;
	CRect m_EnclosingRect;
protected:
	CElement();
	CElement(const CPoint& start, COLORREF color, int penWidth = 1);
	void CreatePen(CPen& aPen, std::shared_ptr<CElement> pElement=nullptr);
};

