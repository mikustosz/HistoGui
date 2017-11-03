/**
 @file    HistoCreator.cpp
 @brief HistoCreator implementation
 @author   kzawisto
 @created:   2015-10-29
 **************************************************************/

#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include <fstream>
#include <iostream>
#include <DataSource.h>
#include <HistoCreator.h>
using namespace std;

///////////////////////// TODO wywal
//#include <TRandom3.h>
//#include <TLorentzVector.h>
//
//namespace {
//  typedef struct {
//    float mass, ptL1, ptL2, isoL1, isoL2;
//  } ENTRY;
//}
//void tools(){
//
//  unsigned int v[(int)1E7];
//
//  for(int i=0;i<1E7;++i){
//    v[i]++;
//  }
//
//}
//
//void makeTestTree(){
//
//  ENTRY aEntry;
//  TFile file("TestData.root","RECREATE");
//  TTree *tree = new TTree("TestTree","TestTree");
//  tree->Branch("entry",&aEntry,"mass/f:ptL1/f:ptL2/f:isoL1/f:isoL2/f");
//
//  TFile fileSgn("TestDataSgn.root","RECREATE");
//  TTree *treeSgn = new TTree("TestTree","TestTree");
//  treeSgn->Branch("entry",&aEntry,"mass/f:ptL1/f:ptL2/f:isoL1/f:isoL2/f");
//
//  TRandom3 r;
//  ///Signal
//  TLorentzVector l1;
//  TLorentzVector l2;
//  float massL1 = 5;
//  float massL2 = 5;
//
//  TH1F *hMass = new TH1F("hMass","Mass",98,0,200);
//  TH1F *hPtL1 = new TH1F("hPtL1","PtL1",98,0,100);
//  TH1F *hPtL2 = new TH1F("hPtL2","PtL2",98,0,100);
//  TH1F *hIsoL1 = new TH1F("hIsoL1","IsoL1",98,0,20);
//  TH1F *hIsoL2 = new TH1F("hIsoL2","IsoL2",98,0,20);
//
//  ofstream out("Data.dat");
//
//
//  for(int iEv=0;iEv<500;++iEv){
//    float x = r.Gaus(20,5);
//    float y = r.Gaus(20,5);
//    float z = r.Gaus(20,5);
//    float mass = r.Gaus(100,5);
//    float e = sqrt(x*x + y*y + z*z + mass*mass);
//    TLorentzVector aParticle(x,y,z,e);
//    x = r.Gaus(5,5);
//    y = r.Gaus(5,5);
//    z = r.Gaus(5,5);
//    e = sqrt(x*x + y*y + z*z + massL1*massL1);
//    l1 = TLorentzVector(x,y,z,e);
//    l2 = aParticle - l1;
//
//    aEntry.ptL1 = l1.Pt();
//    aEntry.ptL2 = l2.Pt();
//    aEntry.mass = (l1+l2).M();
//    aEntry.isoL1 = r.Exp(5);
//    aEntry.isoL2 = r.Exp(5);
//
//    int iBin1 = hMass->FindBin(aEntry.mass);
//    int iBin2 = hPtL1->FindBin(aEntry.ptL1);
//    int iBin3 = hPtL2->FindBin(aEntry.ptL2);
//    int iBin4 = hIsoL1->FindBin(aEntry.isoL1);
//    int iBin5 = hIsoL2->FindBin(aEntry.isoL2);
//
//    unsigned int aVal = iBin1+
//                        100*iBin2+
//                        100*100*iBin3+
//                        100*100*100*iBin4+
//                        100*100*100*100*iBin5;
//    out<<aVal<<std::endl;
//    std::cout<<aVal<<" "<<iBin1<<std::endl;
//
//    tree->Fill();
//    treeSgn->Fill();
//  }
//  ///Background
// for(int iEv=0;iEv<50000;++iEv){
//    float x = r.Exp(20);
//    float y = r.Exp(20);
//    float z = r.Exp(20);
//    float e = sqrt(x*x + y*y + z*z + r.Exp(5));
//    l1 = TLorentzVector(x,y,z,e);
//    x = r.Exp(20);
//    y = r.Exp(20);
//    z = r.Exp(20);
//    e = sqrt(x*x + y*y + z*z + r.Exp(5));
//    l2 = TLorentzVector(x,y,z,e);
//
//    aEntry.ptL1 = l1.Pt();
//    aEntry.ptL2 = l2.Pt();
//    aEntry.mass = (l1+l2).M();
//    aEntry.isoL1 = tf.Exp(20);
//    aEntry.isoL2 = r.Exp(20);
//    tree->Fill();
//
//    int iBin1 = hMass->FindBin(aEntry.mass);
//    int iBin2 = hPtL1->FindBin(aEntry.ptL1);
//    int iBin3 = hPtL2->FindBin(aEntry.ptL2);
//    int iBin4 = hIsoL1->FindBin(aEntry.isoL1);
//    int iBin5 = hIsoL2->FindBin(aEntry.isoL2);
//
//    unsigned int aVal = iBin1+
//                        100*iBin2+
//                        100*100*iBin3+
//                        100*100*100*iBin4+
//                        100*100*100*100*iBin5;
//
//    out<<aVal<<std::endl;
//  }
// file.Write();
// fileSgn.Write();
// out.close();
//}
/////////////////////////


