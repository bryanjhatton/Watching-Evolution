//  this file contains the interface and output algorithms

#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

using namespace std;

//	holds world environmental data
struct varData
{
    //  constructor
    varData();

    //  holds number of times to run simulation
    int mSimTimes;

    //  the population can be REALLY BIG if needed
    double mPopulationSize;

    // 	evolution testing is supported up to A LOT of generations
    double mNumGenerations;

    //  this variable holds the 'mutationPercent' chance of a mutation occuring in clock reproduction
    double mMutationRate;

    //  square root of the size of the genome matrix (i.e. size six means a size 36 genome)
    int mGenomeSize;

    //  controls magnitude of selective pressures
    int mSelectivePressureMagnitude;
};

//	more CLI funtions
int stringTOint (string text);
double stringTOdouble (string text);
void writeSettingHelp (string setting, string settingDetails, string helpDetails);
varData runCLI ();
string getSetting (string entry, string command, string prevSetting);

#endif // INTERFACE_H_INCLUDED
