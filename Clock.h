//  this file defines the prototypes for all organism-level algorithms
//  in this file clockPiece and bioClock are defined

#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

//  vector for dynamic arrays, and mersenne for random number generation
#include <vector>

//	used for random number generation in piece and clock initializations
#include "MersenneTwister.h"

//	need for setprecision()
#include <iomanip>

using namespace std;

//  constants used for the clock part types
const int PTYPE_NULL = 0;
const int PTYPE_ESCAPEMENT = 1;
const int PTYPE_MAINSPRING = 2;
const int PTYPE_GEAR = 3;
const int PTYPE_HAND = 4;
const int PTYPE_PENDULUM = 5;

//  the amount of different part types
const int PTYPE_AMT = 5;

//  max teeth for gears
const int MAX_TEETH = 50000;

//  time constants for gears/pendulums
const int TIME_INTERVAL[] = {60, 3600, 43200};
const int INDEX_SEC = 0;
const int INDEX_MIN = 1;
const int INDEX_HR = 2;

// score limits
const double MAX_SCORE = 1000000;
const double MIN_SCORE = 0.000001;

//  clockPiece class defines the structure of each clock component. It also represents a 'gene' that fits in the clock genome
class clockPiece
{
private:

	//  type of component (defined by P_TYPE)
	int mPieceType;

	//  pendulum length
	double mPendulumLength;

	// 	interval of piece (period for pendulums, time for rotation for gears)
	double mPieceInterval;

	// 	number of teeth for gears
	int mNumTeeth;

	// if a piece is powered (only used for gears)
	bool mIsPowered;

	//  if a piece is attached to a hand (only used for gears)
	bool mIsAttToHand;

public:

	//  Used for detecting if a clock is in one piece (all connected together)
	bool mIsConnected;

	//  getters
	int getPieceType(){return mPieceType;};

	double getPendulumLength(){return mPendulumLength;};

	double getPieceInterval(){return mPieceInterval;};

	int getNumTeeth(){return mNumTeeth;};

	bool getIsPowered(){return mIsPowered;};

	bool getIsAttToHand(){return mIsAttToHand;};

	// 	setters
	void setPieceInterval (double pieceInterval){mPieceInterval = pieceInterval;};

	void setIsPowered (bool isPowered){mIsPowered = isPowered;};

	void setIsAttToHand (bool isAttToHand){mIsAttToHand = isAttToHand;};

	//  a constructor randomizes the variables.
	clockPiece();
};

//  this class represents a clock as an organism, and has a vector of clockPieces representing its genome
class bioClock
{
private:

	//  survival score stores how fit the clock is (higher = better)
	double mSurvivalScore;

	// 	the user can set the genome size before the simulation
	int mGenomeSize;

	//  percent chance of mutation
	double mMutationRate;

	//  stores the component/genetic data for the clock (i.e. its genome)
	vector < vector <clockPiece> > mClockGenome;

	//  stores which gears are in a gear train
	vector <clockPiece> mGearTrain;

	//  stores best pendulum in clock
	clockPiece mBestPendulum;

	//  stores the second, minute, and hour gears (3 in total)
	clockPiece mGear[3];

    //  stores the number of (working) hands
    int mNumHands;

    //	stores number of not null pieces
    int mNotNullPieces;

	//  checks connections of clock piece by piece with recursion
	void checkPieceConn (int x, int y);

	//  checks functionality of pendulums
	double checkPendulum (int x, int y);

    //  check if a pendulum is really regulating a train of gears
    //  used to make sure irrelevant pendulums don't override regulating ones
	bool isPendOnTrain (int x, int y);

	//  checks positioning of escapements and calculates interval of attached gear
	bool checkEscapement(int x, int y);

	//	checks recursively to analyze gear intervals and connectivity to other gears, springs, and hands
	void calcGearInfo(int x, int y, double attPendInterval);

	//  checks to see if a spring or hand is bound to exactly one gear
	bool checkMainspringOrHand(int x, int y);

	//  output clock info
	void outputClockInfo();

public:

	//  constructs a clock randomly or with parents
	bioClock (int genomeSize);
	bioClock (bioClock source1, bioClock source2);

	//  evalutates functionality and accuracy
	double calcSurvivalScore(bool output = false);

	//  setters
	void setMutationRate(double mutationRate){mMutationRate = mutationRate;};

	// getters
	clockPiece getClockPiece(int x, int y){return mClockGenome[x][y];};
	clockPiece getBestPendulum() {return mBestPendulum;};
	clockPiece getTimeGear(int index) {return mGear[index];};
	int getNotNullPieces() {return mNotNullPieces;};
	double getSurvivalScore() {return mSurvivalScore;};
	int getNumHands() {return mNumHands;};
	bool hasGearTrain() {return (mGear[INDEX_SEC].getPieceInterval() > 0 || mGear[INDEX_MIN].getPieceInterval() > 0 || mGear[INDEX_HR].getPieceInterval() > 0);};
};

double scoreDiff(double num1, double num2);
#endif // CLOCK_H_INCLUDED