/**
 * Unused, takes configuration from json at _configPath.
 */
//HistoCreator::HistoCreator(string _configPath) :
//		configPath(_configPath), hc(_configPath) {
//	for (int i = 0; i < hc.vec.size(); ++i) {
//		histos.push_back(vector<unsigned int>(hc.vec[i].bins));
//		cutsLow.push_back(0);
//		cutsHigh.push_back(hc.vec[i].bins);
//	}
//}

///**
// * Constructor takes configuration from boost::program_options ptree, loaded from json_config
// */
HistoCreator::HistoCreator(ptree pt) : hc(pt) {

	for (int i = 0; i < hc.vec.size(); ++i) {
		histos.push_back(vector<unsigned int>(hc.vec[i].bins));
		cutsLow.push_back(0);
		cutsHigh.push_back(hc.vec[i].bins);
	}
}

/**
 * Loads ROOT file from hc.rootDataFile path and saves multiplexed binary file to hc.myDataFile path/
 */
void HistoCreator::processTree() {
	//cout << "sizeof(char*): "<< sizeof(char*) << endl;
	TFile * file = new TFile(hc.rootDataFile.c_str());
	TTree* tree = (TTree*) file->Get(hc.treeName.c_str());
	fstream file1(hc.myDataFile.c_str(), fstream::out | fstream::binary);
	tree->GetEntries();
	float val[32];
	for (int i = 0; i < 32; ++i)
		val[i] = 0;
	tree->SetBranchAddress(hc.branchName.c_str(), val);
	for (int i = 0; i < hc.numOfEvents; ++i) {
		tree->GetEntry(i);
		for (int k = 0; k < hc.vec.size(); ++k) {
			unsigned int j = hc.getBin(k, val[k]);
			if (j == UINT_MAX) {
				//cout << " processTree(): k: " << k << " j: " << j << endl;
			}
			//cout << "write j: " << j << endl;
			file1.write((char*) &j, hc.vec[k].bytes); // TODO tu nie bangla
		}
	}
	////
//	fstream file2("serniczekwywalTODO", fstream::out | fstream::binary);
//	unsigned int jj = 255;
//	file2.write((char*) &jj, hc.vec[0].bytes);
//	jj = 256;
//	file2.write((char*) &jj, hc.vec[0].bytes);
//	jj = 257;
//	file2.write((char*) &jj, hc.vec[0].bytes);
//	file2.close();
	////

	file1.close();

}

/**
 * Loads data from DataSource and fills bins
 */
