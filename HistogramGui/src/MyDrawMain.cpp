/** *************************************************************
 @file      MyDrawMain.cpp
 @brief   Code for Application Frame
 @author    kzawisto
 @created:   2015-10-29

 **************************************************************/

#include "MyDrawMain.h"
#include "Utils.h"
#include "wx/font.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/msgdlg.h>
#include <sstream>

// TODO fix bug with the same page number during window resize
int HistoDrawPane::getMaxColNr() {
	int width, height;
	GetSize(&width, &height);
	return max(1, width / 550);
}
int HistoDrawPane::getMaxRowNr() {
	int width, height;
	GetSize(&width, &height);
	return max(1, height / 300);
}
bool HistoDrawPane::histoVisible(int idx) {
	int histosOnPage = getMaxRowNr() * getMaxColNr();
	return (pageNr-1) * histosOnPage -1 < idx && idx < pageNr * histosOnPage;
}

void HistoDrawPane::setHistSizes() {
	if (!hc.good)
		return;
	GetSize(&old_width, &old_height);
	// cols - real number of columns including hidden ones
	int rows = getMaxRowNr();
	int cols = getMaxColNr();
	int histsOnPage = rows * cols;

	int ofs = 20 + min(old_width, old_height) / 40;
	int sizex = old_width  / cols - ofs * 2;
	int sizey = old_height / rows - ofs * 2;
	wxSize mySize(sizex, sizey);

	int maxPageNr = 1 + ((histovec[0].size() - 1) / histsOnPage); // Round up
	int histIdx = 0;
	for (int pg = 0; pg < maxPageNr; ++pg) {
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols && histIdx < histovec[0].size(); ++j) {
				histovec[0][histIdx].r = wxRect(
						wxPoint(15 + (1 + 2 * (j % cols)) * ofs + (j % cols) * sizex, 10 + (1 + 2 * i) * ofs + i * sizey),
						mySize
						);
				histIdx++;
			}
		}
	}
}

BEGIN_EVENT_TABLE(HistoDrawPane, wxPanel)

EVT_LEFT_DOWN(HistoDrawPane::leftMouseDown)
EVT_RIGHT_DOWN(HistoDrawPane::rightMouseDown)
EVT_MOTION(HistoDrawPane::mouseMoved)

EVT_BUTTON(INCREASE_SCALE_BUTTON_ID, HistoDrawPane::increaseScale)
EVT_BUTTON(DECREASE_SCALE_BUTTON_ID, HistoDrawPane::decreaseScale)

EVT_BUTTON(NEXT_BUTTON_ID, HistoDrawPane::OnPressNext)
EVT_BUTTON(BACK_BUTTON_ID, HistoDrawPane::OnPressBack)

// catch paint events
EVT_PAINT(HistoDrawPane::paintEvent)
EVT_SIZE(HistoDrawPane::sizeEvent)
END_EVENT_TABLE()

HistoDrawPane::HistoDrawPane(wxFrame* parent) : wxPanel(parent), hc("config.json") {


	int textHeight = 6;
	wxSize * buttonSize = new wxSize(150, 30);
	nextButton = new wxButton(this, NEXT_BUTTON_ID, wxT(">"), wxPoint(GetVirtualSize().GetWidth() , 0), *buttonSize, 0);
	backButton = new wxButton(this, BACK_BUTTON_ID, wxT("<"), wxPoint(0, 0), *buttonSize, 0);

	new wxStaticText(this, PAGE_NR_TEXT_ID, wxT("Page: "), wxPoint(165, textHeight), wxDefaultSize, 0);
	pageNumberText = new wxStaticText(this, PAGE_NR_TEXT_ID, to_string(pageNr), wxPoint(200, textHeight), wxDefaultSize, 0);

	// Binding menu events
	function<void (wxCommandEvent &)> saveHandler (bind(&HistoDrawPane::OnSave, this, _1));
	function<void (wxCommandEvent &)> loadHandler (bind(&HistoDrawPane::OnLoad, this, _1));
	function<void (wxCommandEvent &)> resetHandler(bind(&HistoDrawPane::OnReset, this, _1));
	function<void (wxCommandEvent &)> zoomInHandler(bind(&HistoDrawPane::increaseScale, this, _1));
	function<void (wxCommandEvent &)> zoomOutHandler(bind(&HistoDrawPane::decreaseScale, this, _1));
	function<void (wxCommandEvent &)> toggleBacgkroundHandler(bind(&HistoDrawPane::toggleBackground, this, _1));
	function<void (wxCommandEvent &)> aboutHandler(bind(&HistoDrawPane::about, this, _1));
	function<void (wxCommandEvent &)> usageHandler(bind(&HistoDrawPane::usage, this, _1));
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, saveHandler, wxID_SAVE);
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, loadHandler, wxID_OPEN);
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, resetHandler, wxID_REVERT);
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, zoomInHandler, INCREASE_SCALE_BUTTON_ID);
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, zoomOutHandler, DECREASE_SCALE_BUTTON_ID);
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, toggleBacgkroundHandler, TOGGLE_BACKGROUND_ID);
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, aboutHandler, ABOUT_ID);
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, usageHandler, USAGE_ID);
    // Bind key event
    parent->Bind(wxEVT_CHAR_HOOK, &HistoDrawPane::OnKeyDown, this);

	if (hc.good) {
		histovec = hc.buildGuiHistos();
		hc.createLogicalHistos();
		for (auto &h : histovec){
			if (!h.empty()) {
				for (auto &hh : h) {
					hh.histo.setH();
				}
			}
		}
		setHistSizes();
	}
	new wxStaticText(this, EVENT_NR_TEXT_ID, wxT("Events: "), wxPoint(220, textHeight), wxDefaultSize, 0);
	eventNumberText = new wxStaticText(this, EVENT_NR_TEXT_ID, to_string(getEvents()), wxPoint(264, textHeight), wxDefaultSize, 0);
}

