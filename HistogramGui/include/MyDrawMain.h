/** *************************************************************
 @file      MyDrawMain.h
 @brief   Defines wxWidgets Application Frame
 @author:    kzawisto
 @created:   2015-10-29
 **************************************************************/

#ifndef MYDRAWMAIN_H
#define MYDRAWMAIN_H

#define RESET_BUTTON_ID 1234567
#define SAVE_BUTTON_ID 1234566
#define LOAD_BUTTON_ID 1234565
#define INCREASE_SCALE_BUTTON_ID 1234564
#define DECREASE_SCALE_BUTTON_ID 1234563
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
	wxButton * decreaseScaleButton;;

	///Container for histoCreators
	HistoContainer hc;
	/// Histogram wrappers with is position and dimensions
	std::vector<HistoGraph> histovec;
	///Generic constructor
	HistoDrawPane(wxFrame* parent);

	void paintEvent(wxPaintEvent & evt);
	void sizeEvent(wxSizeEvent & evt);
	void paintNow();

	void render(wxDC& dc);
	void drawHisto(wxDC& dc, MyHistogramWrapper & h, wxPoint from,
			wxSize hsize);
	void drawTics(wxDC& dc, MyHistogramWrapper & h, wxPoint from, wxSize hsize);

	// Mouse events
	void mouseDown(wxMouseEvent& event, bool isLeftMouseDown); // TODO make private or sth
	void leftMouseDown(wxMouseEvent& event);
	void rightMouseDown(wxMouseEvent& event);

	// Button events
	void resetAllCuts(wxCommandEvent& event);
	void saveCuts(wxCommandEvent& event);
	void loadCuts(wxCommandEvent& event);
	void increaseScale(wxCommandEvent& event);
	void decreaseScale(wxCommandEvent& event);

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
