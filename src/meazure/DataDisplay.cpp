/*
 * Copyright 2001, 2004, 2011 C Thing Software
 *
 * This file is part of Meazure.
 * 
 * Meazure is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Meazure is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Meazure.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "StdAfx.h"
#include <stdio.h>
#include "MeaAssert.h"
#include "Resource.h"
#include "DataDisplay.h"
#include "Layout.h"
#include "ScreenMgr.h"
#include <AfxPriv.h>
#include <windowsx.h>


const RECT          MeaDataDisplay::kMargin                 = { 7, 17, 7, 7 };
const SIZE          MeaDataDisplay::kControlSpacing         = { 7, 5 };
const int           MeaDataDisplay::kSectionSpacing         = 3;
const int           MeaDataDisplay::kLabelSpacing           = 3;
const int           MeaDataDisplay::DataItem::kSpinWidth    = 17;
const unsigned int  MeaDataDisplay::kLengthChars            = 6;
const int           MeaDataDisplay::kAngleChars             = 6;
const int           MeaDataDisplay::kAreaChars              = 17;
const int           MeaDataDisplay::kResChars               = 6;


BEGIN_MESSAGE_MAP(MeaDataDisplay, CWnd)
    ON_MESSAGE(MeaFieldEnterMsg, OnFieldEntry)
    ON_MESSAGE(MeaFieldArrowMsg, OnFieldEntry)
    ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(MeaDataSection, CButton)
    ON_MESSAGE(MeaFieldEnterMsg, OnFieldEntry)
    ON_MESSAGE(MeaFieldArrowMsg, OnFieldEntry)
    ON_BN_CLICKED(IDC_MEA_CAL_BTN, OnCalButton)
END_MESSAGE_MAP()


MeaDataDisplay::MeaDataDisplay() : CWnd(),
        m_regionSectionProfileEnabled(true), m_screenSectionProfileEnabled(true),
        m_regionSectionEnabled(true), m_screenSectionEnabled(true)
{
    m_fields.insert(&m_x1.GetField());
    m_fields.insert(&m_y1.GetField());
    m_fields.insert(&m_x2.GetField());
    m_fields.insert(&m_y2.GetField());
    m_fields.insert(&m_xv.GetField());
    m_fields.insert(&m_yv.GetField());
    m_fields.insert(&m_width.GetField());
    m_fields.insert(&m_height.GetField());
    m_fields.insert(&m_length.GetField());
    m_fields.insert(&m_angle.GetField());
    m_fields.insert(&m_area.GetField());
    m_fields.insert(&m_screenWidth.GetField());
    m_fields.insert(&m_screenHeight.GetField());
    m_fields.insert(&m_screenResX.GetField());
    m_fields.insert(&m_screenResY.GetField());
}


MeaDataDisplay::~MeaDataDisplay()
{
}


bool MeaDataDisplay::Create(const POINT& topLeft, CWnd* parentWnd)
{
    if (!CWnd::CreateEx(WS_EX_CONTROLPARENT,
            AfxRegisterWndClass(NULL,0, GetSysColorBrush(COLOR_BTNFACE)), _T(""),
            WS_CHILD | WS_VISIBLE, CRect(topLeft, CSize(5, 5)), parentWnd, 0xFFFF)) {
        return false;
    }

    // Create the control sections
    //
    if (!CreateRegionSection()) {
        return false;
    }

    if (!CreateScreenSection()) {
        return false;
    }

    // Position the sections and size the overall window
    //
    m_regionSection.GetWindowRect(&m_regionSectionRect);
    m_regionSection.GetParent()->ScreenToClient(&m_regionSectionRect);

    m_screenSection.GetWindowRect(&m_screenSectionRect);
    m_screenSection.GetParent()->ScreenToClient(&m_screenSectionRect);

    if ((m_screenSectionRect.Width()) < m_regionSectionRect.Width()) {
        m_screenSectionRect.left = m_regionSectionRect.left;
        m_screenSectionRect.right = m_regionSectionRect.right;
    }

    MeaLayout::SetWindowSize(m_regionSection, m_regionSectionRect.Width(),
                                m_regionSectionRect.Height());

    MeaLayout::SetWindowPos(m_screenSection, 0,
                                m_regionSectionRect.Height() + kSectionSpacing);

    MeaLayout::SetWindowSize(m_screenSection, m_screenSectionRect.Width(),
                                m_screenSectionRect.Height());

    SIZE winSize;

    MeaLayout::GetBoundingSize(&winSize, this);
    MeaLayout::SetWindowSize(*this, winSize.cx, winSize.cy);

    return true;
}


bool MeaDataDisplay::CreateRegionSection()
{
    CPoint point(0, 0);
    MeaUnitsMgr& unitsMgr = MeaUnitsMgr::Instance();
    MeaScreenMgr& screenMgr = MeaScreenMgr::Instance();

    // Create the region box
    //
    if (!m_regionSection.CreateEx(WS_EX_CONTROLPARENT, _T("BUTTON"), "", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
           CRect(0, 0, 5, 5), this, 0xFFFF)){
        return false;
    }
    CFont *defaultFont = CFont::FromHandle(static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)));
    m_regionSection.SetFont(defaultFont, FALSE);

    // Create the coordinate controls
    //
    if (!m_x1.CreateField(IDS_MEA_X1, kLengthChars, &m_regionSection, MeaX1Field, MeaX1Field)){
        return false;
    }
    if (!m_y1.CreateField(IDS_MEA_Y1, kLengthChars, &m_regionSection, MeaY1Field, MeaY1Field)) {
        return false;
    }
    if (!m_x2.CreateField(IDS_MEA_X2, kLengthChars, &m_regionSection, MeaX2Field, MeaX2Field)) {
        return false;
    }
    if (!m_y2.CreateField(IDS_MEA_Y2, kLengthChars, &m_regionSection, MeaY2Field, MeaY2Field)) {
        return false;
    }
    if (!m_xv.CreateField(IDS_MEA_XV, kLengthChars, &m_regionSection, MeaXVField, MeaXVField)) {
        return false;
    }
    if (!m_yv.CreateField(IDS_MEA_YV, kLengthChars, &m_regionSection, MeaYVField, MeaYVField)) {
        return false;
    }

    m_x1.SetLimitText(kLengthChars);
    m_y1.SetLimitText(kLengthChars);
    m_x2.SetLimitText(kLengthChars);
    m_y2.SetLimitText(kLengthChars);
    m_xv.SetLimitText(kLengthChars);
    m_yv.SetLimitText(kLengthChars);

    m_x1.SetSpinRange(screenMgr.GetVirtualRect().left, screenMgr.GetVirtualRect().right - 1);
    m_y1.SetSpinRange(screenMgr.GetVirtualRect().top,  screenMgr.GetVirtualRect().bottom - 1);
    m_x2.SetSpinRange(screenMgr.GetVirtualRect().left, screenMgr.GetVirtualRect().right - 1);
    m_y2.SetSpinRange(screenMgr.GetVirtualRect().top,  screenMgr.GetVirtualRect().bottom - 1);
    m_xv.SetSpinRange(screenMgr.GetVirtualRect().left, screenMgr.GetVirtualRect().right - 1);
    m_yv.SetSpinRange(screenMgr.GetVirtualRect().top,  screenMgr.GetVirtualRect().bottom - 1);

    if (!m_y1.CreateLengthUnits(unitsMgr.GetLinearUnits(), &m_regionSection)) {
        return false;
    }
    if (!m_y2.CreateLengthUnits(unitsMgr.GetLinearUnits(), &m_regionSection)) {
        return false;
    }
    if (!m_yv.CreateLengthUnits(unitsMgr.GetLinearUnits(), &m_regionSection)) {
        return false;
    }

    // Create the dimension, angle and area controls
    //
    if (!m_width.CreateField(IDS_MEA_WIDTH, kLengthChars, &m_regionSection, MeaWidthField)) {
        return false;
    }
    if (!m_height.CreateField(IDS_MEA_HEIGHT, kLengthChars, &m_regionSection, MeaHeightField)) {
        return false;
    }
    if (!m_length.CreateField(IDS_MEA_LENGTH, kLengthChars, &m_regionSection, MeaDistanceField)) {
        return false;
    }
    if (!m_angle.CreateField(IDS_MEA_ANGLE, kAngleChars, &m_regionSection, MeaAngleField)) {
        return false;
    }
    if (!m_area.CreateField(IDS_MEA_AREA, kAreaChars, &m_regionSection, MeaAreaField)) {
        return false;
    }

    m_width.SetLimitText(kLengthChars);
    m_height.SetLimitText(kLengthChars);

    if (!m_height.CreateLengthUnits(unitsMgr.GetLinearUnits(), &m_regionSection)) {
        return false;
    }
    if (!m_length.CreateLengthUnits(unitsMgr.GetLinearUnits(), &m_regionSection)) {
        return false;
    }
    if (!m_angle.CreateAngleUnits(unitsMgr.GetAngularUnits(), &m_regionSection)) {
        return false;
    }
    if (!m_area.CreateAreaUnits(unitsMgr.GetLinearUnits(), &m_regionSection)) {
        return false;
    }

    // Position the controls and size the section
    //
    CRect rect;
    int lineHeight;
    SIZE sectionSize;

    m_x1.GetField().GetWindowRect(&rect);
    lineHeight = rect.Height() + kControlSpacing.cy;

    point.x = kMargin.left;
    point.y = kMargin.top;

    MeaLayout::AlignRight(point.x, &m_x1.GetTitleLabel(), &m_x2.GetTitleLabel(),
        &m_xv.GetTitleLabel(), &m_width.GetTitleLabel(),
        &m_length.GetTitleLabel(), &m_area.GetTitleLabel(), NULL);

    MeaLayout::PlaceAfter(m_x1.GetTitleLabel(), m_x1.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_x1.GetField(), m_x1.GetSpin(), 0);
    MeaLayout::PlaceAfter(m_x1.GetSpin(),  m_y1.GetTitleLabel(), kControlSpacing.cx);
    MeaLayout::PlaceAfter(m_y1.GetTitleLabel(), m_y1.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_y1.GetField(), m_y1.GetSpin(), 0);
    MeaLayout::PlaceAfter(m_y1.GetSpin(),  m_y1.GetUnitsLabel(), kLabelSpacing);

    MeaLayout::AlignCenter(point.y, &m_x1.GetTitleLabel(), &m_x1.GetField(), &m_x1.GetSpin(),
        &m_y1.GetTitleLabel(), &m_y1.GetField(), &m_y1.GetSpin(), &m_y1.GetUnitsLabel(), NULL);

    point.y += lineHeight;

    MeaLayout::PlaceAfter(m_x2.GetTitleLabel(), m_x2.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_x2.GetField(), m_x2.GetSpin(), 0);
    MeaLayout::PlaceAfter(m_x2.GetSpin(),  m_y2.GetTitleLabel(), kControlSpacing.cx);
    MeaLayout::PlaceAfter(m_y2.GetTitleLabel(), m_y2.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_y2.GetField(), m_y2.GetSpin(), 0);
    MeaLayout::PlaceAfter(m_y2.GetSpin(),  m_y2.GetUnitsLabel(), kLabelSpacing);

    MeaLayout::AlignCenter(point.y, &m_x2.GetTitleLabel(), &m_x2.GetField(), &m_x2.GetSpin(),
        &m_y2.GetTitleLabel(), &m_y2.GetField(), &m_y2.GetSpin(), &m_y2.GetUnitsLabel(), NULL);

    point.y += lineHeight;

    MeaLayout::PlaceAfter(m_xv.GetTitleLabel(), m_xv.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_xv.GetField(), m_xv.GetSpin(), 0);
    MeaLayout::PlaceAfter(m_xv.GetSpin(),  m_yv.GetTitleLabel(), kControlSpacing.cx);
    MeaLayout::PlaceAfter(m_yv.GetTitleLabel(), m_yv.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_yv.GetField(), m_yv.GetSpin(), 0);
    MeaLayout::PlaceAfter(m_yv.GetSpin(),  m_yv.GetUnitsLabel(), kLabelSpacing);

    MeaLayout::AlignCenter(point.y, &m_xv.GetTitleLabel(), &m_xv.GetField(), &m_xv.GetSpin(),
        &m_yv.GetTitleLabel(), &m_yv.GetField(), &m_yv.GetSpin(), &m_yv.GetUnitsLabel(), NULL);

    point.y += lineHeight;

    MeaLayout::PlaceAfter(m_width.GetTitleLabel(), m_width.GetField(), kLabelSpacing);

    MeaLayout::AlignRightTo(&m_yv.GetTitleLabel(), &m_y1.GetTitleLabel(),
        &m_y2.GetTitleLabel(), &m_height.GetTitleLabel(), NULL);
    MeaLayout::AlignRightTo(&m_yv.GetField(), &m_y1.GetField(), &m_y2.GetField(), NULL);

    MeaLayout::PlaceAfter(m_height.GetTitleLabel(), m_height.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_height.GetField(), m_height.GetUnitsLabel(), kLabelSpacing);

    MeaLayout::AlignCenter(point.y, &m_width.GetTitleLabel(), &m_width.GetField(),
        &m_height.GetTitleLabel(), &m_height.GetField(), &m_height.GetUnitsLabel(), NULL);

    point.y += lineHeight;

    MeaLayout::PlaceAfter(m_length.GetTitleLabel(), m_length.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_length.GetField(), m_length.GetUnitsLabel(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_length.GetUnitsLabel(), m_angle.GetTitleLabel(), kControlSpacing.cx);
    MeaLayout::PlaceAfter(m_angle.GetTitleLabel(), m_angle.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_angle.GetField(), m_angle.GetUnitsLabel(), kLabelSpacing);

    MeaLayout::AlignCenter(point.y, &m_length.GetTitleLabel(), &m_length.GetField(),
        &m_length.GetUnitsLabel(), &m_angle.GetTitleLabel(), &m_angle.GetField(),
        &m_angle.GetUnitsLabel(), NULL);

    point.y += lineHeight;

    MeaLayout::PlaceAfter(m_area.GetTitleLabel(), m_area.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_area.GetField(), m_area.GetUnitsLabel(), kLabelSpacing);

    MeaLayout::AlignCenter(point.y, &m_area.GetTitleLabel(), &m_area.GetField(),
        &m_area.GetUnitsLabel(), NULL);

    MeaLayout::GetBoundingSize(&sectionSize, &m_regionSection);

    sectionSize.cx += kMargin.left + kMargin.right;
    sectionSize.cy += kMargin.top + kMargin.bottom;

    MeaLayout::SetWindowSize(m_regionSection, sectionSize.cx, sectionSize.cy);

    return true;
}


bool MeaDataDisplay::CreateScreenSection()
{
    CPoint point(0, 0);
    MeaUnitsMgr& unitsMgr = MeaUnitsMgr::Instance();

    // Create the region box
    //
    if (!m_screenSection.CreateEx(WS_EX_CONTROLPARENT, _T("BUTTON"), _T(""), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
           CRect(0, 0, 5, 5), this, 0xFFFF)) {
        return false;
    }
    CFont *defaultFont = CFont::FromHandle(static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)));
    m_screenSection.SetFont(defaultFont, FALSE);

    // Create the controls
    //
    if (!m_screenWidth.CreateField(IDS_MEA_WIDTH, kLengthChars, &m_screenSection, MeaScreenWidthField)) {
        return false;
    }
    if (!m_screenHeight.CreateField(IDS_MEA_HEIGHT, kLengthChars, &m_screenSection, MeaScreenHeightField)) {
        return false;
    }
    if (!m_screenResX.CreateField(IDS_MEA_RESOLUTION_X, kResChars, &m_screenSection, MeaResXField)) {
        return false;
    }
    if (!m_screenResY.CreateField(IDS_MEA_RESOLUTION_Y, kResChars, &m_screenSection, MeaResYField)) {
        return false;
    }

    if (!m_screenHeight.CreateLengthUnits(unitsMgr.GetLinearUnits(), &m_screenSection)) {
        return false;
    }
    if (!m_screenResY.CreateResUnits(unitsMgr.GetLinearUnits(), &m_screenSection)) {
        return false;
    }

    // Position the controls and size the section
    //
    CRect rect;
    int lineHeight;
    SIZE sectionSize;

    m_screenWidth.GetField().GetWindowRect(&rect);
    lineHeight = rect.Height() + kControlSpacing.cy;

    point.x = kMargin.left;
    point.y = kMargin.top;

    MeaLayout::AlignRight(point.x, &m_screenWidth.GetTitleLabel(), &m_screenResX.GetTitleLabel(), NULL);

    MeaLayout::PlaceAfter(m_screenResX.GetTitleLabel(), m_screenResX.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_screenResX.GetField(), m_screenResY.GetTitleLabel(), kControlSpacing.cx);
    MeaLayout::PlaceAfter(m_screenResY.GetTitleLabel(), m_screenResY.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_screenResY.GetField(), m_screenResY.GetUnitsLabel(), kLabelSpacing);

    MeaLayout::AlignRightTo(&m_screenResY.GetTitleLabel(), &m_screenHeight.GetTitleLabel(), NULL);

    MeaLayout::PlaceAfter(m_screenWidth.GetTitleLabel(), m_screenWidth.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_screenHeight.GetTitleLabel(), m_screenHeight.GetField(), kLabelSpacing);
    MeaLayout::PlaceAfter(m_screenHeight.GetField(), m_screenHeight.GetUnitsLabel(), kLabelSpacing);

    MeaLayout::AlignCenter(point.y, &m_screenWidth.GetTitleLabel(), &m_screenWidth.GetField(),
        &m_screenHeight.GetTitleLabel(), &m_screenHeight.GetField(), &m_screenHeight.GetUnitsLabel(), NULL);

    point.y += lineHeight;

    MeaLayout::AlignCenter(point.y, &m_screenResX.GetTitleLabel(), &m_screenResX.GetField(),
        &m_screenResY.GetTitleLabel(), &m_screenResY.GetField(), &m_screenResY.GetUnitsLabel(), NULL);

    MeaLayout::GetBoundingSize(&sectionSize, &m_screenSection);

    sectionSize.cx += kMargin.left + kMargin.right;
    sectionSize.cy += kMargin.top + kMargin.bottom;

    MeaLayout::SetWindowSize(m_screenSection, sectionSize.cx, sectionSize.cy);

    // Calibration warning button
    //
    CRect btnRect;
    m_calBtn.LoadBitmaps(IDB_MEA_WARNING, IDB_MEA_WARNING);
    m_calBtn.Create(WS_VISIBLE, &m_screenSection, IDC_MEA_CAL_BTN, IDS_MEA_CAL_WARNING);
    m_calBtn.GetWindowRect(&btnRect);
    MeaLayout::SetWindowPos(m_calBtn, sectionSize.cx - btnRect.Width(),
                            kMargin.top - kControlSpacing.cx);

    return true;
}


void MeaDataDisplay::Enable(MeaSection section)
{
    if (section == MeaRegionSection) {
        if (!m_regionSectionEnabled) {
            m_regionSectionEnabled = true;
            MeaLayout::MoveWindowHeight(m_screenSection, m_regionSectionRect.Height());
            m_regionSection.ShowWindow(SW_SHOW);
        }
    } else {
        if (!m_screenSectionEnabled) {
            m_screenSectionEnabled = true;
            m_screenSection.ShowWindow(SW_SHOW);
        }
    }
}


void MeaDataDisplay::Disable(MeaSection section)
{
    if (section == MeaRegionSection) {
        if (m_regionSectionEnabled) {
            m_regionSectionEnabled = false;
            m_regionSection.ShowWindow(SW_HIDE);
            MeaLayout::MoveWindowHeight(m_screenSection, -m_regionSectionRect.Height());
        }
    } else {
        if (m_screenSectionEnabled) {
            m_screenSectionEnabled = false;
            m_screenSection.ShowWindow(SW_HIDE);
        }
    }
}


void MeaDataDisplay::ShowXY1(const POINT& point, const FPOINT& cpoint)
{
    MeaUnitsMgr& unitsMgr = MeaUnitsMgr::Instance();

    m_x1.SetSpinPos(point.x);
    m_y1.SetSpinPos(point.y);

    m_x1.SetText(unitsMgr.Format(MeaX, cpoint.x));
    m_y1.SetText(unitsMgr.Format(MeaY, cpoint.y));
}


void MeaDataDisplay::ShowXY2(const POINT& point, const FPOINT& cpoint)
{
    MeaUnitsMgr& unitsMgr = MeaUnitsMgr::Instance();

    m_x2.SetSpinPos(point.x);
    m_y2.SetSpinPos(point.y);

    m_x2.SetText(unitsMgr.Format(MeaX, cpoint.x));
    m_y2.SetText(unitsMgr.Format(MeaY, cpoint.y));
}


void MeaDataDisplay::ShowXYV(const POINT& point, const FPOINT& cpoint)
{
    MeaUnitsMgr& unitsMgr = MeaUnitsMgr::Instance();

    m_xv.SetSpinPos(point.x);
    m_yv.SetSpinPos(point.y);

    m_xv.SetText(unitsMgr.Format(MeaX, cpoint.x));
    m_yv.SetText(unitsMgr.Format(MeaY, cpoint.y));
}


void MeaDataDisplay::ShowWH(const FSIZE& size)
{
    MeaUnitsMgr& unitsMgr = MeaUnitsMgr::Instance();

    m_width.SetText(unitsMgr.Format(MeaW, size.cx));
    m_height.SetText(unitsMgr.Format(MeaH, size.cy));
}


void MeaDataDisplay::ShowDistance(const FSIZE& size)
{
    m_length.SetText(MeaUnitsMgr::Instance().Format(MeaD, MeaLayout::CalcLength(size.cx, size.cy)));
}


void MeaDataDisplay::ShowDistance(double dist)
{
    m_length.SetText(MeaUnitsMgr::Instance().Format(MeaD, dist));
}


void MeaDataDisplay::ShowAngle(double angle)
{
    m_angle.SetText(MeaUnitsMgr::Instance().FormatConvertAngle(angle));
}


void MeaDataDisplay::ShowRectArea(const FSIZE& size)
{
    m_area.SetText(MeaUnitsMgr::Instance().Format(MeaAr, size.cx * size.cy));
}


void MeaDataDisplay::ShowCircleArea(double radius)
{
    m_area.SetText(MeaUnitsMgr::Instance().Format(MeaAr, MeaUnits::kPI * radius * radius));
}


void MeaDataDisplay::ShowScreenName(const CString& name)
{
    m_screenSection.SetWindowText(name);
}


void MeaDataDisplay::ShowScreenWH(const FSIZE& size)
{
    MeaUnitsMgr& units = MeaUnitsMgr::Instance();

    m_screenWidth.SetText(units.Format(MeaW, size.cx));
    m_screenHeight.SetText(units.Format(MeaH, size.cy));
}


void MeaDataDisplay::ShowScreenRes(const FSIZE& res)
{
    MeaUnitsMgr& units = MeaUnitsMgr::Instance();

    m_screenResX.SetText(units.Format(MeaRx, res.cx));
    m_screenResY.SetText(units.Format(MeaRy, res.cy));
}


void MeaDataDisplay::EnableRegionFields(UINT enableFields, UINT editableFields)
{
    m_x1.SetReadOnly(!(editableFields & MeaX1Field));
    m_y1.SetReadOnly(!(editableFields & MeaY1Field));
    m_x2.SetReadOnly(!(editableFields & MeaX2Field));
    m_y2.SetReadOnly(!(editableFields & MeaY2Field));
    m_xv.SetReadOnly(!(editableFields & MeaXVField));
    m_yv.SetReadOnly(!(editableFields & MeaYVField));

    m_width.SetReadOnly(!(editableFields & MeaWidthField));
    m_height.SetReadOnly(!(editableFields & MeaHeightField));
    m_length.SetReadOnly(!(editableFields & MeaDistanceField));
    m_angle.SetReadOnly(!(editableFields & MeaAngleField));
    m_area.SetReadOnly(!(editableFields & MeaAreaField));

    m_x1.Enable((enableFields & MeaX1Field) != 0);
    m_y1.Enable((enableFields & MeaY1Field) != 0);
    m_x2.Enable((enableFields & MeaX2Field) != 0);
    m_y2.Enable((enableFields & MeaY2Field) != 0);
    m_xv.Enable((enableFields & MeaXVField) != 0);
    m_yv.Enable((enableFields & MeaYVField) != 0);

    m_width.Enable((enableFields & MeaWidthField) != 0);
    m_height.Enable((enableFields & MeaHeightField) != 0);
    m_length.Enable((enableFields & MeaDistanceField) != 0);
    m_angle.Enable((enableFields & MeaAngleField) != 0);
    m_area.Enable((enableFields & MeaAreaField) != 0);
}


void MeaDataDisplay::EnableScreenFields(UINT enableFields, UINT editableFields)
{
    m_screenWidth.SetReadOnly(!(editableFields & MeaScreenWidthField));
    m_screenHeight.SetReadOnly(!(editableFields & MeaScreenHeightField));
    m_screenResX.SetReadOnly(!(editableFields & MeaResXField));
    m_screenResY.SetReadOnly(!(editableFields & MeaResYField));

    m_screenWidth.Enable((enableFields & MeaScreenWidthField) != 0);
    m_screenHeight.Enable((enableFields & MeaScreenHeightField) != 0);
    m_screenResX.Enable((enableFields & MeaResXField) != 0);
    m_screenResY.Enable((enableFields & MeaResYField) != 0);
}


void MeaDataDisplay::ClearRegionData()
{
    m_x1.SetText(_T(""));
    m_y1.SetText(_T(""));
    m_x2.SetText(_T(""));
    m_y2.SetText(_T(""));
    m_xv.SetText(_T(""));
    m_yv.SetText(_T(""));
    m_width.SetText(_T(""));
    m_height.SetText(_T(""));
    m_length.SetText(_T(""));
    m_angle.SetText(_T(""));
    m_area.SetText(_T(""));
}


MeaNumberField* MeaDataDisplay::GetFieldFocus() const
{
    CWnd *focusWnd = GetFocus();

    if (focusWnd == NULL) {
        return NULL;
    }

    Fields::const_iterator iter = m_fields.find(static_cast<MeaNumberField*>(focusWnd));
    return (iter == m_fields.end()) ? NULL : (*iter);
}


LRESULT MeaDataSection::OnFieldEntry(WPARAM wParam, LPARAM id)
{
    GetParent()->SendMessage(MeaFieldEnterMsg, wParam, id);
    return TRUE;
}


LRESULT MeaDataDisplay::OnFieldEntry(WPARAM wParam, LPARAM id)
{
    FPOINT pos;
    int pixels = 0;
    const MeaNumberField *field = NULL;
    MeaUnitsMgr& unitsMgr = MeaUnitsMgr::Instance();

    switch (id) {
    case MeaX1Field:
        field = &m_x1.GetField();
        pos.x = m_x1.GetValue();
        pos.y = m_y1.GetValue();
        pixels = unitsMgr.UnconvertCoord(pos).x;
        break;
    case MeaY1Field:
        field = &m_y1.GetField();
        pos.x = m_x1.GetValue();
        pos.y = m_y1.GetValue();
        pixels = unitsMgr.UnconvertCoord(pos).y;
        break;
    case MeaX2Field:
        field = &m_x2.GetField();
        pos.x = m_x2.GetValue();
        pos.y = m_y2.GetValue();
        pixels = unitsMgr.UnconvertCoord(pos).x;
        break;
    case MeaY2Field:
        field = &m_y2.GetField();
        pos.x = m_x2.GetValue();
        pos.y = m_y2.GetValue();
        pixels = unitsMgr.UnconvertCoord(pos).y;
        break;
    case MeaXVField:
        field = &m_xv.GetField();
        pos.x = m_xv.GetValue();
        pos.y = m_yv.GetValue();
        pixels = unitsMgr.UnconvertCoord(pos).x;
        break;
    case MeaYVField:
        field = &m_yv.GetField();
        pos.x = m_xv.GetValue();
        pos.y = m_yv.GetValue();
        pixels = unitsMgr.UnconvertCoord(pos).y;
        break;
    default:
        MeaAssert(false);
        break;
    }

    GetParent()->SendMessage(MeaDataChangeMsg, pixels + static_cast<int>(wParam), id);
    MeaAssert(field != NULL);
    const_cast<MeaNumberField*>(field)->SetFocus();

    return TRUE;
}


LRESULT MeaDataDisplay::OnHelpHitTest(WPARAM, LPARAM lparam)
{
    CPoint point(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
    if (m_regionSectionRect.PtInRect(point)) {
        return HID_BASE_COMMAND + ID_HELP_TOOL_INFO;
    }
    return HID_BASE_COMMAND + ID_HELP_SCREEN_INFO;
}


void MeaDataSection::OnCalButton()
{
    AfxGetMainWnd()->PostMessage(MeaShowCalPrefsMsg);
}


BOOL MeaDataSection::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
    *pResult = TRUE;

    return TRUE;
}


BOOL MeaDataDisplay::OnNotify(WPARAM /* wParam */, LPARAM lParam, LRESULT* pResult)
{
    NMHDR *hdr = reinterpret_cast<NMHDR*>(lParam);

    if (hdr->code == UDN_DELTAPOS) {
        NMUPDOWN *ud = reinterpret_cast<NMUPDOWN*>(lParam);
        int pixelPos = ud->iPos + ud->iDelta;

        GetParent()->SendMessage(MeaDataChangeMsg, pixelPos, hdr->idFrom);

        *pResult = TRUE;
    } else {
        *pResult = FALSE;
    }

    return *pResult;
}


