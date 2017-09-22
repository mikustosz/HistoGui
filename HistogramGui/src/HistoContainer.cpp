/**
 @file HistoContainer.cpp
 @brief Implementation of container for HistoCreators
 @author   kzawisto
 @created:   2015-11-04
 */

#include <MyHistogramWrapper.h>
#include "HistoContainer.h"
#include "HistoCreator.h"
#include <cstdlib>
#include <sys/stat.h>
#include <fstream>
/// to test whether config_json exists
bool fileExists(const std::string & name) {
	ifstream f(name.c_str());
	bool ret = (f.good());
	f.close();
	return ret;

}
HistoContainer::HistoContainer(string json_path) {
	ptree pt;

	if (!fileExists(json_path)) {
		cout << "ERROR: No such file: " << json_path << "\n";
		cout << "Current working dir is" << endl;
		system("pwd");
		good = false;
	} else {
		good = true;
		read_json(json_path, pt);
		for (auto & p : pt) {
			if (p.first.substr(0, 3) == "dat")
				creators.push_back(new HistoCreator(p.second));
		}
	}
}

vector <vector<HistoGraph> > HistoContainer::buildGuiHistos() {
	vector <vector<HistoGraph> > histos(2);
	int cNum = 0;
	for (auto c : creators) {
		if (cNum > 1) {
			cout << "Wrong number of files, there should only be 1 or 2\n";
			exit(1);
		}
		auto it = c->histos.begin();
		int i = 0;
		for (auto &h : c->hc.vec) {
			histos[cNum].push_back(HistoGraph(h.bins, h.min, h.max));
			histos[cNum].back().histo.title = h.name;
			histos[cNum].back().histo.bins = &(*it);
			histos[cNum].back().histo.creator = &(*c);
			histos[cNum].back().histo.histoId = i;
			histos[cNum].back().histo.eventNum = h.eventNum;
			it++;
			i++;
		}
		cNum++;
	}

	return histos;
}
void HistoContainer::createLogicalHistos() {
	for (auto c : creators)
		c->createHistos();
}
HistoContainer::~HistoContainer() {
	for (int i = 0; i < creators.size(); ++i)
		delete creators[i];
}
