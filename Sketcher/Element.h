#pragma once
#include "afx.h"
#include "atltypes.h"
class CElement :
	public CObject
{
public:	
	virtual ~CElement();
	virtual void Draw(CDC* pDC) {}
	const CRect& GetEnclosingRect() const { return m_EnclosingRect; }
protected:
	CPoint m_StartPoint;
	int m_PenWidth;
	COLORREF m_Color;
	CRect m_EnclosingRect;
protected:
	CElement();
	CElement(const CPoint& start, COLORREF color, int penWidth = 1);
	void CreatePen(CPen& aPen);
};

