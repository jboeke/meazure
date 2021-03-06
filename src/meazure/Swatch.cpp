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
#include "Swatch.h"
#include "MeaAssert.h"


BEGIN_MESSAGE_MAP(MeaSwatch, CWnd)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


MeaSwatch::MeaSwatch() : CWnd(),
    m_backBrush(NULL)
{
}


MeaSwatch::~MeaSwatch()
{
    try {
        if (m_backBrush != NULL) {
            delete m_backBrush;
        }
    }
    catch(...) {
        MeaAssert(false);
    }
}


void MeaSwatch::OnDestroy()
{
    CWnd::OnDestroy();
    DestroyColors();
}


bool MeaSwatch::Create(COLORREF color, const RECT &rect, CWnd *parent)
{
    SetColor(color);

    MeaAssert(m_backBrush != NULL);
    CString classname = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
        NULL, *m_backBrush);
    return CreateEx(0, classname, "", ((parent == NULL) ? WS_POPUP : WS_CHILD),
        rect, ((parent == NULL) ? AfxGetMainWnd() : parent), 0) ? true : false;
}


void MeaSwatch::DestroyColors()
{
    if (m_backBrush != NULL) {
        delete m_backBrush;
        m_backBrush = NULL;
    }
}


void MeaSwatch::SetColor(COLORREF color)
{
    CBrush *brush   = new CBrush(color);
    if (m_hWnd != NULL) {
        ::SetClassLong(m_hWnd, GCL_HBRBACKGROUND, reinterpret_cast<LONG>(static_cast<HBRUSH>(*brush)));
    }
    
    DestroyColors();
    m_backBrush = brush;

    if (m_hWnd != NULL) {
        Invalidate();
        UpdateWindow();
    }
}