void HistoCreator::createHistos() {
	writeZeros();
	IDataSource *ids = PreloadContainer::get().getDataSource(
			hc.myDataFile.c_str());
	// table of an event for each histogram
	unsigned int val[hc.vec.size()];
	for (int i = 0; i < hc.numOfEvents; ++i) {
		int l = 0;
		for (int k = 0; k < hc.vec.size(); ++k) {
			val[k] = ids->read(hc.vec[k].bytes); // TODO tu nie bangla
//			cout << "READ READ READ READ val["<<k<<"]: " << val[k] << endl;
//			cout << "ddcutsLow[k]: " << cutsLow[k] << endl;
//			cout << "ddcutsHigh[k]: " << cutsHigh[k] << endl;
			if (val[k] < cutsLow[k] or val[k] >= cutsHigh[k])
				l = hc.vec.size();
		}
//		cout << "DUUUUUUUUUPA0 hc.vec.size(): " << hc.vec.size() << endl;
//		cout << "DUUUUUUUUUPA0 l: " << l << endl;
		for (; l < hc.vec.size(); ++l) {
			//cout << "DUUUUUUUUUPA histos["<<l<<"]["<<val[l]<<"]: " << histos[l][val[l]] << endl;
			histos[l][val[l]]++;
		}
	}
}
void HistoCreator::createHistosOld() {
	fstream file1(hc.myDataFile.c_str(), fstream::in | fstream::binary);
	unsigned int val[hc.vec.size()];
	for (int i = 0; i < hc.numOfEvents; ++i) {

		for (int k = 0; k < hc.vec.size(); ++k) {
			val[k] = 0;
			file1.read((char*) (val + k), hc.vec[k].bytes);
			histos[k][val[k]]++;
		}
	}
}
/**
 * Writes zeros to all histograms' bins
 */
void HistoCreator::writeZeros() {
	for (auto &vec : histos)
		for (auto &v : vec)
			v = 0;
}
/**
 * Automatic testing function.
 * Loads data from ROOT tree, then processes the ROOT file again saving it to multiplexed binary file, then
 * loads multiplexed data with createHistos() and checks whether got the same bin values in all the histograms
 */
void HistoCreator::runTests() {
	for (auto h : histos)
		for (int i = 0; i < h.size(); ++i)
			h[i] = 0;

	vector<vector<unsigned int> > histos_copy;
	for (int i = 0; i < hc.vec.size(); ++i) {
		histos_copy.push_back(vector<unsigned int>(hc.vec[i].bins));
	}

	TFile * file = new TFile(hc.rootDataFile.c_str());
	TTree* tree = (TTree*) file->Get(hc.treeName.c_str());
	tree->GetEntries();
	float val[32];
	for (int i = 0; i < 32; ++i)
		val[i] = 0;
	tree->SetBranchAddress(hc.branchName.c_str(), val);
	for (int i = 0; i < hc.numOfEvents; ++i) {
		tree->GetEntry(i);
		bool isOk = true;
		for (int k = 0; k < hc.vec.size(); ++k) {
			unsigned int bin = hc.getBin(k, val[k]);
			if (bin < cutsLow[k] or bin >= cutsHigh[k]) {
//				cout << "bin: " << bin << endl;
//				cout << "cutsLow[k]: " << cutsLow[k] << endl;
//				cout << "cutsHigh[k]: " << cutsHigh[k] << endl;
				isOk = false;
			}
		}
		// if one event is selected out from any histogram, don't add this event to any histogram
		if (!isOk)
			continue;
		for (int k = 0; k < hc.vec.size(); ++k) {
			//cout << "val["<<k<<"]: " << val[k] << endl;
			unsigned int j = hc.getBin(k, val[k]);
			if (j != UINT_MAX) {
				histos_copy[k][j]++;
			} else {
				//cout << " j == UINT_MAX, k: " << k << " j: "<<j<<endl;
			}
		}
	}
	processTree();
	createHistos();
	int errors = 0;
	for (int i = 0; i < histos.size(); ++i)
		for (int j = 0; j < histos[i].size(); ++j) {
			errors += histos[i][j] != histos_copy[i][j];
			if (histos[i][j] != histos_copy[i][j]) {
//				cout << "histos["<<i<<"]["<<j<<"] != histos_copy["<<i<<"]["<<j<<"]: "
//						<<histos[i][j]<<"!="<<histos_copy[i][j]<<endl;//TODO
			} else {
//				cout << "histos["<<i<<"]["<<j<<"] == histos_copy["<<i<<"]["<<j<<"]: "
//						<<histos[i][j]<<"=="<<histos_copy[i][j]<<endl;//TODO
			}
		}
	cout << "Errors: " << errors << "." << endl;
	if (errors == 0)
		cout << " Everything is fine." << endl;
	else
		cerr << "What a terrible failure" << endl;

//	makeTestTree();// TODO wywal
}

