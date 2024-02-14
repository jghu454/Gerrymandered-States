/*
Project 1 Starter Code - Gerrymandering

This program serves to determine if a state is gerrymandered based off state and district data.
It will display district election data, the amount of voters, the efficiency gap, and which party
the state is gerrymandered against.


Student Name: Jason Hu
Done using Visual Studio Community 2022
University of Illinois at Chicago (UIC)
CS 251, Spring 2023



*/


#include "ourvector.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>


struct districtInfo
{
	int republicanVotes;
	int democratVotes;
	bool demWin;
	int overhalf;
	int repubWasted;
	int demWasted;
};

struct stateInfo
{
	bool gerryMandered;
	string stateName;
	int totalDistricts = 0;
	double efficiencyGap;
	int totalRepubWasted = 0;
	int totalDemoWasted = 0;
	int totalRepubVotes = 0;
	int totalDemoVotes = 0;
	int totalVotes = 0;
	int eligibleVoters;
	ourvector <districtInfo> districts;
};

//GENERAL UTILITY FUNCTIONS

//This function will take in a string and return the lowercase version of it
//This serves to make the program case insensitive whenever we compare the two strings (maybe state names) we will use this on both to determine if they are the same state
string toLower(string state)
{
	string lowerCase = "";
	for (char c : state)
	{
		lowerCase += tolower(c);
	}
	return lowerCase;
}

/*This function takes in :
The string that is the name of the state to look for
a reference to the vector that contains all state struct
This will return the index of the state struct with the corresponding state name
*/
int searchStateIndex(string stateName, ourvector<stateInfo>& allStates)
{
	for (int i = 0; i < allStates.size(); i++)
	{
		if (toLower(allStates.at(i).stateName) == toLower(stateName))
		{
			return i;
		}
	}
	return -1;
}

//UTILITY FUNCTION FOR main()
/*
This function takes in :
a bool that determines if the district/first file has been loaded
a bool that determines if the eligibleVoters/second file has been loaded
a bool that determines if a state has been picked to search
a reference to a stateInfo struct if one has been declared.
the function also returns a string that is an input accepted by this function, that string is the command that will be used later to determine the command/function to use
*/
string printPrompt(bool firstFile, bool secondFile, bool statePicked, stateInfo& currentState)
{
	string command;
	cout << "Data Loaded? " << ((firstFile == true && secondFile == true) ? "Yes" : "No") << endl;
	cout << "State: " << (statePicked ? currentState.stateName : "N/A") << endl << endl;
	cout << "Enter command: ";
	cin >> command;
	cout << endl;
	cout << "-----------------------------" << endl << endl;

	return command;
}

/*LOAD UTILITY FUNCTION*/
/*
This function takes in the struct for a state
as well as a struct for the state
this function will add the district's statistics to the stat's total numbers for votes and other stats
*/
void calculateStateInfo(stateInfo& state, districtInfo CD) //Helper Functions for the "Load Data" Function, CD for Current District
{
	state.totalRepubVotes += CD.republicanVotes;
	state.totalDemoVotes += CD.democratVotes;
	state.totalVotes += CD.republicanVotes + CD.democratVotes;
	state.totalRepubWasted += CD.repubWasted;
	state.totalDemoWasted += CD.demWasted;
	state.efficiencyGap = 100 * abs(state.totalRepubWasted - state.totalDemoWasted) / (double)state.totalVotes;
	state.gerryMandered = (state.totalDistricts >= 3 && state.efficiencyGap >= 7) ? true : false;

};

/*
This function takes in:
a reference to a state struct that will be used to store data
a reference to a string stream object
the function uses the string stream object to parse through the file data on the state and dsitricts and stores each individual district's data in a district struct that is inside of a state struct
*/
void storeDistrictData(stateInfo& state, stringstream& sstr) //Helper function for "Load Data" Function
{
	int index = 0;
	string stateInformation;
	while (getline(sstr, stateInformation, ','))
	{
		if (index % 3 == 0)// every 3 strings is one district
		{
			state.districts.push_back(districtInfo());
			state.totalDistricts++;
		}
		else if (index % 3 == 1)// democrat votes
		{
			districtInfo& CD = state.districts.at(state.districts.size() - 1);//get current district (CD)
			CD.democratVotes = stoi(stateInformation);
		}
		else if (index % 3 == 2)// republican votes
		{
			districtInfo& CD = state.districts.at(state.districts.size() - 1);
			CD.republicanVotes = stoi(stateInformation);

			//determine winner of state and statistics
			CD.demWin = (CD.republicanVotes > CD.democratVotes) ? false : true;
			CD.overhalf = ((CD.republicanVotes + CD.democratVotes) / 2) + 1;
			CD.demWasted = CD.demWin == true ? CD.democratVotes - CD.overhalf : CD.democratVotes;
			CD.repubWasted = CD.demWin == false ? CD.republicanVotes - CD.overhalf : CD.republicanVotes;

			calculateStateInfo(state, CD);
		}
		index++;
	}
	sstr.clear();//empty

}

