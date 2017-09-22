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

/**This function specifies histogram layout on each redraw of the GUI.
 * Tweak the layout here.
 */
void HistoDrawPane::setHistSizes() {
	if (!hc.good)
		return;
	GetSize(&old_width, &old_height);
	int rows = 2, cols = (histovec[0].size() + 1) / 2;
	int ofs = min(old_width, old_height) / 20;
	int sizex = old_width / cols - ofs * 2, sizey = old_height / rows - ofs * 2;
	wxSize mySize(sizex, sizey);
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols && i * cols + j < histovec[0].size(); ++j) {
			histovec[0][i * cols + j].r = wxRect(
					wxPoint((1 + 2 * j) * ofs + j * sizex,
							10 + (1 + 2 * i) * ofs + i * sizey), mySize);
		}
}

BEGIN_EVENT_TABLE(HistoDrawPane, wxPanel)
// some useful events
/*
 EVT_MOTION(BasicDrawPane::mouseMoved)

 EVT_LEFT_UP(BasicDrawPane::mouseReleased)
 EVT_LEAVE_WINDOW(BasicDrawPane::mouseLeftWindow)
 EVT_KEY_DOWN(BasicDrawPane::keyPressed)
 EVT_KEY_UP(BasicDrawPane::keyReleased)
 EVT_MOUSEWHEEL(BasicDrawPane::mouseWheelMoved)
 */
EVT_LEFT_DOWN(HistoDrawPane::leftMouseDown)
EVT_RIGHT_DOWN(HistoDrawPane::rightMouseDown)
//EVT_LEFT_UP(HistoDrawPane::rightMouseDown)
//TODO right up


EVT_BUTTON(INCREASE_SCALE_BUTTON_ID, HistoDrawPane::increaseScale)
EVT_BUTTON(DECREASE_SCALE_BUTTON_ID, HistoDrawPane::decreaseScale)

/// TODO
EVT_MENU(wxID_EXIT, HistoDrawPane::OnExit)
///

// catch paint events
EVT_PAINT(HistoDrawPane::paintEvent)
EVT_SIZE(HistoDrawPane::sizeEvent)
END_EVENT_TABLE()

// some useful events
/*
 void BasicDrawPane::mouseMoved(wxMouseEvent& event) {}
 void BasicDrawPane::mouseDown(wxMouseEvent& event) {}
 void BasicDrawPane::mouseWheelMoved(wxMouseEvent& event) {}
 void BasicDrawPane::mouseReleased(wxMouseEvent& event) {}
 void BasicDrawPane::rightClick(wxMouseEvent& event) {}
 void BasicDrawPane::mouseLeftWindow(wxMouseEvent& event) {}
 void BasicDrawPane::keyPressed(wxKeyEvent& event) {}
 void BasicDrawPane::keyReleased(wxKeyEvent& event) {}
 */

HistoDrawPane::HistoDrawPane(wxFrame* parent) : wxPanel(parent), hc("config.json") {
	loadButton = new wxButton(this, INCREASE_SCALE_BUTTON_ID, wxT("+"),
			wxPoint(300, 5), wxDefaultSize, 0);
	loadButton = new wxButton(this, DECREASE_SCALE_BUTTON_ID, wxT("-"),
			wxPoint (400, 5), wxDefaultSize, 0);

	// Binding menu events
	function<void (wxCommandEvent &)> saveHandler (bind(&HistoDrawPane::OnSave, this, _1));
	function<void (wxCommandEvent &)> loadHandler (bind(&HistoDrawPane::OnLoad, this, _1));
	function<void (wxCommandEvent &)> resetHandler(bind(&HistoDrawPane::OnReset, this, _1));
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, saveHandler, wxID_SAVE);
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, loadHandler, wxID_OPEN);
    parent->Bind(wxEVT_COMMAND_MENU_SELECTED, resetHandler, wxID_REVERT);

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
	dc.SetPen(wxPen( wxColor(0, 0, 0), 3)); // black line, 3 pixels thick
	int ofs = min(width, height) / 20;
	int sizex = width / 2 - ofs * 3 / 2, sizey = height / 2 - ofs * 3 / 2;
	wxSize mySize(sizex, sizey);

	// Draw sig or sig + bcgr
	for (int a = 0; a < histovec[0].size(); ++a) {
		// If there is some background histos left use them
		if (a < histovec[1].size()) {
			drawHisto(dc, histovec[0][a].histo, histovec[1][a].histo, true, histovec[0][a].r.GetLeftTop(),
					histovec[0][a].r.GetSize());
		} else {// dont draw bacground
			drawHisto(dc, histovec[0][a].histo, histovec[0][a].histo, false, histovec[0][a].r.GetLeftTop(),
					histovec[0][a].r.GetSize());
		}
	}

}
/** Here we draw all the histogram i.e. bins, ticks, labels, frame, and so on
 *  Takes first two histograms from histovec and tries to draw in the same rectangle
 */
