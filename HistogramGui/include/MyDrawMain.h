/** *************************************************************
 @file      MyDrawMain.h
 @brief   Defines wxWidgets Application Frame
 @author:    kzawisto
 @created:   2015-10-29
 **************************************************************/

#ifndef MYDRAWMAIN_H
#define MYDRAWMAIN_H

#define RESET_BUTTON_ID 100
#define SAVE_BUTTON_ID 101
#define LOAD_BUTTON_ID 102
#define INCREASE_SCALE_BUTTON_ID 103
#define DECREASE_SCALE_BUTTON_ID 104
#define LEFT_BUTTON_DOWN true
#define RIGHT_BUTTON_DOWN false


//(Headers(MyDrawFrame)
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
//*)
#include "wx/wx.h"
#include "wx/sizer.h"
#include <vector>

#include "HistoContainer.h"
#include "MyHistogramWrapper.h"

using namespace std;

///generic wxWidgets panel for drawing, and user interaction
class HistoDrawPane: public wxPanel {
	int old_width, old_height;

public:
	float histoSizeModifier = 0.9;

	// Buttons
	wxButton * resetButton;
	wxButton * saveButton;
	wxButton * loadButton;
	wxButton * increaseScaleButton;
	wxButton * decreaseScaleButton;

	///Container for histoCreators
	HistoContainer hc;
	/// Histogram wrappers with is position and dimensions
	std::vector<std::vector<HistoGraph> > histovec;
	///Generic constructor
	HistoDrawPane(wxFrame* parent);

	void paintEvent(wxPaintEvent & evt);
	void sizeEvent(wxSizeEvent & evt);
	void paintNow();

	void render(wxDC& dc);
	void drawHisto(wxDC& dc, MyHistogramWrapper & h, MyHistogramWrapper & hBcgr, bool isBcgr,
			wxPoint from, wxSize hsize);
	void drawTics(wxDC& dc, MyHistogramWrapper & h, wxPoint from, wxSize hsize);

	// Mouse events
	void mouseDown(wxMouseEvent& event, bool isLeftMouseDown); // TODO make private or sth
	void leftMouseDown(wxMouseEvent& event);
	void rightMouseDown(wxMouseEvent& event);

	// Button events TODO wywal
	void increaseScale(wxCommandEvent& event);
	void decreaseScale(wxCommandEvent& event);

	// Menu events
	void OnSave(wxCommandEvent& event);
	void OnLoad(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnReset(wxCommandEvent& event);

	void setHistSizes();
	~HistoDrawPane() {
	}

	// some useful events
	/*
	 void mouseMoved(wxMouseEvent& event);

	 void mouseWheelMoved(wxMouseEvent& event);
	 void mouseReleased(wxMouseEvent& event);
	 void rightClick(wxMouseEvent& event);
	 void mouseLeftWindow(wxMouseEvent& event);
	 void keyPressed(wxKeyEvent& event);
	 void keyReleased(wxKeyEvent& event);
	 */

DECLARE_EVENT_TABLE()
};

#endif // MYDRAWMAIN_H
