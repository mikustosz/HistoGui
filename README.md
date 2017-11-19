# CMS lab
### A graphical tool for particle physics data analysis.

Given series of events in ROOT data file, the program allows to plot histograms and apply cutoffs to each histogram with wxWidgets based GUI. 

Data are stored in multiplexed binary files, i.e. one needs to create such file from ROOT branch and JSON configuration file, employing provided command line tool. The GUI application uses the very same JSON config, when loading binary data.

Example JSON configuration file and ROOT file data from CMS experiment and Monte Carlo simulations are provided in DataIO folder. JSON contains an arbitrary number of first level nodes (in the example there are two called "dat_file1", "dat_file2"). 
Below we have "option" node, comprising self-evident description of our files structure i.e. ROOT file, tree, and branch names, number of events to process and save in binary file,and binary file name. Second node at this level, "histos" contains the array of histogram descriptions, comprised of histogram name, the number of bins, and the range of values one can put in the histogram. In this case t all the range is redundant for now, since multiplexed values are equal to specific number of bins, but it makes the GUI application more universal.


## Installation
### Requirements
-  **wxWidgets** library must be installed in version >= **2.9**
-  **ROOT** data analysis library installed

### Preprocessing
To generate binary datafiles one needs to build ParseTree program in DataIO folder, specifying path to the ROOT and libraries:
```
export ROOTSYS=/pathToTheROOT/root/
export LD_LIBRARY_PATH=$ROOTSYS/lib:$PWD/lib
```

To build the ParseTree program just run:
```
make 
``` 
To run the program one may also need to provide the path to the installed libraries like wxWidgets eg.:
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```
in DataIO directory, provide config.json file and run the tool:
```
./ParseTree
```
### GUI
Then to run the GUI one needs to build data access shared library and the GUI itself. To do this enter the HistogramGui folder and execute:
```
make
```
From DataIO folder copy config.json and generated binary data files to HistogramGui directory.
Finally, run generated executable:
```
HistogramGui/CMSlab
```

### Summary
To run the app from DataIO folder following commands may be used:
```
make
./ParseTree
cp config.json bindata_*  ../HistogramGui/ #put here names of binary files set in config.json
cd ../HistogramGui/
make
./CMSlab
```