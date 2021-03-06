/**
 @file    HistoConfig.h
 @brief boost::program_options powered, JSON based options container.
 @author   kzawisto
 @created:   2015-10-29
 */

#ifndef INCLUDE_HISTOCONFIG_H_
#define INCLUDE_HISTOCONFIG_H_

#define OUT_OF_BOUND_LEFT -1
#define OUT_OF_BOUND_RIGHT -2

#include <string>
#include <vector>
#include <cmath>
#include <boost/property_tree/ptree.hpp>
using std::string;
using std::vector;
typedef short unsigned int SUINT;
using namespace boost::property_tree;
///A configuration container for histograms created from one ROOT branch (as series of events).

class HistogramConfig {
public:
	///number of histograms in given binary file/ROOT file
	int numOfHistos;
	///number of events to load
	int numOfEvents;
	///length of a given event in bytes in muptiplexed data file (all entries together)
	int multiplexedByteLenOfEvent;
	string treeName, branchName, rootDataFile, myDataFile;
	// Basic single histogram config aggregate, contains its name,
	// value range, byte length (log_128(bins)) and no. of events
	struct MyHisto {
		string name;
		int bins, bytes, eventNum;
		float min, max;
		MyHisto(string n, int bns, int bts, int en, float mn, float mx)
		: name(n), bins(bns), bytes(bts), eventNum(en), min(mn), max(mx) {}
	};
	///vector of histogram config data aggregates
	vector<MyHisto> vec;
//	HistogramConfig(string path_to_json);
	HistogramConfig(ptree pt);
	int getBin(int histo, float value);
	int noOfBytes(int i);
};
/// Returns how much bytes do we need to save given unsigned int
inline int HistogramConfig::noOfBytes(int i) {
	return (log(i) / log(2.0) + 7.9999999) / 8;
}

#endif /* INCLUDE_HISTOCONFIG_H_ */
