
// SketcherView.cpp : implementation of the CSketcherView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Sketcher.h"
#endif

#include "SketcherDoc.h"
#include "SketcherView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Curve.h"
#include "Line.h"
#include "Rectangle.h"
#include "Circle.h"

// CSketcherView

IMPLEMENT_DYNCREATE(CSketcherView, CView)

BEGIN_MESSAGE_MAP(CSketcherView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CSketcherView construction/destruction

CSketcherView::CSketcherView()
	: m_FirstPoint(0)
{
	// TODO: add construction code here

}

CSketcherView::~CSketcherView()
{
}

BOOL CSketcherView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSketcherView drawing

void CSketcherView::OnDraw(CDC* pDC)
{
	CSketcherDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	for (const auto& pElement : *pDoc)
	{
		if (pDC->RectVisible(pElement->GetEnclosingRect()))
		{
			pElement->Draw(pDC);
		}
	}
}


// CSketcherView printing

BOOL CSketcherView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSketcherView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSketcherView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CSketcherView diagnostics

#ifdef _DEBUG
void CSketcherView::AssertValid() const
{
	CView::AssertValid();
}

void CSketcherView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSketcherDoc* CSketcherView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSketcherDoc)));
	return (CSketcherDoc*)m_pDocument;
}
#endif //_DEBUG


// CSketcherView message handlers


void CSketcherView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_FirstPoint = point;
	SetCapture();
}


void CSketcherView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (this == GetCapture())
	{
		ReleaseCapture();
	}
	if (m_pTempElement)
	{
		GetDocument()->AddElement(m_pTempElement);
		InvalidateRect(&m_pTempElement->GetEnclosingRect());
		m_pTempElement.reset();
	}
}


void CSketcherView::OnMouseMove(UINT nFlags, CPoint point)
{
	CClientDC aDC{ this };
	if ((nFlags & MK_LBUTTON) && (this == GetCapture()))
	{
		m_SecondPoint = point;
		if (ElementType::CURVE == GetDocument()->GetElementType())
		{
			std::dynamic_pointer_cast<CCurve>(m_pTempElement)->AddSegment(m_SecondPoint);
			m_pTempElement->Draw(&aDC);
		}
		else
		{
			aDC.SetROP2(R2_NOTXORPEN);
			m_pTempElement->Draw(&aDC);
		}
	}
	m_pTempElement = CreateElement();
	m_pTempElement->Draw(&aDC);
}

std::shared_ptr<CElement> CSketcherView::CreateElement() const
{
	CSketcherDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	COLORREF color{ static_cast<COLORREF>(pDoc->GetElementColor()) };
	switch (pDoc->GetElementType())
	{
	case ElementType::RECTANGLE:
		return std::make_shared<CRectangle>(m_FirstPoint, m_SecondPoint, color);
	case ElementType::CIRCLE:
		return std::make_shared<CCircle>(m_FirstPoint, m_SecondPoint, color);
	case ElementType::CURVE:
		return std::make_shared<CCurve>(m_FirstPoint, m_SecondPoint, color);
	case ElementType::LINE:
		return std::make_shared<CLine>(m_FirstPoint, m_SecondPoint, color);
	default:
		AfxMessageBox(_T("Bad Element code"), MB_OK);
		AfxAbort();
		return nullptr;
	}
}