/**
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void HistoDrawPane::paintEvent(wxPaintEvent & evt) {
	wxPaintDC dc(this);
	render(dc);
}

/**
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 *
 * In most cases, this will not be needed at all; simply handling
 * paint events and calling Refresh() when a refresh is needed
 * will do the job.
 */
void HistoDrawPane::paintNow() {
	wxClientDC dc(this);
	render(dc);
}

/**
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void HistoDrawPane::render(wxDC& dc) {
	int width, height;
	dc.GetSize(&width, &height);
	setHistSizes();
	dc.SetPen(wxPen(wxColor(0, 0, 0), 3)); // black line, 3 pixels thick
	int ofs = min(width, height) / 20;
	int sizex = width / 2 - ofs * 3 / 2, sizey = height / 2 - ofs * 3 / 2;
	wxSize mySize(sizex, sizey);
	// Next button always on the right side of the window
	nextButton->Move(GetVirtualSize().GetWidth()-150, 0);

	// Draw sig or draw sig + bcgr
	int histosOnPage = getMaxRowNr() * getMaxColNr();
	for (int j = (pageNr-1) * histosOnPage; j < histovec[0].size() && j < pageNr * histosOnPage; ++j) {
		// If there is some background histos left use them
		if (j < histovec[1].size() && backgrVisible) {
			drawHisto(dc, histovec[0][j].histo, histovec[1][j].histo, true, histovec[0][j].r.GetLeftTop(),
					histovec[0][j].r.GetSize());
		} else { // Don't use background
			drawHisto(dc, histovec[0][j].histo, histovec[0][j].histo, false, histovec[0][j].r.GetLeftTop(),
					histovec[0][j].r.GetSize());
		}
	}
}
/** Here we draw all the histogram i.e. bins, ticks, labels, frame, and so on
 *  Takes first two histograms from histovec and draws in the same rectangle
 */
