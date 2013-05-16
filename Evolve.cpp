//  this file defines all population-level functions
#include "Evolve.h"

using namespace std;

//  global multiplier that controls magnitude of selective pressures
int * PRESSURE_MAGNITUDE = new int;

//  construct a world simulation
world::world(varData worldSettings)
{
    //  save set variable ranges to private struct within the class
    mWorldSettings = worldSettings;

    *PRESSURE_MAGNITUDE = worldSettings.mSelectivePressureMagnitude;

    createOutputFile();

    outputSettings();
}

void world::createOutputFile()
{
    //  variables for time, and file address
    time_t rawtime;
    struct tm * timeinfo;

    //	The 35 is the max length of the fileName in chars
    char  fileName [35];
    char  GraphicFileName [35];
    size_t pos;
    string filePath;
    string GraphicFilePath;

    //	get the current time
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    //  save the prefix "Sim - ", with time and date info to fileName, ended with ".txt"
    //  The 35 is the max length of the fileName in chars
    strftime (fileName, 35, "Sim - %d %b %Y %Hh%Mm%Ss.txt", timeinfo);
    strftime (GraphicFileName, 35, "Pic - %d %b %Y %Hh%Mm%Ss.txt", timeinfo);

    //	save the path of the executable file to the string: directory
    string directory (getPath());
    /*
    //  find "Clock Evolution" in the path of the exe
    pos = directory.find("Clock Evolution\\");

    //  if the folder is found, move the position to the end of the directory name
    if (pos != string::npos)
    {
        //  move the position to the end of "Clock Evolution\" in the string
        //	The 16 is the length of "Clock Evolution\" in characters
        pos += 16;
    }
    else
    {*/
        //	if the executable isn't under the "Clock Evolution" directory, the simulation folder and files will be in the same directory as the exe
        pos = directory.size() - 19;//directory.find_last_of("/\\");
    //}

    //	resize directory string to end at the Clock Evolution folder, or the folder in which the exe's stored
    directory.resize(pos);

    //	add "Simulations" as a subdirectory to which the simulation details will be stored
    //directory += "Simulations\\";

    //  add the directory  filename to the end of the path, completing the save location
    filePath = directory + fileName;
    GraphicFilePath = directory + GraphicFileName;

    //	this code makes the "Simulations" directory if it is not already there, if it already exists, nothing happens
    //mkdir(directory.c_str());

    //	once the folder has been created, the file is opened
    fout.open (filePath.c_str(), fstream::out);
    gout.open (GraphicFilePath.c_str(), fstream::out);

    //	if file once again doesn't open, something has gone wrong, so alert user of the situation and provide option to quit or continue
    if (!fout.is_open() || !gout.is_open())
    {
        cout << endl << "A simulation record could not be created." << endl;
        cout << endl << "To quit, press the 'x' in the top right corner of this window to exit." << endl;
        system("pause");
    }
    //	if the file was successfully opened
    else
    {
        //	save the file location so that it can be output to the user at the end of a simulation
        mFileSaveLoc = filePath;
    }
}

//  create the vector of clocks
void world::initClocks()
{
    //  create each clock and insert it into the vector
    for (int i = 0; i < mWorldSettings.mPopulationSize; i++)
    {
        bioClock tempClock(mWorldSettings.mGenomeSize);
        tempClock.setMutationRate(mWorldSettings.mMutationRate);
        mPopulation.push_back(tempClock);
    }
}

