//  this file defines all interaction/output functions
#include "Interface.h"

// 	convert a string to an integer
int stringTOint (string text)
{
    int intReturn;

    intReturn = atoi (text.c_str ());

    return intReturn;
}

//  convert a string to a double
double stringTOdouble (string text)
{
    double doubleReturn;

    doubleReturn = atof(text.c_str ());

    return doubleReturn;
}

//  struct constructor setting default simulation parameters
varData::varData ()
{
    mPopulationSize = 1000;
    mSimTimes = 10;
    mNumGenerations = 2000;
    mGenomeSize = 10;
    mMutationRate = 1;
    mSelectivePressureMagnitude = 10;
}

//  outputs help info for commands
void writeSettingHelp (string setting, string settingDetails, string helpDetails)
{
    //  the output size allotted to the command
    const int CMD_COL_SIZE = 6;


    //  the output size allotted to the command parameters
    const int PARAM_COL_SIZE = 20;

    cout << endl << setting;
    cout.width (CMD_COL_SIZE - setting.length ());
    cout << "" << settingDetails;
    cout.width (PARAM_COL_SIZE - settingDetails.length ());
    cout << "" << helpDetails;
}

// 	scans for a command and returns the value
string getSetting (string entry, string command, double prevSetting)
{
    //  search the string for the command
    size_t stringPosition;
    stringPosition = entry.find(command);

    //  stores the updated setting value
    string settingValue;

    //  used to convert numbers to strings
    stringstream numConv;

    // 	store the user entered number
    if (stringPosition != string::npos)
        for (unsigned int i = int(stringPosition) + command.length (); i < entry.length (); i++)
        {
            if (isdigit(entry[i]) || entry[i] == '.')
                settingValue.append(1, entry[i]);
            else if (entry[i] != ' ')
                break;
        }
    else
    {
        //  convert the previous settings to a string
        numConv << prevSetting;
        numConv >> settingValue;
    }

    return settingValue;
}

varData runCLI ()
{
    //  the mutation percent must be between 0 and 1 and is entered as a percent
    varData userSettings;

    cout << "Welcome to Clock Evolution!" << endl;
    string settingEntry;

    // 	changeable CLI commands and such
    string simTimes = "sims";
    string simTimeDetails = " [1 - 10000]";
    string population = "pop";
    string populationDetails = " [3 - 10000]";
    string numGenerations = "gens";
    string numGenDetails = " [1 - 10000]";
    string mutationRate = "mrate";
    string mutationRateDetails = " [0 - 100]";
    string genomeSize = "genome";
    string genomeSizeDetails = " [1 - 50]";
    string selectivePressure = "selmag";
    string selectivePressureDetails = " [1 - 10000000]";
    string help = "help";
    string run = "run";

    //  found is used to store the starting location of found string
    size_t stringPosition;

    cout << "For a list of commands enter 'help'" << endl;

    while (true)
    {
        cout << "Clock Evolution Console>>";
        getline (cin, settingEntry);

        //  print help info, when the help command is input
        stringPosition = settingEntry.find (help);
        if (stringPosition != string::npos)
        {
            writeSettingHelp (simTimes, simTimeDetails, "Sets the number of times to run simulation.");
            writeSettingHelp (population, populationDetails, "Sets the starting population size.");
            writeSettingHelp (numGenerations, numGenDetails, "Sets the number of generations to run.");
            writeSettingHelp (genomeSize, genomeSizeDetails, "Sets the size of the genome matrix.");
            writeSettingHelp (mutationRate, mutationRateDetails, "Sets the percent rate of mutation.");
            writeSettingHelp (selectivePressure, selectivePressureDetails, "Sets the magnitude of selective pressure.");
            cout << endl << "run                       Executes simulation using current settings." << endl << endl;
        }


        cout.precision(1);
        cout << endl;
        // 	search string for specific key phrases, and output simulation settings
        userSettings.mSimTimes = stringTOint (getSetting (settingEntry, simTimes, userSettings.mSimTimes));
        cout << "The number of simulations is set to " << userSettings.mSimTimes << endl;

        userSettings.mPopulationSize = stringTOdouble(getSetting (settingEntry, population, userSettings.mPopulationSize));
        cout << "The starting population size is set to " << fixed << userSettings.mPopulationSize << endl;

        userSettings.mNumGenerations = stringTOint(getSetting (settingEntry, numGenerations, userSettings.mNumGenerations));
        cout << "The number of generations is set to " << userSettings.mNumGenerations << endl;

        userSettings.mGenomeSize = stringTOint(getSetting (settingEntry, genomeSize, userSettings.mGenomeSize));
        cout << "The genome matrix size is set to " << userSettings.mGenomeSize << endl;

        userSettings.mMutationRate = stringTOdouble(getSetting (settingEntry, mutationRate, userSettings.mMutationRate));
        cout << "The rate of mutation is set to " << userSettings.mMutationRate << endl;

        userSettings.mSelectivePressureMagnitude = stringTOint(getSetting (settingEntry, selectivePressure, userSettings.mSelectivePressureMagnitude));
        cout << "The selective pressure magnitude is set to " << userSettings.mSelectivePressureMagnitude << endl;

        // 	exit CLI when user specifies to run simulation
        stringPosition = settingEntry.find (run);
        if (stringPosition != string::npos)
            break;

        cout << endl;
    }

    return userSettings;
}
