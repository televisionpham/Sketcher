
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
#include "ScaleDialog.h"
#include "Text.h"
#include "TextDialog.h"

// CSketcherView

IMPLEMENT_DYNCREATE(CSketcherView, CScrollView)

BEGIN_MESSAGE_MAP(CSketcherView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ELEMENT_DELETE, &CSketcherView::OnElementDelete)
	ON_COMMAND(ID_ELEMENT_MOVE, &CSketcherView::OnElementMove)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_ELEMENT_SENDTOBACK, &CSketcherView::OnElementSendtoback)
	ON_COMMAND(ID_VIEW_SCALE, &CSketcherView::OnViewScale)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCALE, &CSketcherView::OnUpdateScale)
END_MESSAGE_MAP()

// CSketcherView construction/destruction

CSketcherView::CSketcherView()
	: m_FirstPoint(0)
{
	// TODO: add construction code here
	SetScrollSizes(MM_TEXT, CSize{});
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
			pElement->Draw(pDC, m_pSelected);					
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
	CClientDC aDC{ this };
	OnPrepareDC(&aDC);
	aDC.DPtoLP(&point);
	CSketcherDoc* pDoc{ GetDocument() };

	if (m_MoveMode)
	{
		m_MoveMode = false;
		auto pElement{ m_pSelected };
		m_pSelected.reset();
		pDoc->UpdateAllViews(nullptr, 0, pElement.get());
	}
	else if (pDoc->GetElementType() == ElementType::TEXT)
	{
		CTextDialog aDlg;
		if (aDlg.DoModal() == IDOK)
		{
			CSize textExtent{ aDC.GetOutputTextExtent(aDlg.m_TextString) };
			textExtent.cx *= m_Scale;
			textExtent.cy *= m_Scale;
			std::shared_ptr<CElement> pTextElement
			{
				std::make_shared<CText>(
					point,
					point + textExtent,
					aDlg.m_TextString,
					static_cast<COLORREF>(pDoc->GetElementColor()))
			};
			pDoc->AddElement(pTextElement);			
			pDoc->UpdateAllViews(nullptr, 0, pTextElement.get());
		}
	}
	else
	{
		m_FirstPoint = point;
		SetCapture();
	}	
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
		CRect aRect{ m_pTempElement->GetEnclosingRect() };
		CClientDC aDC{ this };
		OnPrepareDC(&aDC);
		aDC.LPtoDP(aRect);
		InvalidateRect(aRect);
		m_pTempElement.reset();
	}	
}