bool MeaDataDisplay::DataItem::CreateField(UINT labelId, int numChars,
                                           CWnd *parentWnd, MeaFields fieldId,
                                           int spinId)
{
    if (!m_titleLabel.Create(labelId, WS_CHILD | WS_VISIBLE, CPoint(0, 0), parentWnd)) {
        return false;
    }
    if (!m_field.Create(WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | ES_RIGHT | ES_AUTOHSCROLL,
           CPoint(0, 0), numChars, parentWnd, fieldId)) {
        return false;
    }
    if (spinId != 0xFFFF) {
        if (!CreateSpinButton(parentWnd, static_cast<MeaFields>(spinId))) {
            return false;
        }
    }
    return true;
}


bool MeaDataDisplay::DataItem::CreateSpinButton(CWnd *parentWnd, MeaFields id)
{
    CRect rect;

    m_field.GetWindowRect(&rect);
    m_spin = new CSpinButtonCtrl();
    bool ret = m_spin->Create(WS_CHILD | WS_VISIBLE,
        CRect(0, rect.top, 0, rect.bottom), parentWnd, id) ? true : false;
    MeaLayout::SetWindowSize(*m_spin, kSpinWidth, rect.Height());

    UDACCEL uda;
    uda.nSec = 0;
    uda.nInc = 1;
    m_spin->SetAccel(1, &uda);

    return ret;
}


void MeaDataDisplay::DataItem::Enable(bool enable)
{
    m_titleLabel.EnableWindow(enable);
    m_field.EnableWindow(enable);
    if (m_spin != NULL) {
        m_spin->EnableWindow(enable);
    }
    if (m_unitsLabel != NULL) {
        m_unitsLabel->EnableWindow(enable);
    }
}