/*
This function takes in:
a reference to the ifstream object that contains will read the data in the "eligible voters" file
a reference to a string stream object that will be used to parse through the data
a string that contains the name of the file, which is used with the ifstream object to open it
a reference to the vector containing all state structs to look through in order to store the amount of "eligible voters" in their respective state struct if the struct exists for that state
a reference to the bool that determines if the eligible voter's file successfully loaded, and if the entire function runs without failing, it will set this bool as true
this function will print messages if there is any failure opening files
the function will be called in the LOAD function

*/
void storeEligibleVoters(ifstream& fT, stringstream& sstr, string eVF, ourvector <stateInfo>& aS, bool& secondLoaded)//aS = allStates,  eVF = eligibleVoterFile, fT = fileTwo
{
	fT.open(eVF);
	if (fT.fail()) {//error messages
		cout << "Invalid second file, try again." << endl << endl;
		fT.close();
		return;
	}

	string stateData;

	cout << "Reading: " << eVF << endl;
	while (getline(fT, stateData))
	{

		sstr << stateData;
		getline(sstr, stateData, ',');
		int indexOfState = searchStateIndex(stateData, aS);
		if (indexOfState != -1)
		{
			getline(sstr, stateData, ',');
			aS.at(indexOfState).eligibleVoters = stoi(stateData);
			cout << "..." << aS.at(indexOfState).stateName << "..." << aS.at(indexOfState).eligibleVoters << " eligible voters" << endl;//print data to screen
			sstr.clear();
		}
		getline(sstr, stateData, ',');
		sstr.clear();
	}
	cout << endl;
	fT.close();
	secondLoaded = true;
}

//LOAD FUNCTION
/*
This function takes in:
a reference to the bool that determines if the "district" data file is loaded
a reference to the bool that determines if the "eligible" data file is loaded (This will be passed through the "storeEligibleVoters()" function when it is called)
a reference to the vector that holds all state structs
a reference to the string that contains the district file name
a reference to the string that contains the eligibleVoters file name (This will be passed through the "storeEligibleVoters()" function when it is called)
a reference to a string stream object that will bve used to parse through file data for the "district" data file
a reference to a ifstream object "fO" that is used to read the district file
a reference to a ifstream object "fT" that is used to read the eligibleVoters file

This function will take the ifstream "fO" and read the string that contains the district file name and store data to the respective state inside the vector
if the file fails to open, it will print out a prompt or error message and end the function and loaded process
if the loading process is completed, it will set the bool for the firstFileLoaded to be true and will call "storeEligibleVoters()" after
*/
void useLoad(bool& firstLoaded, bool& secondLoaded, ourvector<stateInfo>& aS, string& dF, string& eVF, stringstream& sstr, ifstream& fO, ifstream& fT)//dF = districtFile, fO = file one, aS = allStates, eVF = eligibleVoterFile
{
	cin >> dF;
	cin >> eVF;
	fO.open(dF);
	if (firstLoaded && secondLoaded) {//error messages
		cout << "Already read data in, exit and start over." << endl << endl;
		return;
	}
	if (fO.fail()) {//error messages
		cout << "Invalid first file, try again." << endl << endl;
		fO.close();
		return;
	}

	string stateData;
	cout << "Reading: " << dF << endl;
	while (getline(fO, stateData))
	{
		sstr << stateData;

		stateInfo state;
		getline(sstr, stateData, ',');
		state.stateName = stateData;
		storeDistrictData(state, sstr);
		cout << "..." << state.stateName << "..." << state.totalDistricts << " districts total" << endl;
		aS.push_back(state);
	}
	cout << endl;
	fO.close();
	firstLoaded = true;
	storeEligibleVoters(fT, sstr, eVF, aS, secondLoaded);//Store eligible voters separately
}

//SEARCH FUNCTION
/*
This function takes in:
a reference to the vector that holds all state structs
a reference to the struct that will be used to hold the data searched for
a bool that determines if the first file is loaded at the very least
a string is created in this function to store input from the user in order to determine the state they want to search for
once the user has entered that input, the function loops through the vector with all states in it and looks for the struct that has the name
this is done by using the "searchStateIndex()" function from earlier
with the index, we will set the reference variable of the state struct to the struct at the index
if a state is found, this function will return true to a variable that tells us if a state has been found
if there are any problems, there will be error messages printed
*/
bool useSearch(ourvector <stateInfo>& aS, stateInfo& cS, bool firstLoaded)//cS = currentState
{
	string state;
	getline(cin, state);

	if (!firstLoaded)//error messages
	{
		cout << "No data loaded, please load data first." << endl << endl;
		return false;
	}

	state = state.substr(1, state.length());//will cut off the first character of the state name which is an empty space to avoid accepting info like " New Jersey" or " Wisconsin"
	int indexOfState = searchStateIndex(state, aS);

	if (indexOfState == -1) {//error messages
		cout << "State does not exist, search again." << endl << endl;
		return false;
	}
	cS = aS.at(indexOfState);//set the current state to the state searched for
	return true;
}