void HistoDrawPane::drawHisto(wxDC& dc, MyHistogramWrapper & hist, MyHistogramWrapper & histBcgr,
		bool isBcgr, wxPoint from, wxSize hsize) {
	wxFont signFont(min(hsize.y, hsize.x) / 20,
			wxFontFamily::wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
			wxFontWeight::wxFONTWEIGHT_NORMAL);
	dc.SetFont(signFont);
	dc.SetBrush(*wxWHITE_BRUSH); // blue filling
	dc.SetPen(wxPen(wxColor(0, 0, 0), 2));
	hsize.Set(hsize.x, hsize.y+2);
	dc.DrawRectangle(from, hsize);
	hsize.Set(hsize.x, hsize.y-2);

	wxRect r(from.x + hsize.x / 2 - 50, from.y + hsize.y / 12 - 60, 100, 100);
	wxString mstr(hist.title.c_str(), wxConvUTF8);
	dc.DrawLabel(mstr, r, wxALIGN_CENTER);

	drawTics(dc, hist, from, hsize);

	// Draw histogram poles
	MyHistogramWrapper * hists[2];
	hists[0] = &hist;
	hists[1] = &histBcgr;
	wxColour colours[2] = {wxColor(10, 10, 100), wxColor(255, 100, 100)};

	int offset = 0; // Bckgr is moved one px to the right for better visibility
	for (int k = 0; k < 2 && (k == 0 || isBcgr); ++k, offset++) {
		float binLength = hsize.x / hists[k]->bins->size();
		for (int i = 0; i < hists[k]->bins->size(); ++i) {
			int x = from.x + i * hsize.x / hists[k]->bins->size() + offset;
			int y = int(from.y + (1 - hists[k]->getNormalizedBin(i)*histoSizeModifier) * hsize.y);

			if (y <= from.y) { // If the height of the histogram is out of scale, mark the dot with different color
				y = int (from.y);
				dc.SetPen(wxPen(wxColor(234, 0, 0), 1));
			}

			if (hists[k]->getNormalizedBin(i) > 0) {
				// Horizontal bin line
				dc.DrawLine(wxPoint(x,y), wxPoint(x+binLength,y));
				dc.SetPen(wxPen(colours[k], 1));
				int nextY = from.y + hsize.y;

				// Vertical lines
				if (i == 0 || hists[k]->getNormalizedBin(i-1) == 0) {
					// Left bin side
					dc.DrawLine(wxPoint(x,y), wxPoint(x,from.y + hsize.y-1));
				}
				if (i < hists[k]->bins->size()-1) {
					nextY = max(from.y,int(from.y + (1 - hists[k]->getNormalizedBin(i+1)*histoSizeModifier) * hsize.y));
				}
				// right bin side
				dc.DrawLine(wxPoint(x+binLength,y), wxPoint(x+binLength,nextY));
			}
		}
	}
}
/**
 * Function triggered by wxWidgets when the window is resized. We trigger setHistSize in render()
 * anyway, so we just Refresh() here.
 */
void HistoDrawPane::sizeEvent(wxSizeEvent& evt) {
	Refresh();
}
/**
 * Draw ticks and labels
 */
