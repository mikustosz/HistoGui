/**
 @file    HistoConfig.cpp
 @brief Implementation for HistogramConfig
 @author   kzawisto
 @created:   2015-10-29
 */

#include "HistoConfig.h"

#include<boost/property_tree/json_parser.hpp>
#include <cstdlib>
#include <iosfwd>
#include <iostream>
using namespace std;

/**
 * Constructor loads config stuff from boost::program_options ptree (json initialized), i.e.
 * ROOT data config (number of events to load, file name, tree name, branch name), multiplexed data file name
 * (location of the existing or a path to write a new one from ROOT tree.
 */
HistogramConfig::HistogramConfig(ptree pt) {
	ptree histos = pt.get_child("histos");
	ptree options = pt.get_child("options");
	numOfEvents = options.get<int>("numOfEvents");
	treeName = options.get<string>("treeName");
	branchName = options.get<string>("branchName");
	rootDataFile = options.get<string>("rootDataFile");
	myDataFile = options.get<string>("myDataFile");
	multiplexedByteLenOfEvent = 0;
	for (auto h : histos) {
		int bins = h.second.get<int>("bins");
		int bytes = noOfBytes(bins);
		multiplexedByteLenOfEvent += bytes;
		vec.emplace_back(
			h.second.get <string> ("name"),
			bins,
			bytes,
			numOfEvents,
			h.second.get<float>("min"),
			h.second.get<float>("max")
		);
	}
	numOfHistos = vec.size();
}
/*
 * For a given value retrieve a bin number it should be added to, for a given histogram.
 */
int HistogramConfig::getBin(int histo, float value) {
	long int b = (long int) ((value - vec[histo].min)
			/ (vec[histo].max - vec[histo].min) * vec[histo].bins);
	if (b < 0) {
		b = 0;
	} else if (b >= vec[histo].bins) {
		b = vec[histo].bins-1;
	}
	return b;
}

