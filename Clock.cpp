//  this file defines what all of the organism-level functions do
#include "Clock.h"

extern int * PRESSURE_MAGNITUDE;

//  constructs a random clockpiece
clockPiece::clockPiece()
{
    //  creates an MTRand that can make random numbers
    MTRand randGen;

    //  initialize data
    mPieceType = randGen.randInt(PTYPE_AMT);
    mPieceInterval = 0;
    mPendulumLength = 0;
    mNumTeeth = 0;
    mIsConnected = false;
    mIsPowered = false;
    mIsAttToHand = false;

    //  determine other member variables based on the part type
    if (mPieceType == PTYPE_PENDULUM)
		//	random length of pendulum between 0 and 1 meter
        mPendulumLength = randGen.rand();
    else if (mPieceType == PTYPE_GEAR)
		//	random number of gear teeth (physical minimum is 3)
        mNumTeeth = randGen.randInt(MAX_TEETH - 3) + 3;
}

//  constructor randomly generates the clock genome
bioClock::bioClock(int genomeSize)
{
    mGenomeSize = genomeSize;
    mClockGenome.resize(mGenomeSize);

    //  initialize survival score
    mSurvivalScore = 0;

    //  initialize number of working hands
    mNumHands = 0;

    //  fill the genome matrix with randomly constructed clock pieces
    for (int i = 0; i < mGenomeSize; i++)
    {
        mClockGenome[i].resize(mGenomeSize);
        for (int j = 0; j < mGenomeSize; j++)
        {
            clockPiece tempPiece;
            mClockGenome[i][j] = tempPiece;
        }
    }
}

// 	a clock can be intialized with source data (parents)
bioClock::bioClock (bioClock source1, bioClock source2)
{
    //  creates an MTRand that can make random numbers
    MTRand randGen;
    double randNum;
    double remainingPercent;

    //  initialize survival score
    mSurvivalScore = 0;

    //  initialize number of working hands
    mNumHands = 0;

    //  get the genome size from one of the parents (doesn't really matter which one)
    mGenomeSize = source1.mGenomeSize;
    mClockGenome.resize(mGenomeSize);

    //  get mutation rate from a parent (doesn't matter which one)
    mMutationRate = source1.mMutationRate;

    //  fill genome with either a random (mutated) gene, or an equal chance of a mother's or father's gene
    for (int i = 0; i < mGenomeSize; i++)
    {
        mClockGenome[i].resize(mGenomeSize);
        for (int j = 0; j < mGenomeSize; j++)
        {
            //  random real number from 0-1
            randNum = randGen.rand();

            //  calculate remaining percent left after accounting for mutation rate
            remainingPercent = 1 - (mMutationRate / 100);

            //  create a random piece upon mutation
            if (randNum > remainingPercent)
            {
                clockPiece tempPiece;
                mClockGenome[i][j] = tempPiece;
            }
            //  otherwise it's split 50/50 for inheritance of traits from mother or father
            //  copy the piece from parent
            else if (randNum > (remainingPercent / 2))
                mClockGenome[i][j] = source1.mClockGenome[i][j];
            else
                mClockGenome[i][j] = source2.mClockGenome[i][j];

            //  reset connection status of copied piece
            mClockGenome[i][j].mIsConnected = false;

            //  reset more data for gears
            mClockGenome[i][j].setIsPowered(false);
            mClockGenome[i][j].setIsAttToHand(false);
            mClockGenome[i][j].setPieceInterval(0);
        }
    }
}