void world::mateClocks()
{
    //  create a random number generator
    MTRand randGen;
    const int CLOCKS_TO_COMPETE = 3;
    const int SRC_1 = 0;
    const int SRC_2 = 1;
    const int LEAST_ACC = 2;

    //  store indexes and accuracies of relevant clocks
    int clockIndexes[CLOCKS_TO_COMPETE];
    double clockScores[CLOCKS_TO_COMPETE];

    //  temp variables for swapping while sorting
    int tempIndex;
    double tempScore;

    //  stores selected clocks, initialized to a value out of array
    int selectedClocks[CLOCKS_TO_COMPETE - 1] = {-1,-1};
    int randClock = 0;

    //  run through each generation
    for (int x = 0; x < mWorldSettings.mNumGenerations; x++)
    {
        //  a generation is defined by x matings in a population of x clocks
        for (int y = 0; y < mWorldSettings.mPopulationSize; y++)
        {
            for (int i = 0; i < CLOCKS_TO_COMPETE; i++)
            {
                do
                {
                    randClock = randGen.randInt(mPopulation.size() - 1);
                }
                while (selectedClocks[0] == randClock || selectedClocks[1] == randClock);

                if (i != CLOCKS_TO_COMPETE)
                    selectedClocks[i] = randClock;

                // 	randomly choose a clock index from the population
                clockIndexes[i] = randClock;
                // 	retrieve the score of the clock
                clockScores[i] =  mPopulation[clockIndexes[i]].calcSurvivalScore();
            }

            //  sort clocks by survival score
            for (int j = 0; j < CLOCKS_TO_COMPETE; j++)
            {
                for (int k = j; k < CLOCKS_TO_COMPETE; k++)
                {
                    // 	ensure that the index goes from greatest to least accurate
                    if (clockScores[j] < clockScores[k])
                    {
                        //  if it doesn't, swap the accuracy and index reference
                        tempScore = clockScores[k];
                        clockScores[k] = clockScores[j];
                        clockScores[j] = tempScore;

                        tempIndex = clockIndexes[k];
                        clockIndexes[k] = clockIndexes[j];
                        clockIndexes[j] = tempIndex;
                    }
                }
            }

            //  rewrite third (least accurate) clock using source data from two other ones (the parents)
            bioClock childClock (mPopulation[clockIndexes[SRC_1]], mPopulation[clockIndexes[SRC_2]]);
            mPopulation[clockIndexes[LEAST_ACC]] = childClock;
        }
        recordGeneration();
        cout << endl << "This is generation " << x + 1 << endl;
        outputGenAverages();
    }

    //	print the location of the saved file to the screen
    cout << "The file containing the details and settings of this simulation is located in: " << mFileSaveLoc << endl;
}

void world::recordGeneration()
{
    //  clear the screen
    system("cls");

    //  choose a random clock from within the population
    MTRand randGen;
    int randNum = randGen.randInt((int)mWorldSettings.mPopulationSize - 1);

    //  for each row in the genome
    for (int x = 0; x < mWorldSettings.mGenomeSize; x++)
    {
        //  for each column in the genome
        for (int y = 0; y < mWorldSettings.mGenomeSize; y++)
        {
            //  get the piece genome data
            clockPiece outputPiece = mPopulation[randNum].getClockPiece(x, y);
            outputPiece.getPieceType();

            //  output genome layout to screen, replacing numeric values with letters which represent the part type
            if (outputPiece.getPieceType() != PTYPE_NULL)
            {
                if (outputPiece.getPieceType() == PTYPE_ESCAPEMENT)
                {
                    cout << "E";
                    gout << "E";
                }
                else if (outputPiece.getPieceType() == PTYPE_GEAR)
                {
                    cout << "G";
                    gout << "G";
                }
                else if (outputPiece.getPieceType() == PTYPE_PENDULUM)
                {
                    cout << "P";
                    gout << "P";
                }
                else if (outputPiece.getPieceType() == PTYPE_MAINSPRING)
                {
                    cout << "M";
                    gout << "M";
                }
                else if (outputPiece.getPieceType() == PTYPE_HAND)
                {
                    cout << "H";
                    gout << "H";
                }
            }
            else
            {
                cout << " ";
                gout << " ";
            }
        }
        cout << endl;
        gout << endl;
    }

	//	output clock data
    mPopulation[randNum].calcSurvivalScore(true);
    gout << endl;
}

//  This function was taken from http://www.hardforum.com/showthread.php?t=973922 from Shadow2531
//  The path of the exe file was found and returned as text
string getPath()
{
    TCHAR path[2048] = {0};
    GetModuleFileName(NULL, path, 2048);
    const string exe_path(path);

    return exe_path;
}

