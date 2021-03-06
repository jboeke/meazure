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

/// @file
/// @brief Header file for the application class.

#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"


/// The application class. Instantiating this class starts the app.
///
class CMeazureApp : public CWinApp
{
public:
    /// Starts the application.
    ///
    CMeazureApp();

    /// Called by the framework to see if each modified doc wants
    /// to save before the app goes away.
    ///
    /// @return TRUE if safe to exit the application, FALSE otherwise.
    ///
    virtual BOOL SaveAllModified();

    /// Central interface to the windows help system. In this case
    /// we override the base class calls to the old WinHelp system and
    /// instead call the new Windows HTML Help system.
    ///
    /// @param dwData   [in] ID for context sensitive help.
    /// @param nCmd     [in] Type of help requested.
    ///
    virtual void WinHelp(DWORD dwData, UINT nCmd);

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMeazureApp)
    public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    //}}AFX_VIRTUAL

    /// @fn InitInstance()
    /// Performs all initialization for the application including
    /// processing of the command line arguments, if any.
    /// @return TRUE if initialization successful.

    /// @fn ExitInstance()
    /// Called when the application is terminating. Destroys all
    /// managers.
    /// @return Application exit code.

    //{{AFX_MSG(CMeazureApp)
    afx_msg void OnAppAbout();
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG

    /// @fn OnAppAbout()
    /// Displays the application About dialog.

    /// Called by the Mouse Hook DLL to route pointer movement
    /// messages to the View.
    ///
    /// @param wParam   [in] Message ID.
    /// @param lParam   [in] MOUSEHOOKSTRUCT
    ///
    /// @return Always returns 0.
    ///
    afx_msg void OnMouseHook(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