//  calculates and returns the clock's survival score
double bioClock::calcSurvivalScore(bool output)
{
    //  counters
    int i, j, k;
    int connectedPieces = 0;
    int notNullPieces = 0;

    //  stores if pendulums are conflicting in a gear train
    bool pendConflict = false;

    //  if the best pendulum is attached to a gear train
    bool bestPendOnTrain = false;

    //  score multiplier
    const double SCORE_MULTIPLIER = *PRESSURE_MAGNITUDE;

    //  number of times to analyze the clock in loops
    const int TIMES_TO_SCAN = 3;

    //  breaks out of nested loops
    bool isBroken = false;

    //  the calculated score
    double returnScore = 0.;

    //  other scores (they add up to returnScore)
    double pendScore = 0.;
    double totalGearScore = 0.;
    double gearScore[] = {0, 0, 0};

    //	if clock's gear train is powered by a spring
    bool isTrainPowered = false;

    //  reset all piece intervals
    for (i = 0; i < mGenomeSize; i++)
        for (j = 0; j < mGenomeSize; j++)
            mClockGenome[i][j].setPieceInterval(0);


    //  check which pieces are connected to the 'main island' of pieces
    //  if they aren't all attached to each other, then the clock is 'broken'
    for (i = 0; i < mGenomeSize && !isBroken; i++)
    {
        for (j = 0; j < mGenomeSize; j++)
        {
            //  once a non-empty piece is found, start checking from there and break the search
            if (mClockGenome[i][j].getPieceType() != PTYPE_NULL)
            {
                checkPieceConn(i, j);
                isBroken = true;
                break;
            }
        }
    }

    //  analyze the clock in stages
    for (k = 1; k <= TIMES_TO_SCAN; k++)
    {
        //  reset 'isBroken'
        isBroken = false;

        for (i = 0; i < mGenomeSize && !isBroken; i++)
        {
            for (j = 0; j < mGenomeSize; j++)
            {
                //  on first scanning, count up total pieces and total connected pieces
                if (k == 1)
                {
                    notNullPieces += (mClockGenome[i][j].getPieceType() != PTYPE_NULL);
                    connectedPieces += (mClockGenome[i][j].mIsConnected && mClockGenome[i][j].getPieceType() != PTYPE_NULL);
                }
                //  on second scanning, check all pendulums
                else if (k == 2)
                {
                    if (mClockGenome[i][j].getPieceType() == PTYPE_PENDULUM)
                    {
                        double currentPendScore = checkPendulum(i, j);

                        //  if two pendulums are on a train
                        pendConflict += (bestPendOnTrain && isPendOnTrain(i, j));

                        //  if the current best pendulum is not on a train
                        if (!bestPendOnTrain)
                        {
                            if (currentPendScore > pendScore)
                            {
                                //  store score of best pendulum only
                                pendScore = currentPendScore;

                                //  store the pendulum
                                mBestPendulum = mClockGenome[i][j];

                                //  check if the pendulum is on a train by checking attached escapement
                                bestPendOnTrain = isPendOnTrain(i, j);
                            }
                        }
                    }
                }
                //	on third scanning, analyze gear train
                else if (k == 3)
                {
                	if (mClockGenome[i][j].getPieceType() == PTYPE_GEAR && mClockGenome[i][j].getPieceInterval() != 0)
                	{
                		//	determine attached pendulum's length of period
                		double attPendInterval = mClockGenome[i][j].getPieceInterval() / (double)mClockGenome[i][j].getNumTeeth();

                        //  start calculating recursively
						calcGearInfo(i, j, attPendInterval);

						//  stop scanning the clock
						isBroken = true;
						break;
                	}
                }
            }
        }

        //  store the amount of not null pieces
        mNotNullPieces = notNullPieces;

        //  if the clock is divided, the clock is 'broken'
        if (notNullPieces != connectedPieces)
        {
            //  output info and stop calculating
            if (output)
                outputClockInfo();

            return 0;
        }
    }

    //  analyze gearTrain if there is one
    if (mGearTrain.size() > 0)
    {
        for (i = 0; i < (signed int)mGearTrain.size(); i++)
        {
            if (pendConflict)
            {
                //  if the pendulums were conflicting, the whole train breaks
                totalGearScore = 0;
                gearScore[INDEX_SEC] = 0;
                gearScore[INDEX_MIN] = 0;
                gearScore[INDEX_HR] = 0;
                mGearTrain.clear();

                break;
            }
            else
            {
                //  store if the train is powered by a spring at any point
                isTrainPowered += mGearTrain[i].getIsPowered();

                //  store the scores of all gears closest to each time interval
                for (j = 0; j < 3; j++)
                {
                    //  calculate current gear's score for an interval
                    double currentGearScore = scoreDiff(mGearTrain[i].getPieceInterval(), TIME_INTERVAL[j]);

                    // add a modifier if the gear has a hand attached
                    if (mGearTrain[i].getIsAttToHand())
                    {
                        currentGearScore *= SCORE_MULTIPLIER;
                    }

                    //  calculate gear if it's better than the current gear in this interval slot
                    //  also, ensure that gears with the same interval (or the same gear itself) can't correspond to multiple slots
                    if (currentGearScore > gearScore[j] && mGear[INDEX_SEC].getPieceInterval() != mGearTrain[i].getPieceInterval() && mGear[INDEX_MIN].getPieceInterval()
                     != mGearTrain[i].getPieceInterval() && mGear[INDEX_HR].getPieceInterval() != mGearTrain[i].getPieceInterval())
                    {
                        gearScore[j] = currentGearScore;

                        //  store the gear
                        mGear[j] = mGearTrain[i];
                    }
                }
            }
        }
    }

    //  store number of working hands
    mNumHands = mGear[INDEX_SEC].getIsAttToHand() + mGear[INDEX_MIN].getIsAttToHand() + mGear[INDEX_HR].getIsAttToHand();

    //  add up the total gear score
    totalGearScore = gearScore[INDEX_SEC] + gearScore[INDEX_MIN] + gearScore[INDEX_HR];

    //  if the train is powered, add a multiplier
    if (isTrainPowered)
        totalGearScore *= SCORE_MULTIPLIER;

    //  add multiplier as long as a gear train exists
    if (mGearTrain.size() > 0)
        totalGearScore *= SCORE_MULTIPLIER;

    //  factor in gear score
    returnScore += totalGearScore;

    //  factor in the best pendulum score
    returnScore += pendScore;

    //  having gears in a gear train is not detrimental to the clock, cancels out the subtraction of # of parts
    returnScore += (mGearTrain.size() / SCORE_MULTIPLIER);

    //  factor in the number of pieces (less = better)
    returnScore -= (notNullPieces / SCORE_MULTIPLIER);

	//	make sure the score doesn't go negative
    if (returnScore < 0)
        returnScore = 0;

    //  store the survival score
    mSurvivalScore = returnScore;

    //  output info if desired
    if (output)
        outputClockInfo();

    //  clear the gear train vector
    mGearTrain.clear();

    return returnScore;
}