void CSketcherView::OnMouseMove(UINT nFlags, CPoint point)
{
	CClientDC aDC{ this };
	OnPrepareDC(&aDC);
	aDC.DPtoLP(&point);
	if (m_MoveMode)
	{
		MoveElement(aDC, point);
	}
	else if ((nFlags & MK_LBUTTON) && (this == GetCapture()))
	{
		m_SecondPoint = point;
		if (m_pTempElement)
		{
			if (ElementType::CURVE == GetDocument()->GetElementType())
			{
				std::dynamic_pointer_cast<CCurve>(m_pTempElement)->AddSegment(m_SecondPoint);
				m_pTempElement->Draw(&aDC);
				return;
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
	else
	{
		auto pOldSelected = m_pSelected;
		m_pSelected = GetDocument()->FindElement(point);
		if (m_pSelected != pOldSelected)
		{
			if (m_pSelected)
			{
				GetDocument()->UpdateAllViews(nullptr, 0, m_pSelected.get());
			}
			if (pOldSelected)
			{
				GetDocument()->UpdateAllViews(nullptr, 0, pOldSelected.get());
			}
		}
	}
}

std::shared_ptr<CElement> CSketcherView::CreateElement() const
{
	CSketcherDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	COLORREF color{ static_cast<COLORREF>(pDoc->GetElementColor()) };
	int penWidth{ pDoc->GetPenWidth() };
	switch (pDoc->GetElementType())
	{
	case ElementType::RECTANGLE:
		return std::make_shared<CRectangle>(m_FirstPoint, m_SecondPoint, color, penWidth);
	case ElementType::CIRCLE:
		return std::make_shared<CCircle>(m_FirstPoint, m_SecondPoint, color, penWidth);
	case ElementType::CURVE:
		return std::make_shared<CCurve>(m_FirstPoint, m_SecondPoint, color, penWidth);
	case ElementType::LINE:
		return std::make_shared<CLine>(m_FirstPoint, m_SecondPoint, color, penWidth);
	default:
		AfxMessageBox(_T("Bad Element code"), MB_OK);
		AfxAbort();
		return nullptr;
	}
}

void CSketcherView::MoveElement(CClientDC & aDC, const CPoint & point)
{
	CSize distance{ point - m_CursorPos };
	m_CursorPos = point;
	if (m_pSelected)
	{
		auto pDoc{ GetDocument() };
		pDoc->UpdateAllViews(nullptr, 0, m_pSelected.get());				
		if (typeid(*(m_pSelected.get())) == typeid(CText))
		{
			CRect oldRect{ m_pSelected->GetEnclosingRect() };
			aDC.LPtoDP(oldRect);
			m_pSelected->Move(distance);
			InvalidateRect(&oldRect);
			UpdateWindow();
			m_pSelected->Draw(&aDC, m_pSelected);
		}
		else
		{
			aDC.SetROP2(R2_NOTXORPEN);
			m_pSelected->Draw(&aDC, m_pSelected);
			m_pSelected->Move(distance);
			m_pSelected->Draw(&aDC, m_pSelected);
		}
		pDoc->UpdateAllViews(nullptr, 0, m_pSelected.get());
	}
}


void CSketcherView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (pHint)
	{
		CClientDC aDC{ this };
		OnPrepareDC(&aDC);
		CRect aRect{ dynamic_cast<CElement*>(pHint)->GetEnclosingRect() };
		aDC.LPtoDP(aRect);
		InvalidateRect(aRect);
	}
	else
	{
		InvalidateRect(nullptr);
	}
}


void CSketcherView::OnInitialUpdate()
{	
	ResetScrollSizes();
	CScrollView::OnInitialUpdate();
}


void CSketcherView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	menu.LoadMenuW(IDR_CONTEXT_MENU);
	CMenu* pContext{ };
	if (m_pSelected)
	{
		pContext = menu.GetSubMenu(0);
	}
	else
	{
		pContext = menu.GetSubMenu(1);
		ElementColor color{ GetDocument()->GetElementColor() };
		menu.CheckMenuItem(ID_COLOR_BLACK,
			(color == ElementColor::BLACK ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_COLOR_RED,
			(color == ElementColor::RED ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_COLOR_GREEN,
			(color == ElementColor::GREEN ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_COLOR_BLUE,
			(color == ElementColor::BLUE ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		ElementType type{ GetDocument()->GetElementType() };
		menu.CheckMenuItem(ID_ELEMENT_LINE,
			(type == ElementType::LINE ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_ELEMENT_RECTANGLE,
			(type == ElementType::RECTANGLE ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_ELEMENT_CIRCLE,
			(type == ElementType::CIRCLE ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_ELEMENT_CURVE,
			(type == ElementType::CURVE ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
	}
	ASSERT(pContext != nullptr);
	pContext->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}


void CSketcherView::OnElementDelete()
{
	if (m_pSelected)
	{
		GetDocument()->DeleteElement(m_pSelected);
		m_pSelected.reset();
	}
}


void CSketcherView::OnElementMove()
{
	CClientDC aDC{ this };
	OnPrepareDC(&aDC);
	GetCursorPos(&m_CursorPos);
	ScreenToClient(&m_CursorPos);
	aDC.DPtoLP(&m_CursorPos);
	m_FirstPos = m_CursorPos;
	m_MoveMode = true;
}


void CSketcherView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_MoveMode)
	{
		CClientDC aDC{ this };
		OnPrepareDC(&aDC);		
		MoveElement(aDC, m_FirstPos);
		m_pSelected.reset();
		GetDocument()->UpdateAllViews(nullptr);
	}
}


void CSketcherView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_MoveMode)
	{
		m_MoveMode = false;
	}
	else
	{
		CScrollView::OnRButtonUp(nFlags, point);
	}
}


void CSketcherView::OnElementSendtoback()
{
	GetDocument()->SendToBack(m_pSelected);
}


void CSketcherView::OnViewScale()
{
	CScaleDialog aDlg;
	aDlg.m_Scale = m_Scale;
	if (aDlg.DoModal() == IDOK)
	{
		m_Scale = aDlg.m_Scale;
		ResetScrollSizes();
		InvalidateRect(nullptr);
	}
}


void CSketcherView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CScrollView::OnPrepareDC(pDC, pInfo);
	CSketcherDoc* pDoc{ GetDocument() };
	pDC->SetMapMode(MM_ANISOTROPIC);
	CSize DocSize{ pDoc->GetDocSize() };
	pDC->SetWindowExt(DocSize);
	// Get the number of pixels per inch in x and y
	int xLogPixels{ pDC->GetDeviceCaps(LOGPIXELSX) };
	int yLogPixels{ pDC->GetDeviceCaps(LOGPIXELSY) };

	// Calculate the viewport extent in x and y for the current scale
	int xExtent{ (DocSize.cx*m_Scale*xLogPixels) / 100 };
	int yExtent{ (DocSize.cy*m_Scale*yLogPixels) / 100 };
	pDC->SetViewportExt(xExtent, yExtent); // Set viewport extent
}


void CSketcherView::ResetScrollSizes()
{
	CClientDC aDC{ this };
	OnPrepareDC(&aDC);
	CSize DocSize{ GetDocument()->GetDocSize() };
	aDC.LPtoDP(&DocSize);
	SetScrollSizes(MM_TEXT, DocSize);
}


void CSketcherView::OnUpdateScale(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString scaleStr;
	scaleStr.Format(_T(" View Scale : %d"), m_Scale);
	pCmdUI->SetText(scaleStr);
}
