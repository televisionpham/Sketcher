
// SketcherDoc.cpp : implementation of the CSketcherDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Sketcher.h"
#endif

#include "SketcherDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSketcherDoc

IMPLEMENT_DYNCREATE(CSketcherDoc, CDocument)

BEGIN_MESSAGE_MAP(CSketcherDoc, CDocument)
	ON_COMMAND(ID_COLOR_BLACK, &CSketcherDoc::OnColorBlack)
	ON_COMMAND(ID_COLOR_RED, &CSketcherDoc::OnColorRed)
	ON_COMMAND(ID_COLOR_GREEN, &CSketcherDoc::OnColorGreen)
	ON_COMMAND(ID_COLOR_BLUE, &CSketcherDoc::OnColorBlue)
	ON_COMMAND(ID_ELEMENT_LINE, &CSketcherDoc::OnElementLine)
	ON_COMMAND(ID_ELEMENT_RECTANGLE, &CSketcherDoc::OnElementRectangle)
	ON_COMMAND(ID_ELEMENT_CIRCLE, &CSketcherDoc::OnElementCircle)
	ON_COMMAND(ID_ELEMENT_CURVE, &CSketcherDoc::OnElementCurve)
	ON_UPDATE_COMMAND_UI(ID_COLOR_BLACK, &CSketcherDoc::OnUpdateColorBlack)
	ON_UPDATE_COMMAND_UI(ID_COLOR_RED, &CSketcherDoc::OnUpdateColorRed)
	ON_UPDATE_COMMAND_UI(ID_COLOR_GREEN, &CSketcherDoc::OnUpdateColorGreen)
	ON_UPDATE_COMMAND_UI(ID_COLOR_BLUE, &CSketcherDoc::OnUpdateColorBlue)
	ON_UPDATE_COMMAND_UI(ID_ELEMENT_LINE, &CSketcherDoc::OnUpdateElementLine)
	ON_UPDATE_COMMAND_UI(ID_ELEMENT_RECTANGLE, &CSketcherDoc::OnUpdateElementRectangle)
	ON_UPDATE_COMMAND_UI(ID_ELEMENT_CIRCLE, &CSketcherDoc::OnUpdateElementCircle)
	ON_UPDATE_COMMAND_UI(ID_ELEMENT_CURVE, &CSketcherDoc::OnUpdateElementCurve)
	ON_COMMAND(ID_PEN_WIDTH, &CSketcherDoc::OnPenWidth)
	ON_COMMAND(ID_ELEMENT_TEXT, &CSketcherDoc::OnElementText)
	ON_UPDATE_COMMAND_UI(ID_ELEMENT_TEXT, &CSketcherDoc::OnUpdateElementText)
END_MESSAGE_MAP()


// CSketcherDoc construction/destruction

CSketcherDoc::CSketcherDoc()
	: m_Sketch(0)
{
	// TODO: add one-time construction code here

}

CSketcherDoc::~CSketcherDoc()
{
}

BOOL CSketcherDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CSketcherDoc serialization

void CSketcherDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << static_cast<COLORREF>(m_Color)
			<< static_cast<int>(m_Element)
			<< m_PenWidth
			<< m_DocSize;
		ar << m_Sketch.size();
		for (const auto& pElement : m_Sketch)
		{
			ar << pElement.get();
		}
	}
	else
	{
		// TODO: add loading code here
		COLORREF color{};
		int elementType{};
		ar >> color
			>> elementType
			>> m_PenWidth
			>> m_DocSize;
		m_Color = static_cast<ElementColor>(color);
		m_Element = static_cast<ElementType>(elementType);

		size_t elementCount{};
		ar >> elementCount;
		CElement* pElement;
		for (size_t i = 0; i < elementCount; i++)
		{
			ar >> pElement;
			m_Sketch.push_back(std::shared_ptr<CElement>(pElement));
		}
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CSketcherDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CSketcherDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CSketcherDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CSketcherDoc diagnostics

#ifdef _DEBUG
void CSketcherDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSketcherDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSketcherDoc commands


ElementType CSketcherDoc::GetElementType() const
{
	return m_Element;
}

ElementColor CSketcherDoc::GetElementColor() const
{
	return m_Color;
}

void CSketcherDoc::OnColorBlack()
{
	m_Color = ElementColor::BLACK;
	SetModifiedFlag();
}


void CSketcherDoc::OnColorRed()
{
	m_Color = ElementColor::RED;
	SetModifiedFlag();
}


void CSketcherDoc::OnColorGreen()
{
	m_Color = ElementColor::GREEN;
	SetModifiedFlag();
}


void CSketcherDoc::OnColorBlue()
{
	m_Color = ElementColor::BLUE;
	SetModifiedFlag();
}


void CSketcherDoc::OnElementLine()
{
	m_Element = ElementType::LINE;
	SetModifiedFlag();
}


void CSketcherDoc::OnElementRectangle()
{
	m_Element = ElementType::RECTANGLE;
	SetModifiedFlag();
}


void CSketcherDoc::OnElementCircle()
{
	m_Element = ElementType::CIRCLE;
	SetModifiedFlag();
}


void CSketcherDoc::OnElementCurve()
{
	m_Element = ElementType::CURVE;
	SetModifiedFlag();
}


void CSketcherDoc::OnUpdateColorBlack(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Color == ElementColor::BLACK);
}


void CSketcherDoc::OnUpdateColorRed(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Color == ElementColor::RED);
}


void CSketcherDoc::OnUpdateColorGreen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Color == ElementColor::GREEN);
}


void CSketcherDoc::OnUpdateColorBlue(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Color == ElementColor::BLUE);
}


void CSketcherDoc::OnUpdateElementLine(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Element == ElementType::LINE);
}


void CSketcherDoc::OnUpdateElementRectangle(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Element == ElementType::RECTANGLE);
}


void CSketcherDoc::OnUpdateElementCircle(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Element == ElementType::CIRCLE);
}


void CSketcherDoc::OnUpdateElementCurve(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Element == ElementType::CURVE);
}


void CSketcherDoc::AddElement(std::shared_ptr<CElement>& pElement)
{
	m_Sketch.push_back(pElement);
	UpdateAllViews(nullptr, 0, pElement.get());
	SetModifiedFlag();
}


void CSketcherDoc::DeleteElement(std::shared_ptr<CElement>& pElement)
{
	m_Sketch.remove(pElement);
	UpdateAllViews(nullptr, 0, pElement.get());
	SetModifiedFlag();
}

std::shared_ptr<CElement> CSketcherDoc::FindElement(const CPoint & point) const
{
	for (const auto& pElement : m_Sketch)
	{
		if (pElement->GetEnclosingRect().PtInRect(point))
		{
			return pElement;
		}
	}
	return nullptr;
}

void CSketcherDoc::SendToBack(std::shared_ptr<CElement>& pElement)
{
	if (pElement)
	{
		m_Sketch.remove(pElement);
		m_Sketch.push_back(pElement);
		SetModifiedFlag();
	}
}

CRect CSketcherDoc::GetDocExtent() const
{
	if (m_Sketch.empty())
	{
		return CRect{ 0,0,1,1 };
	}
	CRect docExtent{ m_Sketch.front()->GetEnclosingRect() };
	for (auto& pElement : m_Sketch)
	{
		docExtent.UnionRect(docExtent, pElement->GetEnclosingRect());
	}
	docExtent.NormalizeRect();
	return docExtent;
}


void CSketcherDoc::OnPenWidth()
{
	CPenDialog aDlg;
	aDlg.m_PenWidth = m_PenWidth;
	if (aDlg.DoModal() == IDOK)
	{
		m_PenWidth = aDlg.m_PenWidth;
		SetModifiedFlag();
	}
}


void CSketcherDoc::OnElementText()
{
	m_Element = ElementType::TEXT;
	SetModifiedFlag();
}


void CSketcherDoc::OnUpdateElementText(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Element == ElementType::TEXT);
}
