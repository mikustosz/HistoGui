/**
 @file      MyDrawApp.cpp
 @brief   Implementation for Application Class
 @author    kzawisto
 @created  2015-10-29
 * Copyright: kzawisto ()
 * License:
 **************************************************************/

#include "MyDrawApp.h"
#include "wx/wx.h"
#include "wx/sizer.h"

//(*AppHeaders
#include "MyDrawMain.h"
#include <wx/image.h>

#include <HistoCreator.h>
#include "Menu.h"
//*)

IMPLEMENT_APP(MyHistoApp)

bool MyHistoApp::OnInit() {
//	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
//	frame = new wxFrame((wxFrame *) NULL, -1, wxT("CMS lab"),
//			wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxMAXIMIZE);
//
//	drawPane = new HistoDrawPane((wxFrame*) frame);
//
//	sizer->Add(drawPane, 1, wxEXPAND);
//	frame->SetSizer(sizer);
//	frame->SetAutoLayout(true);
//
//	frame->Show();

	// Menu
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	menu = new MyMenu( wxT("CMS lab"));
//	frame = new wxFrame((wxFrame *) NULL, -1, wxT("CMS lab"),
//			wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxMAXIMIZE);

	drawPane = new HistoDrawPane((wxFrame*) menu);

	sizer->Add(drawPane, 1, wxEXPAND);
	menu->SetSizer(sizer);
	menu->SetAutoLayout(true);

	menu->Show();
	return true;
}
