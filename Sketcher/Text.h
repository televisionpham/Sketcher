#pragma once
#include "Element.h"
class CText :
	public CElement
{
	DECLARE_SERIAL(CText)

public:
	CText(const CPoint& start, const CPoint& end, const CString& aString, COLORREF color);
	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement = nullptr) override;
	virtual void Move(const CSize& aSize) override;
	virtual ~CText();
protected:
	CString m_String;
	CText();
public:
	virtual void Serialize(CArchive& ar) override;
};