void world::outputGenAverages()
{

    // set precision to 5 decimal points and create a blank line
   fout << setprecision(5);

    //  declare average counters
    double bestPend = 0, secGearInterval = 0, minGearInterval = 0, hrGearInterval = 0,
    avgNotNullPieces = 0, secGearHand = 0, minGearHand = 0, hrGearHand = 0, avgSurvivalScore = 0,
    numDeadClocks = 0, numPendClocks = 0, numGearClocks = 0, num1HClocks = 0, num2HClocks = 0, num3HClocks = 0;

    for (int x = 0; x < mWorldSettings.mPopulationSize; x++)
    {
        //  for each clock, recalculate score and add up averages
        avgSurvivalScore += mPopulation[x].calcSurvivalScore();
        if (mPopulation[x].getSurvivalScore() != 0)
        {
            bestPend += mPopulation[x].getBestPendulum().getPieceInterval() ;
            secGearInterval+= mPopulation[x].getTimeGear(INDEX_SEC).getPieceInterval();
            minGearInterval+= mPopulation[x].getTimeGear(INDEX_MIN).getPieceInterval();
            hrGearInterval+= mPopulation[x].getTimeGear(INDEX_HR).getPieceInterval();
            secGearHand += mPopulation[x].getTimeGear(INDEX_SEC).getIsAttToHand();
            minGearHand += mPopulation[x].getTimeGear(INDEX_MIN).getIsAttToHand();
            hrGearHand+= mPopulation[x].getTimeGear(INDEX_HR).getIsAttToHand();
            avgNotNullPieces += mPopulation[x].getNotNullPieces();
        }
        numDeadClocks += (mPopulation[x].getSurvivalScore() == 0);

        //  add to whatever sort of clock this is (3h, 2h, 1h, gearTrain, pend)
        if (mPopulation[x].getNumHands() == 3)
            num3HClocks++;
        else if (mPopulation[x].getNumHands() == 2)
            num2HClocks++;
        else if (mPopulation[x].getNumHands() == 1)
            num1HClocks++;
        else if (mPopulation[x].hasGearTrain())
            numGearClocks++;
        else if (mPopulation[x].getBestPendulum().getPieceInterval() != 0)
            numPendClocks++;

    }

    //  divide out all of the averages
    avgSurvivalScore /= mWorldSettings.mPopulationSize - numDeadClocks;
    bestPend /= mWorldSettings.mPopulationSize - numDeadClocks;
    secGearInterval /= mWorldSettings.mPopulationSize - numDeadClocks;
    minGearInterval /= mWorldSettings.mPopulationSize - numDeadClocks;
    hrGearInterval /= mWorldSettings.mPopulationSize - numDeadClocks;
    secGearHand /= mWorldSettings.mPopulationSize - numDeadClocks;
    minGearHand /= mWorldSettings.mPopulationSize - numDeadClocks;
    hrGearHand /= mWorldSettings.mPopulationSize - numDeadClocks;
    avgNotNullPieces /= mWorldSettings.mPopulationSize - numDeadClocks;

    /*
    //  output all of the survival scores
    for (int i = 0; i < mWorldSettings.mPopulationSize; i++)
	{
	    if (mPopulation[i].calcSurvivalScore() != 0)
	    {
            fout << (double)mPopulation[i].getSurvivalScore();

            if (i != mWorldSettings.mPopulationSize - 1)
                fout << ",";
	    }
	}
    */

    fout << bestPend << "," << secGearInterval << "," << minGearInterval << "," << hrGearInterval << "," << secGearHand << "," << minGearHand << ","
	<< hrGearHand << "," << avgNotNullPieces << "," << avgSurvivalScore << "," << numDeadClocks << "," << numPendClocks << "," << numGearClocks << ","
	<< num1HClocks << "," << num2HClocks << "," << num3HClocks;

	fout << endl;
}

void world::outputSettings()
{
    fout << "Genome Size: " << mWorldSettings.mGenomeSize << "," << "Mutation Rate: " << mWorldSettings.mMutationRate << "," << "Number of Generations: " << mWorldSettings.mNumGenerations << "," << "Population Size: " << mWorldSettings.mPopulationSize << "," << "SPM: " << mWorldSettings.mSelectivePressureMagnitude << endl;
    //fout << "Genome Size" << "," << "Mutation Rate" << "," << "Number of Generations" << "," << "Population Size" << "," << "SPM" << endl;
    //fout << mWorldSettings.mGenomeSize << "," << mWorldSettings.mMutationRate << "," << mWorldSettings.mNumGenerations << "," << mWorldSettings.mPopulationSize << "," << mWorldSettings.mSelectivePressureMagnitude << endl;
}