void HistoDrawPane::drawHisto(wxDC& dc, MyHistogramWrapper & hist, MyHistogramWrapper & histBcgr,
		bool isBcgr, wxPoint from, wxSize hsize) {
	wxFont signFont(min(hsize.y, hsize.x) / 20,
			wxFontFamily::wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
			wxFontWeight::wxFONTWEIGHT_NORMAL);
	dc.SetFont(signFont);
	dc.SetBrush(*wxWHITE_BRUSH); // blue filling
	dc.SetPen(wxPen( wxColor(0, 0, 0), 2));
	hsize.Set(hsize.x, hsize.y+2);
	dc.DrawRectangle(from, hsize);
	hsize.Set(hsize.x, hsize.y-2);

	wxRect r(from.x + hsize.x / 2 - 50, from.y + hsize.y / 12 - 60, 100, 100);

	wxString mstr(hist.title.c_str(), wxConvUTF8);
	dc.DrawLabel(mstr, r, wxALIGN_CENTER);

	drawTics(dc, hist, from, hsize);

	float histModif = 1; // TODO delete those variables
	float bcgrModif = 1;
	// Get proper normalization of two histograms //TODO
//	if (isBcgr) {
//		if (histBcgr.hmax / histBcgr.eventNum < hist.hmax / hist.eventNum) {
//			bcgrModif = (histBcgr.hmax * hist.eventNum) / (histBcgr.eventNum * hist.hmax);
//		} else {
//			histModif = (histBcgr.eventNum * hist.hmax) / (histBcgr.hmax * hist.eventNum);
//		}
//	}

	// Draw histogram points
	MyHistogramWrapper * hists[2];
	hists[0] = &hist;
	hists[1] = &histBcgr;
	wxColour colours[2] = {wxColor(10, 10, 10), wxColor(0, 0, 220)};

	for (int k = 0; k < 2 && (k == 0 || isBcgr); ++k) {
		float binLength = hsize.x / hists[k]->bins->size();
		for (int i = 0; i < hists[k]->bins->size(); ++i) {
			int x = from.x + i * hsize.x / hists[k]->bins->size();
			int y = int(from.y + (1 - hists[k]->getNormalizedBin(i)*histoSizeModifier*histModif) * hsize.y);

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
					// left bin side
					dc.DrawLine(wxPoint(x,y), wxPoint(x,from.y + hsize.y-1));
				}
				if (i < hists[k]->bins->size()-1) {
					nextY = max(from.y,int(from.y + (1 - hists[k]->getNormalizedBin(i+1)*histoSizeModifier*histModif) * hsize.y));
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
		// vertical
		dc.DrawLine(wxPoint(from.x + hsize.x * j, from.y + hsize.y-1),
				wxPoint(from.x + hsize.x * j,
						from.y + hsize.y - hsize.y * 0.015f));
		// horizontal
		dc.DrawLine(wxPoint(from.x, from.y + hsize.y * j),
				wxPoint(from.x + hsize.x * 0.01f, from.y + hsize.y * j));
	}
	for (float j = hist.numbered_tics_spacing; j <= 1.f;
			j += hist.numbered_tics_spacing) {
		wxRect r(from.x + hsize.x * j - 20,
				from.y + hsize.y - 10 + fontSize * 2, 40, 20);
		string s1 = Utils::itos<double>(hist.xmin + (hist.xmax - hist.xmin) * j,
				2);
		dc.DrawLabel(wxString(s1.c_str(), wxConvUTF8), r, wxALIGN_CENTER);
	}

	// Labels
	for (float j = hist.numbered_tics_spacing; j <= 1.f;
			j += hist.numbered_tics_spacing) {
		wxRect r1(from.x - 20 - fontSize * 2.5, from.y + hsize.y * (1 - j) - 10,
				40, 20);
		string s2 = Utils::itos<double>(hist.hmin + (hist.hmax - hist.hmin)*j/histoSizeModifier,
				2);
		dc.DrawLabel(wxString(s2.c_str(), wxConvUTF8), r1, wxALIGN_CENTER);
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
/** Here we react to clicks at point x,y, by setting cuts to the given histogram and resetting
 * all the histograms from the HistoCreator it belongs to
 */
// TODO change for both histos
void HistoDrawPane::mouseDown(wxMouseEvent& event, bool isLeftMouseDown) {
	long int x, y;
	event.GetPosition(&x, &y);
	wxPoint p = event.GetPosition();
	int a = 0;
	for (; a < histovec[0].size(); ++a)
		if (histovec[0][a].r.Contains(p))
			break;
	if (a == histovec[0].size())
		return;

	int cut = histovec[0][a].histo.bins->size() * ((double) (x - histovec[0][a].r.x))
			/ (histovec[0][a].r.width);
	if (isLeftMouseDown) {
		if (histovec[0][a].histo.cutHigh && cut >= histovec[0][a].histo.cutHigh)
			return;
		histovec[0][a].histo.setCutLow(cut);
		if (a < histovec[1].size()) { // Background histogram
			histovec[1][a].histo.setCutLow(cut);
		}
	} else { // isRightMouseDown
		cut++;
		if (cut <= histovec[0][a].histo.cutLow)
			return;
		histovec[0][a].histo.setCutHigh(cut);
		if (a < histovec[1].size()) { // Background histogram
			histovec[1][a].histo.setCutHigh(cut);
		}
	}

	Refresh();
}

void HistoDrawPane::leftMouseDown(wxMouseEvent& event) {
	this->mouseDown(event, LEFT_BUTTON_DOWN);
}

void HistoDrawPane::rightMouseDown(wxMouseEvent& event) {
	this->mouseDown(event, RIGHT_BUTTON_DOWN);
}
void HistoDrawPane::OnReset(wxCommandEvent& event) {
	for (int i = 0; i < histovec[0].size(); ++i) {
		histovec[0][i].histo.setCutLow(0);
		histovec[0][i].histo.setCutHigh(histovec[0][i].histo.bins->size());
	}
	histoSizeModifier = 0.9;
	Refresh();
}

// TODO bcgr
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
				histovec[0][i].histo.setCutLow(cutLow);
				histovec[0][i].histo.setCutHigh(cutHigh);
			} else {
				wxMessageBox(wxT("The file is too short."), wxT("Load failure"), wxICON_ERROR);
				return;
			}
		}
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
