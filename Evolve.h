//  this file contains the world class, and population-level algorithms

#ifndef EVOLVE_H_INCLUDED
#define EVOLVE_H_INCLUDED

#include "Clock.h"
#include "Interface.h"
#include <fstream>
#include <direct.h>
#include <shlwapi.h>

using namespace std;

//  class to run the test instance
class world
{
private:

	//	stores the population of clocks
    vector<bioClock> mPopulation;

    //  worldSettings contains the starting population size, mutation rate, and other global data
    varData mWorldSettings;

    //	mFileSaveLoc contains the location to which the file that the simulation details has been saved
    string mFileSaveLoc;

    void outputGenAverages();

public:

    //  the constructor is built based on the user entered restrictions
    world (varData worldSettings);

    //  create the first generation of clocks randomly
    void initClocks();

    //  this is the algorithm used to 'mate' clocks
    void mateClocks();

    //  records generation data to file
    void recordGeneration();

    //  the file that the generation data will be output to
    ofstream fout;
    ofstream gout;

    //  create and open file to output to
    void createOutputFile();

    //  print simulation setting to file
    void outputSettings();
};

//  the details for this function were found at http://www.hardforum.com/showthread.php?t=973922 from Shadow2531 and gets the path of the executable file
string getPath();

#endif // EVOLVE_H_INCLUDED