//  checks if an individual piece is connected to 'main island' of pieces
void bioClock::checkPieceConn (int x, int y)
{
    //  connected to main body of the clock
    mClockGenome[x][y].mIsConnected = true;

    //  check in each direction
    for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
		{
			if ((i == 0) != (j == 0))
				//  make sure you're within boundaries
				if (x + i < mGenomeSize && x + i >= 0 && y + j < mGenomeSize && y + j >= 0)
					if (mClockGenome[x + i][y + j].getPieceType() != PTYPE_NULL && mClockGenome[x + i][y + j].mIsConnected == false)
						checkPieceConn(x + i, y + j);
		}
}


//  evaluates functionality of a pendulum and returns its score
double bioClock::checkPendulum(int x, int y)
{
    //  number of parts that the pendulum is connected to
    int numConnections = 0;

    double returnScore = 0;

    //  some constants needed for physics
    const double gravStrength = 9.81;
    const double pi = 3.14;

    //  check if the pendulum can swing freely
    //  do this by checking adjacent pieces
    for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
		{
			if ((i == 0) != (j == 0))
				//  make sure you're within boundaries
				if (x + i < mGenomeSize && x + i >= 0 && y + j < mGenomeSize && y + j >= 0)
				{
					if (mClockGenome[x + i][y + j].getPieceType() != PTYPE_NULL)
						numConnections++;
				}
		}

    //  stop here if the pendulum isn't connected to exactly one point
    if (numConnections != 1)
        return 0;
    else
    {
        //  calculate the period of the pendulum
        double pendPeriod = 2 * pi * sqrt(mClockGenome[x][y].getPendulumLength() / gravStrength);

        //  check how close the period is to 1 second
        returnScore = scoreDiff(pendPeriod, 1);

        //  store the pendulum's period
        mClockGenome[x][y].setPieceInterval(pendPeriod);
        return returnScore;
    }
}

//  evaluates functionality of an escapement, and calculates interval of attached gear
bool bioClock::checkEscapement(int x, int y)
{
    //  counters for detecting attached pieces
    int workingPends = 0;
    int gears = 0;
    int nullPieces = 0;

    //  coordinates of attached gear
    int attGearX = 0;
    int attGearY = 0;

    //  coordinates of attached pendulum
    int attPendX = 0;
    int attPendY = 0;

    //  check if the escapement is attached to one working pendulum and one gear only
    //  do this by checking adjacent pieces
    for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
		{
			if ((i == 0) != (j == 0))
			{
				//  make sure you're within boundaries
				if (x + i < mGenomeSize && x + i >= 0 && y + j < mGenomeSize && y + j >= 0)
				{
					//  check for null spaces
					nullPieces += (mClockGenome[x + i][y + j].getPieceType() == PTYPE_NULL);

					//  check for pendulums and record coordinates
					if (mClockGenome[x + i][y + j].getPieceType() == PTYPE_PENDULUM && mClockGenome[x + i][y + j].getPieceInterval() != 0)
					{
						workingPends++;
						attPendX = x + i;
						attPendY = y + j;
					}

					//  check for gears and record coordinates
					if (mClockGenome[x + i][y + j].getPieceType() == PTYPE_GEAR)
					{
						gears++;
						attGearX = x + i;
						attGearY = y + j;
					}
				}
			}
		}

    // if connected to EXACTLY one gear and one working pendulum and nothing else (two null spaces)
    if (workingPends == 1 && gears == 1 && nullPieces == 2)
    {
        double attPendInterval = mClockGenome[attPendX][attPendY].getPieceInterval();
        int attGearNumTeeth = mClockGenome[attGearX][attGearY].getNumTeeth();

        // calculate interval of attached gear
        mClockGenome[attGearX][attGearY].setPieceInterval(attGearNumTeeth * attPendInterval);

        return true;
    }

    return false;
}

