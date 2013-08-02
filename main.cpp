////////////////////////////////
/*
    Watching Evolution
    Bryan Hatton & Eddie Kim
    ICS 4MI
    Started: Oct 14 2008
    Completed: (Comp-Sci) Jan 21 2009
			   (Sci-Fair) Feb 25 2009
*/
///////////////////////////////

/*
RULES FOR PIECE CONNECTION:

1. Gears connect to everything at the axle, except for other gears and escapements.
2. Pendulums are only functional when attached to ONLY one other piece, whatever that piece may be.
3. For a clock to become more than a simple pendulum, it must fit the following criteria:
        i. Exactly one working pendulum.
        ii. A mainspring-driven, non-binding gear train.
        iii. An escapement that bridges the working pendulum with a working gear train.
4. Hands may attach to a gear, which increases the gear's significance. Hands can turn as long as they are attached only to that gear.
*/

#include "Evolve.h"
#include "Interface.h"

int main()
{
    //  get simulation info from user
    varData settings;
    settings = runCLI();

    // check for manual user quit
    if (settings.mQuitFlag)
    {
        cout << endl << "User quit manually!" << endl;
        return 0;
    }

    //  run the simulations
    for (int i = 0; i < settings.mSimTimes; i++)
    {
    	//	make the world
        world simulation (settings);

        //	initialize clocks
        simulation.initClocks();

        //	mate them
        simulation.mateClocks();

        //	close output file
        simulation.fout.close();
        simulation.gout.close();
    }

    return 0;
}
