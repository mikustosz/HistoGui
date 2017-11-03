/** *************************************************************
 @file      MyDrawMain.h
 @brief   Defines wxWidgets Application Frame
 @author:    kzawisto
 @created:   2015-10-29
 **************************************************************/

#ifndef MYDRAWMAIN_H
#define MYDRAWMAIN_H

#define RESET_BUTTON_ID 100
#define NEXT_BUTTON_ID 101
#define BACK_BUTTON_ID 102
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
#include <map>

#include "HistoContainer.h"
#include "MyHistogramWrapper.h"

using namespace std;

// Generic wxWidgets panel for drawing, and user interaction
class HistoDrawPane: public wxPanel {
	int old_width, old_height;

public:
	int getMaxColNr();
	int getMaxRowNr();
	bool histoVisible(int idx);

	// Percentage of height reserved for histogram data (default - 90% from down to up)
	float histoSizeModifier = 0.9;

	// For histograms pagination
	int pageNr = 1;

	// Buttons
	wxButton * nextButton;
	wxButton * backButton;

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
	void mouseDown(wxMouseEvent& event, bool isLeftMouseDown); // TODO make private
	void leftMouseDown(wxMouseEvent& event);
	void rightMouseDown(wxMouseEvent& event);

	// Button events TODO wywal te dwa na górze
	void increaseScale(wxCommandEvent& event);
	void decreaseScale(wxCommandEvent& event);
	void OnPressNext(wxCommandEvent& event);
	void OnPressBack(wxCommandEvent& event);

	// Menu events
	void OnSave(wxCommandEvent& event);
	void OnLoad(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnReset(wxCommandEvent& event);

	void setHistSizes();
	~HistoDrawPane() {
	}

DECLARE_EVENT_TABLE()
};

#endif // MYDRAWMAIN_H