//STATS FUNCTION
/*
This function takes in:
a struct of the state that the player searched for with the search function
a bool that determines if the first/district file has been loaded
a bool that determines if the eligibleVoters/second file has been loaded
a bool that determines if a state has been found from the search function
this function displays the stats for a state and if it has been gerrymandered or not
*/
void useStats(stateInfo& cS, bool firstLoaded, bool secondLoaded, bool sP)//sP = statePicked, cS = currentState
{
	if (!sP)//error messages
	{
		cout << "No state indicated, please search for state first." << endl << endl;
		return;
	}
	if (!firstLoaded || !secondLoaded)//error messages
	{
		cout << "No data loaded, please load data first." << endl << endl;
		return;
	}
	cout << "Gerrymandered: " << ((cS.gerryMandered == true) ? "Yes" : "No");
	if (cS.gerryMandered == true)
	{
		cout << "Gerrymandered against: " << ((cS.totalDemoVotes > cS.totalRepubVotes) ? "Republicans" : "Democrats") << endl;
		cout << "Efficiency Factor: " << cS.efficiencyGap << "%" << endl;
	}
	cout << endl << "Wasted Democratic votes: " << cS.totalDemoWasted << endl;
	cout << "Wasted Republican votes: " << cS.totalRepubWasted << endl;
	cout << "Eligible voters: " << cS.eligibleVoters << endl << endl;
}

/*PLOT UTILITY FUNCTION*/
/*
This function takes in:
an int that contains the amount of letter D's to print for Democrats
an int that contains the amount of letter R's to print for Republicans
This function will be called by the "usePlot" function
*/
void displayDistrictHistogram(int democrats, int republicans)
{

	for (int i = 0; i < democrats; i++)
	{
		cout << "D";
	}
	for (int i = 0; i < republicans; i++)
	{
		cout << "R";
	}
	cout << endl;
}

//PLOT FUNCTION
/*
This function takes in:
a struct for the state that has been chosen
a bool to determine if the first/district file has been loaded
a bool to determine if the second/eligibleVoters file has been loaded
a bool that determines if a state has been chosen with the search function earlier

If there are problems then this function will print out error messages
If there are no problems, this function will calculate the amount of votes for each republicans and democrats and pass them to the "displayDistrictHistogram()" function to print out the letters
*/
void usePlot(stateInfo& cS, bool firstLoaded, bool secondLoaded, bool sP)//sP = statePicked, cs = currentState
{

	if (!firstLoaded || !secondLoaded) {//error messages
		cout << "No data loaded, please load data first." << endl << endl;
		return;
	}
	if (!sP)//error messages
	{
		cout << "No state indicated, please search for state first." << endl << endl;
		return;
	}
	for (int i = 0; i < cS.districts.size(); i++) {//looping through all districts in a state
		districtInfo& CD = cS.districts.at(i);//CD for current District
		int democrats = (CD.democratVotes / (double)(CD.democratVotes + CD.republicanVotes) * 100); // calculating democrat votes by doing democratVotes/totalVotes then multiplying by 100
		double republicans = 100 - democrats; // subtract the votes from 100 to get republicans
		cout << "District: " << i + 1 << endl;
		displayDistrictHistogram(democrats, republicans);
	}
	cout << endl;
}
int main()
{
	bool appOn = true;
	bool statePicked = false;
	bool firstFileLoaded = false;
	bool secondFileLoaded = false;

	stateInfo currentState;
	ourvector <stateInfo> allStates;

	string command;
	string districtFile;
	string eligVoterFile;

	stringstream sstr;
	ifstream fileOne;
	ifstream fileTwo;
	cout << "Welcome to the Gerrymandering App!" << endl << endl;

	while (appOn)
	{
		command = printPrompt(firstFileLoaded, secondFileLoaded, statePicked, currentState);
		if (command == "load") {
			useLoad(firstFileLoaded, secondFileLoaded, allStates, districtFile, eligVoterFile, sstr, fileOne, fileTwo);
		}
		else if (command == "search") {
			statePicked = useSearch(allStates, currentState, firstFileLoaded);
		}
		else if (command == "stats") {
			useStats(currentState, firstFileLoaded, secondFileLoaded, statePicked);
		}
		else if (command == "plot") {
			usePlot(currentState, firstFileLoaded, secondFileLoaded, statePicked);
		}
		else if (command == "exit") {
			appOn = false;
			break;
		}
	}

	return 0;
}