//  calculate gear info and add gear to gear train vector
void bioClock::calcGearInfo(int x, int y, double attPendInterval)
{
    bool isGearPowered = false;
    bool isAttToHand = false;

    double gearInterval = 0;

    //counters
    int i, j;

    //	calculate this gear's interval
    gearInterval = (double)mClockGenome[x][y].getNumTeeth() * attPendInterval;

    //	find if gear is attached to a hand or mainspring
    for (i = -1; i <= 1; i++)
		for (j = -1; j <= 1; j++)
        	if ((i == 0) != (j == 0))
				//  make sure you're within boundaries
				if (x + i < mGenomeSize && x + i >= 0 && y + j < mGenomeSize && y + j >= 0)
				{
					//	check surrounding mainsprings and hands
					if (mClockGenome[x + i][y + j].getPieceType() == PTYPE_MAINSPRING && !isGearPowered)
						isGearPowered = checkMainspringOrHand(x + i, y + j);
					else if (mClockGenome[x + i][y + j].getPieceType() == PTYPE_HAND && !isAttToHand)
						isAttToHand = checkMainspringOrHand(x + i, y + j);
				}

	//  set if the gear is powered or not
	mClockGenome[x][y].setIsPowered(isGearPowered);

    //  set if the gear is attached to a hand or not
	mClockGenome[x][y].setIsAttToHand(isAttToHand);

    //  set the gear's interval
	mClockGenome[x][y].setPieceInterval(gearInterval);

	//  recur this function on adjacent gears
	for (i = -1; i <= 1; i++)
		for (j = -1; j <= 1; j++)
        	if ((i == 0) != (j == 0))
				//  make sure you're within boundaries
				if (x + i < mGenomeSize && x + i >= 0 && y + j < mGenomeSize && y + j >= 0)
                    if (mClockGenome[x + i][y + j].getPieceType() == PTYPE_GEAR && mClockGenome[x + i][y + j].getPieceInterval() == 0)
                        calcGearInfo(x + i, y + j, attPendInterval);

    mGearTrain.push_back(mClockGenome[x][y]);
}

bool bioClock::checkMainspringOrHand(int x, int y)
{
	int attPieceCount = 0;

	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
        	if ((i == 0) != (j == 0))
                //  check boundaries
                if (x + i < mGenomeSize && x + i >= 0 && y + j < mGenomeSize && y + j >= 0)
                    attPieceCount += (mClockGenome[x + i][y + j].getPieceType() != PTYPE_NULL);

	//	the mainspring/hand works when attached to only one piece
	//	since this function is only called when the spring/hand is detected to be next to a gear
	//	there is no need to verify what the one attached piece is
	return (attPieceCount == 1);
}

//  returns a higher number the closer the compared numbers are in value
double scoreDiff(double num1, double num2)
{
    double difference = num1 - num2;
    double retVal;

    // 	always keep score positive
    if (difference < 0)
        difference = -difference;

    if (difference < MIN_SCORE)
        retVal = MAX_SCORE;
    else
        retVal = 1 / difference;

    return retVal;
}

void bioClock::outputClockInfo()
{
    // set precision to 5 decimal points and create a blank line
    cout << setprecision(5) << endl;

    //  output data to screen
    cout << "Pendulum interval   : " << mBestPendulum.getPieceInterval() << endl;
    cout << "Second-gear interval: " << mGear[INDEX_SEC].getPieceInterval() << endl;
    cout << "Minute-gear interval: " << mGear[INDEX_MIN].getPieceInterval() << endl;
    cout << "Hour-gear interval  : " << mGear[INDEX_HR].getPieceInterval() << endl;
    cout << "Working second-hand : " << mGear[INDEX_SEC].getIsAttToHand() << endl;
    cout << "Working minute-hand : " << mGear[INDEX_MIN].getIsAttToHand() << endl;
    cout << "Working hour-hand   : " << mGear[INDEX_HR].getIsAttToHand() << endl;
    cout << "Total size of clock : " << mNotNullPieces << endl;
    cout << "Total survival score: " << mSurvivalScore << endl;
}

bool bioClock::isPendOnTrain(int x, int y)
{
    for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
        	if ((i == 0) != (j == 0))
                //  check boundaries
                if (x + i < mGenomeSize && x + i >= 0 && y + j < mGenomeSize && y + j >= 0)
                    if (mClockGenome[x + i][y + j].getPieceType() == PTYPE_ESCAPEMENT)
                        if (checkEscapement(x + i, y + j))
                            return true;
    return false;
}
