
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
#include "PrintData.h"

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
	CPrintData* printData{ new CPrintData };
	CSketcherDoc* pDoc{ GetDocument() };
	CRect docExtent{ pDoc->GetDocExtent() };
	printData->m_DocRefPoint = docExtent.TopLeft();
	printData->m_DocTitle = pDoc->GetTitle();
	// Calculate how many printed page widths are required
	// to accommodate the width of the document
	printData->m_nWidths = static_cast<UINT>(ceil(
		static_cast<double>(docExtent.Width()) / printData->printWidth));
	// Calculate how many printed page lengths are required
	// to accommodate the document length
	printData->m_nLengths = static_cast<UINT>(
		ceil(static_cast<double>(docExtent.Height()) / printData->printLength));
	// Set the first page number as 1 and
	// set the last page number as the total number of pages
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(printData->m_nWidths*printData->m_nLengths);
	pInfo->m_lpUserData = printData; // Store address of PrintData object
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSketcherView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSketcherView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{
	// TODO: add cleanup after printing
	delete static_cast<CPrintData*>(pInfo->m_lpUserData);
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
		pDoc->SetModifiedFlag();
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
	int scale{ pDC->IsPrinting() ? 1 : m_Scale }; // If we are printing, use scale 1
	int xExtent{ (DocSize.cx*scale*xLogPixels) / 100 };
	int yExtent{ (DocSize.cy*scale*yLogPixels) / 100 };
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


void CSketcherView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	//CScrollView::OnPrint(pDC, pInfo);
	CPrintData* p{ static_cast<CPrintData*>(pInfo->m_lpUserData) };
	// Output the document filename
	pDC->SetTextAlign(TA_CENTER); // Center the following text
	pDC->TextOut(pInfo->m_rectDraw.right / 2, 20, p->m_DocTitle);
	CString str;
	str.Format(_T("Page %u"), pInfo->m_nCurPage);
	pDC->TextOut(pInfo->m_rectDraw.right / 2, pInfo->m_rectDraw.bottom - 20, str);
	pDC->SetTextAlign(TA_LEFT); // Left justify text
								// Calculate the origin point for the current page
	int xOrg{ static_cast<int>(p->m_DocRefPoint.x +
		p->printWidth*((pInfo->m_nCurPage - 1) % (p->m_nWidths))) };
	int yOrg{ static_cast<int>(p->m_DocRefPoint.y +
		p->printLength*((pInfo->m_nCurPage - 1) / (p->m_nWidths))) };
	// Calculate offsets to center drawing area on page as positive values
	int xOffset{ static_cast<int>((pInfo->m_rectDraw.right - p->printWidth) / 2) };
	int yOffset{ static_cast<int>((pInfo->m_rectDraw.bottom - p->printLength) / 2) };
	// Change window origin to correspond to current page & save old origin
	CPoint OldOrg{ pDC->SetWindowOrg(xOrg - xOffset, yOrg - yOffset) };
	// Define a clip rectangle the size of the printed area
	pDC->IntersectClipRect(xOrg, yOrg, xOrg + p->printWidth, yOrg + p->printLength);
	OnDraw(pDC); // Draw the whole document
	pDC->SelectClipRgn(nullptr); // Remove the clip rectangle
	pDC->SetWindowOrg(OldOrg); // Restore old window origin
}