void HistoDrawPane::drawTics(wxDC& dc, MyHistogramWrapper& hist, wxPoint from,
		wxSize hsize) {
	int fontSize = min(hsize.y, hsize.x) / 40;
	dc.SetPen(wxPen( wxColor(0, 0, 0), 1));
	wxFont ticsFont(fontSize, wxFontFamily::wxFONTFAMILY_DEFAULT,
			wxFONTSTYLE_NORMAL, wxFontWeight::wxFONTWEIGHT_NORMAL);
	dc.SetFont(ticsFont);
	for (float j = 0; j <= 1.f; j += hist.tics_spacing) {
		// Vertical
		dc.DrawLine(wxPoint(from.x + hsize.x * j, from.y + hsize.y-1),
				wxPoint(from.x + hsize.x * j,
						from.y + hsize.y - hsize.y * 0.015f));
		// Horizontal
		dc.DrawLine(wxPoint(from.x, from.y + hsize.y * j),
				wxPoint(from.x + hsize.x * 0.01f, from.y + hsize.y * j));
	}

	// Labels
	for (float j = 0; j <= 1.f; j += hist.numbered_tics_spacing) { // Horizontal labels
		wxRect r(from.x + hsize.x * j, from.y + hsize.y + fontSize*1.4, 0, 0);
		string s1 = Utils::format(hist.xmin + (hist.xmax - hist.xmin) * j, 2);
		dc.DrawLabel(wxString(s1.c_str(), wxConvUTF8), r, wxALIGN_CENTRE);
	}
	for (float j = hist.numbered_tics_spacing; j <= 1.f; j += hist.numbered_tics_spacing) { // Vertical labels
		wxRect r1(from.x - 2, from.y + hsize.y * (1 - j), 0, 0);
		string s2 = Utils::format(hist.hmin + (hist.hmax - hist.hmin)*j/histoSizeModifier, 2);
		dc.DrawLabel(wxString(s2.c_str(), wxConvUTF8), r1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
	}

	// Cuts
	if (hist.cutLow) {
		double c = hist.cutLow;
		c /= hist.bins->size();
		int x = from.x + hsize.x * c;
		dc.SetPen(wxPen( wxColor(204, 0, 0), 2));
		dc.DrawLine(wxPoint(x, from.y), wxPoint(x, from.y + hsize.y));
	}
	if (hist.cutHigh && hist.cutHigh != hist.bins->size()) {
		double c = hist.cutHigh;
		c /= hist.bins->size();
		int x = from.x + hsize.x * c;
		dc.SetPen(wxPen( wxColor(0, 204, 0), 2));
		dc.DrawLine(wxPoint(x, from.y), wxPoint(x, from.y + hsize.y));
	}
	dc.SetPen(wxPen( wxColor(0, 0, 0), 1));
}
// Function returns current number of events (regarding all cuts)
int HistoDrawPane::getEvents() {
	int binSum = 0;
	for (auto b: * histovec[0][0].histo.bins) {
		binSum += b;
	}
	return binSum;
}

/** Here we react to clicks at point x,y, by setting cuts to the given histogram and resetting
 * all the histograms from the HistoCreator it belongs to
 */
void HistoDrawPane::mouseDown(wxMouseEvent& event, bool isLeftMouseDown) {
	long int x, y;
	event.GetPosition(&x, &y);
	wxPoint p = event.GetPosition();
	int a = 0;
	for (; a < histovec[0].size(); ++a)
		if (histovec[0][a].r.Contains(p) && histoVisible(a))
			break;
	if (a == histovec[0].size())
		return;
	int cut = histovec[0][a].histo.bins->size() * ((double) (x - histovec[0][a].r.x))
			/ (histovec[0][a].r.width);
	if (isLeftMouseDown) {
		if (histovec[0][a].histo.cutHigh && cut >= histovec[0][a].histo.cutHigh)
			return;
		m_dragging = 1;
		histovec[0][a].histo.setCutLow(cut);
		if (a < histovec[1].size()) { // Background histogram
			histovec[1][a].histo.setCutLow(cut);
		}
	} else { // isRightMouseDown
		cut++;
		if (cut <= histovec[0][a].histo.cutLow)
			return;
		m_dragging = 2;
		histovec[0][a].histo.setCutHigh(cut);
		if (a < histovec[1].size()) { // Background histogram
			histovec[1][a].histo.setCutHigh(cut);
		}
	}
	eventNumberText->SetLabel(to_string(getEvents()));
	Refresh();
}

void HistoDrawPane::mouseMoved(wxMouseEvent& event) {
    if (event.Dragging()) {
    	switch(m_dragging) {
    	case 1:
    		mouseDown(event, LEFT_BUTTON_DOWN);
    		break;
    	case 2:
    		mouseDown(event, RIGHT_BUTTON_DOWN);
    		break;
    	}
    }
}

void HistoDrawPane::leftMouseDown(wxMouseEvent& event) {
	this->mouseDown(event, LEFT_BUTTON_DOWN);
}
void HistoDrawPane::rightMouseDown(wxMouseEvent& event) {
	this->mouseDown(event, RIGHT_BUTTON_DOWN);
}
void HistoDrawPane::OnReset(wxCommandEvent& event) {
	for (int g = 0; g < histovec.size(); g++) {
		for (int i = 0; i < histovec[g].size(); ++i) {
			histovec[g][i].histo.setCutLowFast(0);
			histovec[g][i].histo.setCutHighFast(histovec[g][i].histo.bins->size());
		}
	}
	histovec[0][0].histo.createHistos();
	if (histovec.size() > 1) {
		histovec[1][0].histo.createHistos();
	}
	histoSizeModifier = 0.9;
	eventNumberText->SetLabel(to_string(getEvents()));
	Refresh();
}

void HistoDrawPane::OnLoad(wxCommandEvent& event) {
	wxString fileName;
	wxFileDialog * openFileDialog = new wxFileDialog(this);

	if (openFileDialog->ShowModal() == wxID_OK) {
		fileName = openFileDialog->GetPath();
		// TODO check if file has right extension, eg. *.ser
		ifstream infile(fileName.ToAscii());
		int histNum, cutLow, cutHigh;
		if (!(infile >> histNum)) {
			wxMessageBox(wxT("The file couldn't be loaded."), wxT("Load failure"), wxICON_ERROR);
			return;
		}
		for (int i = 0; i < histNum; ++i) {
			if (infile >> cutLow >> cutHigh) {
				histovec[0][i].histo.setCutLowFast(cutLow);
				histovec[0][i].histo.setCutHighFast(cutHigh);
				if (histovec.size() > 1 && histovec[1].size() > i) {
					histovec[1][i].histo.setCutLowFast(cutLow);
					histovec[1][i].histo.setCutHighFast(cutHigh);
				}
			} else {
				wxMessageBox(wxT("The file is too short."), wxT("Load failure"), wxICON_ERROR);
				return;
			}
		}
		histovec[0][0].histo.createHistos();
		if (histovec.size() > 1) {
			histovec[1][0].histo.createHistos();
		}
		eventNumberText->SetLabel(to_string(getEvents()));
		Refresh();
	}
}
void HistoDrawPane::increaseScale(wxCommandEvent& event) {
	histoSizeModifier *= 1.15;
	Refresh();
}
void HistoDrawPane::decreaseScale(wxCommandEvent& event) {
	histoSizeModifier *= 0.85;
	Refresh();
}

void HistoDrawPane::OnSave(wxCommandEvent& event) {
	// Save dialog
    const wxString& message = wxFileSelectorPromptStr;
    const wxString& defaultDir = wxEmptyString;
    const wxString& defaultFile = wxEmptyString;
    const wxString& wildCard = wxFileSelectorDefaultWildcardStr;
    long style = wxFD_SAVE;
    const wxPoint& pos = wxDefaultPosition;
    const wxSize& sz = wxDefaultSize;
    const wxString& name = wxFileDialogNameStr;

	wxFileDialog * saveFileDialog = new wxFileDialog(this, message,
			defaultDir, defaultFile, wildCard, style, pos, sz, name);

	int status = saveFileDialog->ShowModal();

	string path;
	if (status == wxID_OK) {
		path = saveFileDialog->GetPath().ToAscii();
	} else if (status == wxID_CANCEL) {
		return;
	}
	// Create file
	ofstream fileStream(path);
	ostringstream stringStream;

	stringStream << histovec[0].size() << endl;
	for (int i = 0; i < histovec[0].size(); ++i) {
		//TODO hack
		int cutHigh = histovec[0][i].histo.cutHigh;
		if (!histovec[0][i].histo.cutHigh) {
			cutHigh = histovec[0][i].histo.bins->size();
		}
		stringStream << histovec[0][i].histo.cutLow << " "
				<< cutHigh << endl;
	}
	fileStream << stringStream.str();
	fileStream.close();
}

void HistoDrawPane::OnExit(wxCommandEvent& event) {
	Close(true);
}

// Button handler functions
void HistoDrawPane::OnPressNext(wxCommandEvent& event) {
	int histosNum = histovec[0].size();
	if (pageNr * getMaxColNr() * getMaxRowNr() < histosNum) {
		pageNr++;
		pageNumberText->SetLabel(to_string(pageNr));
		Refresh();
	}
}
void HistoDrawPane::OnPressBack(wxCommandEvent& event) {
	if (pageNr > 1) {
		pageNr--;
		pageNumberText->SetLabel(to_string(pageNr));
		Refresh();
	}
}

// Key handler function
void HistoDrawPane::OnKeyDown(wxKeyEvent& event) {
    switch((int)event.GetKeyCode()) {
    case 314: // Key code for ←
    	OnPressBack(* new wxCommandEvent());
    	break;
    case 316: // Key code for →
    	OnPressNext(* new wxCommandEvent());
    	break;
    case 315: // Key code for ↑
    	increaseScale(* new wxCommandEvent());
    	break;
    case 317: // Key code for ↓
    	decreaseScale(* new wxCommandEvent());
    	break;

    }
    event.Skip();
}
void HistoDrawPane::toggleBackground(wxCommandEvent& event) {
	backgrVisible = !backgrVisible;
	Refresh();
}
void HistoDrawPane::about(wxCommandEvent& event) {
	wxMessageBox(
			* new wxString(//ser
				"The program CMS lab is designed to provide easy tool for data analysis from particle detectors from Large Hadron Collider.\n\nCMS lab was first developed by Krystian Zawistowski in 2015, later the program was improved by Michał Kustosz in 2017 as a Bachelor thesis under supervision of dr hab. Artur Kalinowski."
				, wxConvUTF8)
			);
}
void HistoDrawPane::usage(wxCommandEvent& event) {
	wxMessageBox(
			* new wxString(//ser
				"To cut histogram from the left/right side click (or hold) left/right mouse button\nTo move between histogram pages press left/right arrow on the screen or keyboard\nTo zoom in/out use up/down keys on the keyboard\nTo toggle the background histogram press 'b'\nTo help somebody donate money here https://www.caritas.org/donate-now/"
				, wxConvUTF8)
			);
}

