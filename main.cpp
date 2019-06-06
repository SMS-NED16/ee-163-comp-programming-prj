/* EE-163 Computers and Programming Final Project
 * Electricity Bill Calculation Programme
 * Submitted by EE-16163, FE-EE 2016-2017
 */

#include <iostream>         //for cin, cout
#include <iomanip>          //for setw and left/right alignment flags
#include <vector>           //for bill information stored in a vector
#include <fstream>          //for file I/O
#include <cstdlib>          //for exit(1) - terminate programme
#include <windows.h>        //for SetWindow function
using namespace std;

///*=====================================================STRUCTS==================================================*/

struct billData
{
    /*------------------------------------MEMBER VARIABLES INITIALISED DIRECTLY-----------------------------------*/
	double		load;				//in kW
	int		month;				//month number - b/w 1 and 12 inclusive
	string		monthName;			//three char string
	int		year;				//int
	int		units;				//units consumed or kWh
	bool		isPaid;				//1 if paid, 0 if unpaid
	bool		isThreePhase;			//1 if 3-phase, 0 if single phase
	double		kiloVolts;			//for industrial only
	/*-----------------------------------MEMBER VARIABLES INITIALISED INDIRECTLY-----------------------------------*/
	double		amount;				//init to 0;
	double		base;				//basePrice;
	double		surcharge;			//additional charge on basis of units
	double		perKW_Charge;			//load pricing

};

///*=====================================================STRUCTS==================================================*/


///*====================================================CONSTANTS=================================================*/

const int ROW_SIZE = 100; 				//used for creating textboxes/boundaries

///*====================================================CONSTANTS=================================================*/


///*==============================================FUNCTION DECLARATIONS===========================================*/

//FUNCTION TO SET WINDOW SIZE
void SetWindow(int Width, int Height);


//TEXT AND HEADER FUNCTIONS
void    printRow(const char mySymbol, int rowSize);
void    printFormatted(const string myString, int rowSize, char mySymbol);
void    printTextBox(const string& myString, const char& myChar, const int& rowSize);
void    printProjectHeading();


//CATEGORY FUNCTIONS
bool    isValidCategory(const char &myCategory);
void    toUpper(char &myCategory);
string  getCategoryName(const char &myCategory);
void    getCategory(char &myCategory);


//LOGIN FUNCTIONS
bool    isValidUsername(const string &myUsername, const char &myCategory);
void    getUsername(string &myUsername, const char &myCategory);
bool    isValidPassword(const string &myPassword, const char &myCategory);
void    getPassword(string &myPassword, const char &myCategory, int &myAttempts);


//RECORD FUNCTIONS
bool    validEntryInput(const char &myInput);
void    chooseInputMethod(char &myInput);
string  setMonth(const billData &myData);
bool    validUserData(const billData &myData);
void    recordFromUser(vector<billData> &myVector);
void    recordFromFile(vector<billData> &myVector, const string &infileName);


//SERVICE INPUT FUNCTIONS
bool    isValidService(const char &myService);
void    getService(char &myService);
bool    validContinueInput(const char &myInput);


//SERVICE SECONDARY FUNCTIONS
double  getResidentialCharges(billData &myData);
double  getCommercialCharges(billData& myData);
double  getIndustrialCharges(billData &myData);
void    printBillStructure(const billData &myData, const char& myCategory);
void    getRangeFlags(const vector<billData> &myRecord, int &myMinMarker, int &myMaxMarker);
void    billBreakdown(const vector<billData> &myRecord, const char &myCategory);
void    printBar(const double &myMin, const double &myMax, const double &myAmount);
void    printBillData(const billData &myData);


//SERVICE PRIMARY FUNCTIONS
void    paymentHistory(const vector<billData> &myRecord, const char &myCategory);
void    billPayment(vector<billData> &myRecord, const char &myCategory);

///*==========================================FUNCTION DECLARATIONS===========================================*/


///*=================================================MAIN=====================================================*/

int main()
{
    ///*-------------------------------------------VARIABLES--------------------------------------------------*/

	char				userCategory;			//to choose appropriate bill calculation functions
	char				mainBorder = '+', 		//variables to make textbox modding easier
					secBorder = '=';
	string				username, password;		//login variables
	int				attemptsRemaining = 5;		//to init lockout after 5 incorrect password entries
	char				inputChoice;			//to choose data entry method - from file or manually
	string 				inputFile;			//name of txt file with user's bill data
	vector<billData>		userRecord;			//programme's copy of user's bill data
	char				serviceChoice, proceed;		//to choose service/continue or terminate loops

	///*--------------------------------------------SETTINGS--------------------------------------------------*/

	cout << fixed;                              //ensures large numeric values not displayed in scientific form
	cout << setprecision(2);                    //sets precision to 2 decimal places for all numeric values
	SetWindow(150, 100);                        //sets upper limits on window size - prevents line overflow

	///*-------------------------------------STEP 1: PRINT HEADERS--------------------------------------------*/

	printProjectHeading();
	printTextBox("Welcome to KESC's Customer Portal", mainBorder, ROW_SIZE);

	///*----------------------------------STEP 2: ASK FOR CATEGORY--------------------------------------------*/

	getCategory(userCategory);

	///*----------------------------------------STEP 3: LOGIN-------------------------------------------------*/

	printTextBox("LOGIN", secBorder, ROW_SIZE);
	getUsername(username, userCategory);
	getPassword(password, userCategory, attemptsRemaining);
	cout << "You are a category " << userCategory << " user." << endl;
	cout << "Username:\t" << username << endl;
	cout << "Password:\t" << password << endl;
	printTextBox("LOGIN SUCCESSFUL", secBorder, ROW_SIZE);
	printTextBox("Welcome user " + username + "!", secBorder, ROW_SIZE);

	///*-------------------------------------STEP 4: LOAD DATA------------------------------------------------*/

	printTextBox("CHOOSE INPUT METHOD", secBorder, ROW_SIZE);
	chooseInputMethod(inputChoice);         //ask user for data entry choice - A for manual, B for file
	inputFile = username + ".txt";          //initialising name of input file - for file input
	if (inputChoice == 'A' || inputChoice == 'a')
	{
		recordFromUser(userRecord);
	}
	else
	{
		recordFromFile(userRecord, inputFile);
	}

	//Programme exits if no data entered
	if (userRecord.size() == 0)
	{
		cout << "No data entered. Run programme again." << endl;
		exit(1);
	}

	//Initialising bill amount for each element in userRecord
	//using category to choose appropriate function
	for (int i = 0; i < userRecord.size(); i++)
	{
		if (userCategory == 'A')
			userRecord[i].amount = getResidentialCharges(userRecord[i]);

		else if (userCategory == 'B')
			userRecord[i].amount = getCommercialCharges(userRecord[i]);

		else
			userRecord[i].amount = getIndustrialCharges(userRecord[i]);
	}
	printTextBox("END OF DATA INPUT", secBorder, ROW_SIZE);

	///*--------------------------------------STEP 5: SERVICES------------------------------------------------*/

	do
	{
		printTextBox("SERVICES MENU", secBorder, ROW_SIZE);
		getService(serviceChoice);
		if (serviceChoice == 'A')
		{
			printTextBox("PAYMENT HISTORY", secBorder, ROW_SIZE);
			paymentHistory(userRecord, userCategory);
			printTextBox("END OF PAYMENT HISTORY", secBorder, ROW_SIZE);
		}
		else
		{
			printTextBox("BILL PAYMENT", secBorder, ROW_SIZE);
			billPayment(userRecord, userCategory);
			printTextBox("END OF BILL PAYMENT", secBorder, ROW_SIZE);
		}

        //Ask user if they wish to return to services
        //Continue asking until Y/N input received
		do
		{
			cout << "Would you like to return to the SERVICES MENU?" << endl;
			cout << "Enter 'Y' for yes, 'N' for no." << endl;
			cin >> proceed;
			if (!validContinueInput(proceed))
			{
				cout << "Invalid input. Please try again." << endl;
			}
		}
		while(!validContinueInput(proceed));
	}
	while (!(proceed == 'N' || proceed == 'n'));            //Service loop exits when user enters 'N' or n

	///*--------------------------------------STEP 6: LOGOUT MESSAGES-----------------------------------------*/

	printTextBox("Goodbye User " + username + "!", secBorder, ROW_SIZE);
	printTextBox("Thank you for using KESC's Customer Portal", mainBorder, ROW_SIZE);
    cout << endl << endl;
	///*------------------------------STEP 7: BEG SIR IQBAL FOR BONUS MARKS-----------------------------------*/

	printTextBox("Pls give bonus marks thx", '-', ROW_SIZE); //Midterm bohat kharaab hua tha sir. Pls give marks. Thx
	cout << "END OF PROGRAMME" << endl;
	return 0;
}

///*=================================================MAIN=====================================================*/


///*==========================================FUNCTION DEFINITIONS============================================*/


/*
 * from https://www.3dbuzz.com/forum/threads/102195-How-do-you-resize-a-console-window-in-C
 * Sets window size to prevent line overflow due to different IDE console widths
 */
void SetWindow(int Width, int Height)
{
    _COORD coord;
    coord.X = Width;
    coord.Y = Height;

    _SMALL_RECT Rect;
    Rect.Top = 0;
    Rect.Left = 0;
    Rect.Bottom = Height - 1;
    Rect.Right = Width - 1;

    HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);      // Get Handle
    SetConsoleScreenBufferSize(Handle, coord);            // Set Buffer Size
    SetConsoleWindowInfo(Handle, TRUE, &Rect);            // Set Window Size

}
///*-----------------------------------------TEXT AND HEADER FUNCTIONS----------------------------------------*/

/*print row of symbols - used in textboxes*/
void printRow(const char mySymbol, int rowSize)
{
	for (int i = 0; i < rowSize; i++)
	{
		cout << mySymbol;
	}
	cout << endl;
}


/*
 *print center-justified string with symbols at either end
 *i.e. in format (symbol | spacing | string | spacing | symbol)
 */
void printFormatted(const string myString, int rowSize, char symbol)
{
	int spacing = (rowSize - myString.length())/2;

	if (myString.length()%2 != 0)	//if string length is odd, R_spacing = L_Spacing + 1
	{
		cout << setw(spacing) << left << symbol << myString << setw(spacing+1) << right << symbol << endl;
	}
	else
	{
		cout << setw(spacing) << left << symbol << myString << setw(spacing) << right << symbol << endl;
	}
}


/*prints textbox with text in center and border of myChar*/
void printTextBox(const string& myString, const char& myChar, const int& rowSize)
{
	printRow(myChar, rowSize);
	printFormatted(myString, rowSize, myChar);
	printRow(myChar, rowSize);
}


/*prints heading at beginning of programme*/
void printProjectHeading()
{
	printRow('*', ROW_SIZE);        //top row
	printFormatted("EE-163 COMPUTERS AND PROGRAMMING", ROW_SIZE, '*');
	printFormatted("Electricity Bill Calculation Programme", ROW_SIZE, '*');
	printFormatted("Saad Mashkoor Siddiqui EE-163 Section D FE-EE 2016-2017", ROW_SIZE, '*');
	printRow('*', ROW_SIZE);        //bottom row
	cout << endl;
}

///*---------------------------------------------CATEGORY FUNCTIONS-------------------------------------------*/
/*
 * checks if user has entered correct letter
 * for category choice; handles uppercase/lowercase
 */
bool isValidCategory(const char &myCategory)
{
	if (!(myCategory == 'A' || myCategory == 'a'
	   || myCategory == 'B' || myCategory == 'b'
	   || myCategory == 'C' || myCategory == 'c'))
	{
		return false;
	}
	else
	{
		return true;
	}
}


/*
 * convert lowercase category to uppercase category
 * used b/c input filename includes uppercase category letter
 */
void toUpper(char &myCategory)
{
	if (myCategory == 'a')
	{
		myCategory = 'A';
	}
	else if (myCategory == 'b')
	{
		myCategory = 'B';
	}
	else
	{
		myCategory = 'C';
	}
}


/*
 * returns category name as string from category character
 * used for echoing category name after category input
 */
string getCategoryName(const char &myCategory)
{
	if (myCategory == 'A')
	{
		return "RESIDENTIAL";
	}
	else if (myCategory == 'B')
	{
		return "COMMERCIAL";
	}
	else
	{
		return "INDUSTRIAL";
	}
}


/*
 * Asks the user to enter a character for category
 * A - Residential B - Commercial C- Industrial
 * Prevents progression until valid input received
 */
void getCategory(char &myCategory)
{
    //STEP 1: Get input
	do      //start asking for category letter
	{
		cout << "Please enter your user category." << endl;
		cout << "A: Residential\t\tB: Commercial\t\tC: Industrial" << endl;
		cin >> myCategory;
		if(!isValidCategory(myCategory))
		{
			cout << "Invalid input. Please try again." << endl;
		}
	}
	while(!isValidCategory(myCategory));      //continue asking until A/B/C input

	//STEP 2: Convert input to uppercase if necessary
	if (myCategory == 'a' || myCategory == 'b' || myCategory == 'c')
	{
		toUpper(myCategory);
	}

	//STEP 3: Echo category
	cout << "You are a category " << myCategory << " or " << getCategoryName(myCategory) << " user." << endl;
}


///*---------------------------------------------LOGIN FUNCTIONS----------------------------------------------*/

/*
 * If user category char = 'x', then username is 'x163'
 * Checks username against user category
 */
bool isValidUsername(const string &myUsername, const char &myCategory)
{
	if ((myCategory == 'A' && myUsername == "A163") ||
		(myCategory == 'B' && myUsername == "B163") ||
		(myCategory == 'C' && myUsername == "C163"))
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*
 * Prompts user to enter username - validates against category
 * If valid, stores in main - used in login/logout messages
 */
void getUsername(string &myUsername, const char &myCategory)
{
	do
	{
		cout << "Enter username." << endl;
		cin >> myUsername;
		if(!isValidUsername(myUsername, myCategory))
		{
			cout << "Invalid username. Please try again." << endl;
		}
	}
	while(!isValidUsername(myUsername, myCategory));
}


/*
 * if category character = 'x', then password = "1234x"
 * validates password against user category
 */
bool isValidPassword(const string &myPassword, const char &myCategory)
{
	if ((myCategory == 'A' && myPassword == "1234A")
	||  (myCategory == 'B' && myPassword == "1234B")
	||  (myCategory == 'C' && myPassword == "1234C"))
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*
 * Checks password entered by user for validity
 * If incorrect, decrements myAttempts
 * After 5 incorrect passwords, programme terminates - account lockout
 */
void getPassword(string &myPassword, const char &myCategory, int &myAttempts)
{

	do
	{
		cout << "Please enter password." << endl;
		cin >> myPassword;
		if (!isValidPassword(myPassword, myCategory))
		{
			cout << "Invalid password. Try again." << endl;
			myAttempts--;           //attemptsRemaining decremented here

			if (myAttempts == 1)    //Separate message for only 1 attempt remaining
			{
				cout << myAttempts << " attempt remaining until lockout." << endl;
			}
			else                    //Separate message for several attempts remaining
			{
				cout << myAttempts << " attempts remaining until lockout." << endl;
			}
		}
	}
	//loop will terminate if password correct or if attempts run out
	while(!(isValidPassword(myPassword, myCategory))&&(myAttempts!=0));
	if (myAttempts == 0)            //account lockout message
	{
		cout << "Too many unsuccessful login attempts." << endl;
		cout << "Please contact your nearest KESC office." << endl;
		exit(1);                    //premature termination
	}
}


///*---------------------------------------------RECORD FUNCTIONS----------------------------------------------*/


/*
 * Checks if user input matches preset values for data entry methods
 */
bool validEntryInput(const char &myInput)
{
	if (myInput == 'A' || myInput == 'a' || myInput == 'B' || myInput == 'b')
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*
 * allow user to choose to enter bill information manually
 * or let the computer read data from a file
 */
void chooseInputMethod(char &myInput)
{
	do
	{
		cout << "Please choose how to enter data." << endl;
		cout << "A: Manually\t\tB: From file" << endl;
		cin >> myInput;
		if (!validEntryInput(myInput))
		{
			cout << "Invalid entry. Please try again." << endl;
		}
	}
	while(!validEntryInput(myInput));
}


/*
 * sets monthName member variable's value using month number
 * as only month number entered in either data input method
 */
string setMonth(const billData &myData)
{
	if (myData.month == 1)
	{
		return "Jan";
	}
	else if (myData.month == 2)
	{
		return "Feb";
	}
	else if (myData.month == 3)
	{
		return "Mar";
	}
	else if (myData.month == 4)
    {
        return "Apr";
    }
	else if (myData.month == 5)
	{
		return "May";
	}
	else if (myData.month == 6)
	{
		return "June";
	}
	else if (myData.month == 7)
	{
		return "July";
	}
	else if (myData.month == 8)
	{
		return "Aug";
	}
	else if (myData.month == 9)
	{
		return "Sep";
	}
	else if (myData.month == 10)
	{
		return "Oct";
	}
	else if (myData.month == 11)
	{
		return "Nov";
	}
	else
	{
		return "Dec";
	}
}


/*
 * Used to validate data entered by user in recordFromUser
 * Will not allow data to be added to userRecord vector
 * If data entered for any field is invalid
 */
bool validUserData(const billData &myData)
{
    //Checking all fields for validity
    //load, units, kiloVolts cannot be less than 0
    //month number must be between 1 and 12 inclusive
    //boolean flags cannot be anything other than 1 or 0
	if (myData.load <= 0 || myData.month < 1 || myData.month > 12 ||
		myData.units < 0 || !((myData.isPaid == 1)||(myData.isPaid == 0))
		|| (!(myData.isThreePhase == 1 || myData.isThreePhase == 0)) || myData.kiloVolts < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}


/*
 * asks user to enter bill information, one value at a time in order
 * checks validity of all data, pushes to userRecord if no errors
 */
void recordFromUser(vector<billData> &myVector)
{
	double 		tempVal;            //temporarily stores user input
	billData 	temp;               //temporary object that is pushed to vector
	do
	{
	    //INTRODUCTORY MESSAGES
		printTextBox("MANUAL DATA ENTRY", '-', ROW_SIZE);
		cout << "Enter bill data as prompted." << endl;
		cout << "Enter -1 to exit at any point." << endl;

		//GETTING LOAD
		cout << "\nEnter load in kW." << endl;
		cout << "Load must be a positive number." << endl;
		cin >> tempVal;
		if (tempVal == -1)          //the values stored by tempVal can never be negative
			break;                  //so using -1 as a sentinel for exit
		temp.load = tempVal;        //assigning tempVal to corresponding temp member variable


		//GETTING MONTH NUMBER
		cout << "\nEnter month number." << endl;
		cout << "Month number must be a whole number between 1 and 12 inclusive." << endl;
		cin >> tempVal;
		if (tempVal == -1)
			break;
		temp.month = (int)tempVal;


		//GETTING YEAR
        cout << "\nEnter year." << endl;
        cout << "Year must be a positive number." << endl;
        cin >> tempVal;
        if (tempVal == -1)
            break;
        temp.year = (int)tempVal;


        //GETTING UNITS
		cout << "\nEnter units consumed." << endl;
		cout << "Units consumed must be a positive whole number." << endl;
		cin >> tempVal;
		if (tempVal == -1)
			break;
		temp.units = (int)tempVal;


		//GETTING BILL STATUS - numeric input typecast to bool automatically
		cout << "\nIs the bill for this month paid?" << endl;
		cout << "Enter '1' for yes, '0' for no." << endl;
		cin >> tempVal;
		if (tempVal == -1)
			break;
        temp.isPaid = (int)tempVal;             //anything other than zero is 1


        //GETTING THREE PHASE STATUS - same as above for typecasting
		cout << "\nIs your load powered by a three phase system?" << endl;
		cout << "Enter '1' for yes, '0' for no." << endl;
        cin >> tempVal;
		if (tempVal == -1)
			break;
		temp.isThreePhase = (int)tempVal;


		//GETTING SUPPLY VOLTAGE
		cout << "\nEnter supply voltage in kV." << endl;
		cout << "Supply voltage must be a positive number." << endl;
		cin >> tempVal;
		if (tempVal == -1)
			break;
		temp.kiloVolts = tempVal;


		//VALIDATING ALL FIELDS OF TEMP
		if (!validUserData(temp))
		{
			cout << "One or more invalid inputs. Please try again." << endl;
		}
		else
		{
			temp.monthName = setMonth(temp);        //initialise month name using month number
			myVector.push_back(temp);               //add complete data object to vector
		}
	}
	while(!(tempVal==-1.0)||(!validUserData(temp)));
	//Loop will continue until user decides to quit or until all valid data entered
	cout << "Data read successfully!" << endl;      //echoing successful data input
}


/*
 * Reads bill information from a txt file
 * Checks if specified file is in programmme directory
 * If not, exits with error message
 * Begins reading data
 * Order: load -> month Number -> year -> units -> paidFlag -> threePhaseFlag -> supply volts
 * Stops reading data when reaches end of file - eof flag asserted
 * closes input file - prevents involuntary modification/corruption
 */
void recordFromFile(vector<billData> &myVector, const string &infileName)
{
	ifstream infile;                                //declaring input file stream (ifstream) object named infile
	billData	temp;                               //to hold data before pushing to vector
	infile.open(infileName.c_str());                //opens infileName - converts to C-String - added post Code::Blocks test
	if (!infile.is_open())                          //Check: input file not in directory or file name incorrect?
	{
		cout << "Error communicating with KESC servers." << endl;
		cout << "Please try again later." << endl;
		cout << "(Check if A163/B163/C163 files are in programme directory.)" << endl;
		exit(1);                                    //terminates if file not found
	}
	else
	{
		cout << "Fetching your data from KESC servers..." << endl;
		do
		{
			infile >> temp.load >> temp.month
				   >> temp.monthName >> temp.year
				   >> temp.units >> temp.isPaid
				   >> temp.isThreePhase
				   >> temp.kiloVolts;
			myVector.push_back(temp);               //pushing w/o checking b/c assuming file data in correct format
		}
		while(!infile.eof());                       //loop continues until end of file reached
		infile.close();                             //close input file - prevent involuntary data corruption
		cout << "Data read successfully!" << endl;  //echo successful data input
	}
}


///*---------------------------------------------SERVICE INPUT FUNCTIONS----------------------------------------------*/


/*validates user's input for service choice against presets*/
bool isValidService(const char &myService)
{
	if (myService == 'A' || myService == 'a' || myService == 'B' || myService == 'b')
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*
 * Asks user to choose service by entering character
 * Checks entered character for validity - Repeats until valid service choice entered
 */
void getService(char &myService)
{
	do
	{
		cout << "Please enter a letter to choose your service." << endl;
		cout << "A: Payment History\tB: Bill Payment" << endl;
		cin >> myService;
		if (!isValidService(myService))
		{
			cout << "Invalid input. Please try again." << endl;
		}
	}
	while(!isValidService(myService));
}


/*
 * At several points in the programme, user has to enter Y/N
 * to decide what to do next - validates input
 */
bool validContinueInput(const char &myInput)
{
	if (myInput == 'Y' || myInput == 'y' || myInput == 'N' || myInput == 'n')
	{
		return true;
	}
	else
	{
		return false;
	}
}


///*-------------------------------------------SERVICE SECONDARY FUNCTIONS--------------------------------------------*/

/*compute charges for residential billData object*/
double getResidentialCharges(billData &myData)
{
	double variableCharges = 0, fixedCharges = 0, surcharge = 0;
	double basePrice;

	//VARIABLE CHARGES FOR LOAD UNDER 5kW
	if (myData.load < 5)
	{
		if (myData.units <= 50)
			basePrice = myData.units*2.00;

		else if (myData.units > 50 && myData.units <= 100)
			basePrice = (myData.units-50)*5.79 + 50*2.00;

		else if (myData.units > 100 && myData.units <= 200)
			basePrice = (myData.units-100)*8.11 + 100*5.79;

		else if (myData.units > 200 && myData.units <= 300)
			basePrice = (myData.units - 200)*10.20 + 200*8.11;

		else if (myData.units > 300 && myData.units <= 700)
		{
			basePrice = (myData.units - 300)*15.45 + 300*10.20;
			surcharge = myData.units*0.55;
		}

		else
		{
			basePrice = (myData.units - 700)*17.33 + 700*15.45;
			surcharge = myData.units*0.67;
		}
	}

	//VARIABLE CHARGES FOR LOAD OVER 5kW - uses peak/off-peak system (4h peak:20h off-peak)
	else
	{
		basePrice = (0.167*myData.units*18.00) + (0.833*myData.units*17.33);    //4/24 = 0.167; 20/24 = 0.833
		surcharge = myData.units*0.17;
	}

	//ADDING SINGLE PHASE/THREE PHASE FIXED CHARGES
	if (myData.isThreePhase)
	{
		fixedCharges += 150;
	}
	else
	{
		fixedCharges += 75;
	}

	//init object's base/perKW/surcharge for use in billBreakdown
	myData.base = basePrice;
	myData.perKW_Charge = 0;
	myData.surcharge = surcharge;
	fixedCharges += 35 + 15 + 8;				                //TVL + Meter Rent + Bank Charge
	variableCharges += 0.015*basePrice + 0.175*basePrice + basePrice;	//Duty on units + GST on units + Units
	return (variableCharges + fixedCharges + surcharge);		        //Total
}


/*compute charges for commercial billData object*/
double getCommercialCharges(billData& myData)
{
	double variableCharges = 0, fixedCharges = 0, surcharge = 0, FixedPer_kW = 0, basePrice = 0;
	if (myData.load < 5)
	{
		basePrice += 17.83*(myData.units);
		surcharge += 0.17*(myData.units);
	}
	else
	{
		FixedPer_kW += 400*myData.load;
		basePrice += 15.03*myData.units;
		surcharge += 0.97*myData.units;
	}
	if (myData.isThreePhase)
	{
		fixedCharges+=350;
	}
	else
	{
		fixedCharges+=175;
	}
	fixedCharges += (8 + 15 + 60);                  //bank charges + meter rent + tv license fees
	variableCharges += 0.02*basePrice + 0.175*basePrice + basePrice;
	//init object's base/perKW/surcharge for use in billBreakdown
	myData.base = basePrice;
	myData.perKW_Charge = FixedPer_kW;
	myData.surcharge = surcharge;
 	return (fixedCharges + variableCharges + surcharge + FixedPer_kW);
}


/*compute charges for industrial billData object*/
double getIndustrialCharges(billData &myData)
{
	double variableCharges = 0, fixedCharges = 0, basePrice = 0,
			surcharge = 0, FixedPer_kW = 0, fixedMonthly, totalCharges;

	if (myData.load < 5 && (myData.kiloVolts == 0.4 || myData.kiloVolts == 0.23))
	{
		fixedMonthly=350;
		surcharge += myData.units*0.17;
		basePrice += myData.units*14.33;
	}
	else if (myData.load >= 5 && myData.load <= 500 && myData.kiloVolts == 0.4)
	{
		fixedMonthly = 2000;
		FixedPer_kW += 400*myData.load;
		surcharge += 1.07*myData.units;
		basePrice += 12.93*myData.units;
	}
	else if (myData.load >=5 && myData.load <= 5000 && myData.kiloVolts == 33 )
	{
		fixedMonthly = 50000;
		FixedPer_kW += 380*myData.load;
		surcharge += 3.61*myData.units;
		basePrice += 12.53*myData.units;
	}
	else
	{
		fixedMonthly = 500000;
		FixedPer_kW += 360*myData.load;
		surcharge += 3.61*myData.units;
		basePrice += 12.13*myData.units;
	}
	variableCharges +=  basePrice +
						0.175*basePrice + 0.015*basePrice;
	fixedCharges += (fixedMonthly + 60.00 + 8.00);
	totalCharges  =  fixedCharges + variableCharges + surcharge + FixedPer_kW;
	//init object's base/perKW/surcharge for use in billBreakdown
	myData.base   =  basePrice;
	myData.perKW_Charge = FixedPer_kW;
	myData.surcharge = surcharge;
	return totalCharges;
}


/*
 * Supports billBreakdown feature
 * Shows how the total amount for a bill is calculated
 * Different conditionals for each category
 */
void printBillStructure(const billData &myData, const char& myCategory)
{
	printTextBox("BILL STRUCTURE FOR " + myData.monthName, '-', ROW_SIZE);

	///STEP 1: LOAD INFORMATION BEGINS HERE
	//These fields are common to all categories - A,B,C - so printed in all cases
	cout << setw(30) << "Load in kW" << setw(15) << myData.load << endl;
	cout << setw(30) << "Units" << setw(15) << myData.units << endl;
 	cout << setw(30) << "Supply Voltage in kV" << setw(15) << myData.kiloVolts << endl;

	//Supply system can vary between 1 phase and 3 phase for Res/Com
	cout << setw(30) << "Supply System";
	if (myCategory == 'A' || myCategory == 'B')
	{
		if (myData.isThreePhase)
			cout << setw(15) << "Three Phase" << endl;
		else
			cout << setw(15) << "Single Phase" << endl;
	}
	//All industries are three phase by default
	else
    {
        cout << setw(15) << "Three Phase" << endl;
    }
    //Printing row of '-' to separate LOAD INFO from BILL STRUCTURE
    printRow('-', 45);

	///STEP 2:BILL STRUCTURE BEGINS HERE
	cout << setw(30) << "Base Price - PKR" << setw(15) << myData.base << endl;
	cout << setw(30) << "Surcharge - PKR"  << setw(15) << myData.surcharge << endl;
	cout << setw(30) << "Per kW Charges - PKR" << setw(15) << myData.perKW_Charge << endl;

	//FOR RES, 17.5% GST ON BASE PRICE +
	//1.5 % DUTY ON BASE PRICE
	//+ PHASE CHARGES + TVL + BANK CHARGES + METER RENT
	if (myCategory == 'A')
	{
		cout << setw(30) << "17.5% GST - PKR" << setw(15) << 0.175*myData.base << endl;
		cout << setw(30) << "1.5% Electricity Duty - PKR" << setw(15) << 0.015*myData.base << endl;
		if (myData.isThreePhase)
		{
			cout << setw(30) << "Three Phase Charges - PKR" << setw(15) << 150 << endl;
		}
		else
		{
			cout << setw(30) << "Single Phase Charges - PKR" << setw(15) << 75 << endl;
		}
		cout << setw(30) << "TV License Fees - PKR" << setw(15) << 35.00 << endl;
		cout << setw(30) << "Meter Rent - PKR" << setw(15) << 15.00 << endl;
		cout << setw(30) << "Bank Charges - PKR" << setw(15) << 8.00 << endl;
	}
	else if (myCategory == 'B')
	{
		cout << setw(30) << "17.5% GST - PKR" << setw(15) << 0.175*myData.base << endl;
		cout << setw(30) << "2% Electricity Duty - PKR" << setw(15) << 0.02*myData.base << endl;
		if (myData.isThreePhase)
		{
			cout << setw(30) << "Three Phase Charges - PKR" << setw(15) << 350.00 << endl;
		}
		else
		{
			cout << setw(30) << "Single Phase Charges - PKR" << setw(15) << 175.00 << endl;
		}
		cout << setw(30) << "TV License Fees - PKR" << setw(15) << 60.00 << endl;
		cout << setw(30) << "Meter Rent - PKR" << setw(15) << 15.00 << endl;
		cout << setw(30) << "Bank Charges - PKR" << setw(15) << 8.00 << endl;
	}
	else
	{
		if (myData.load < 5)
		{
			cout << setw(30) << "Fixed Load Charges - PKR" << setw(15) << 350.00 << endl;
		}
		else if (myData.load >= 5 && myData.load <= 500 && myData.kiloVolts == 0.4)
		{
			cout << setw(30) << "Fixed Load Charges - PKR" << setw(15) << 2000.00 << endl;
		}
		else if (myData.load <= 5000 && myData.kiloVolts == 33)
		{
			cout << setw(30) << "Fixed Load Charges - PKR" << setw(15) << 50000.00 << endl;
		}
		else
		{
			cout << setw(30) << "Fixed Load Charges - PKR" << setw(15) << 500000.00 << endl;
		}
		cout << setw(30) << "17.5% GST - PKR" << setw(15) << 0.175*myData.base << endl;
		cout << setw(30) << "1.5% Electricity Duty - PKR" << setw(15) << 0.015*myData.base << endl;
		cout << setw(30) << "TV License Fees - PKR" << setw(15) << 60.00 << endl;
		cout << setw(30) << "Bank Charges - PKR" << setw(15) << 8.00 << endl;
	}
    printRow('-', 45);

	///STEP 3: PRINTING FINAL AMOUNT
	cout << setw(30) << "Gross Payable Amount - PKR" << setw(15) << myData.amount << endl;
    printRow('-', 45);
	printTextBox("END OF BILL STRUCTURE", '-', ROW_SIZE);
}


/*
 * returns minimum and maximum valid month number
 * for a vector of billData - used in billBreakdown
 */
void getRangeFlags(const vector<billData> &myRecord, int &myMinMarker, int &myMaxMarker)
{
	myMinMarker = myRecord[0].month;
	myMaxMarker = myRecord[0].month;
	for (int i = 1; i < myRecord.size(); i++)
	{
		if (myRecord[i].month < myMinMarker)
		{
			myMinMarker = myRecord[i].month;
		}
		if (myRecord[i].month > myMaxMarker)
		{
			myMaxMarker = myRecord[i].month;
		}
	}
}


/*
 * Shows how total cost of bill for a particular month was calculated
 * Gets min/max valid months for which details can be printed
 * Asks user if they wish to see details: Y-> asks for month number -> prints details
 * N-> exits
 */
void billBreakdown(const vector<billData> &myRecord, const char &myCategory)
{
	char proceed;
	int	 monthInput, minMonth, maxMonth;

	getRangeFlags(myRecord, minMonth, maxMonth);        //getting minimum and maximum valid values for monthInput from myRecord
	do                                                  //ask if user wants to see bill details for a specific month
	{
		cout << "Would you like to view the bill structure for a particular month?" << endl;
		cout << "Enter 'Y' for yes, 'N' for no." << endl;
		cin >> proceed;
		if(!validContinueInput(proceed))
		{
			cout << "Invalid input. Please try again." << endl;
		}
	}
	while(!validContinueInput(proceed));                //continue until Y/N input is not valid
	if (proceed == 'Y' || proceed == 'y')
	{
		do                                              //Ask for month number if yes
		{
			cout << "Enter entry number for a month to see it's bill structure." << endl;
			cout << "Entry number must be between " << minMonth << " and " << maxMonth << endl;
			cin >> monthInput;
			if (monthInput < minMonth || monthInput > maxMonth)
			{
				cout << "Invalid input. Please try again." << endl;
			}
		}
		while(monthInput < minMonth || monthInput > maxMonth);

		for (int i = 0; i < myRecord.size(); i++)               //parse userRecord for month matching monthInput
		{
			if (monthInput == myRecord[i].month)
			{
				printBillStructure(myRecord[i], myCategory);    //print details of this object
			}
		}
	}
}


/*
 * Prints bar of * symbols to represent expenditure amount
 * Size determined by amount's value with respect to min and max expenditure in same record
 * Thus prints a scaled graph to represent expenditure
 */
void printBar(const double &myMin, const double &myMax, const double &myAmount)
{
	string bar;
	double scaledValue = ((myAmount - myMin)/(myMax - myMin))*10;       //scaling: min = 0 *s, max = 10*s
	for (int i = 0; i < scaledValue; i++)
	{
		bar += "*";         //concatenation for every i below scaledValue
	}
	if (scaledValue < 1)
	{
		bar+="*";           //fixes bug of no star appearing for minimum value
	}
	cout << "\t" << bar << endl;
}


/*Prints billData object in payment history function*/
void printBillData(const billData &myData)
{
	string statusMessage;               //status message replaces 1/0 for boolean isPaid
	if (myData.isPaid)
	{
		statusMessage = "Paid";
	}
	else
	{
		statusMessage = "NOT PAID";
	}
	cout << setw(5)  << myData.month
		 <<	setw(10) << myData.monthName
		 << setw(10) << myData.year
		 << setw(10) << myData.units
		 << setw(20) << myData.amount
		 << setw(15) << statusMessage;
}

///*-------------------------------------------SERVICE PRIMARY FUNCTIONS---------------------------------------------*/


/*
 * Service A: Payment History
 * shows formatted basic bill information for each month
 * includes category to allow for call to billBreakdown for full bill details
 */
void paymentHistory(const vector<billData> &myRecord, const char &myCategory)
{
	string paidMessage;

	//Getting min/max expenditure for scaling bars later on
	double min = myRecord[0].amount, max = myRecord[0].amount;
	for (int i = 1; i < myRecord.size(); i++)
	{
		if (myRecord[i].amount > max)
		{
			max = myRecord[i].amount;
		}
		if (myRecord[i].amount < min)
		{
			min = myRecord[i].amount;
		}
	}

	//Printing heading for each column of data
	cout << setw(5) << "Entry" << setw(10) << "Month" << setw(10) << "Year" << setw(10) << "Units"
		 <<  setw(20) << "Amount" << setw(15) << "Status" << "\tRelative Spending" << endl;

	//Sequentially printing each element's data from userRecord
	for (int i = 0; i < myRecord.size(); i++)
	{
		printBillData(myRecord[i]);             //numeric data
		printBar(min, max, myRecord[i].amount); //string of *s
	}
	billBreakdown(myRecord, myCategory);        	//ask if bill structure needed
}


/*
 * Service B: Bill Payment
 * Parses record for last unpaid bill - asks user if payment is to be made
 * If yes, changes status of unpaid bill object accordingly
 */
void billPayment(vector<billData> &myRecord, const char &myCategory)
{
	bool unpaidBill = false;            //if no unpaid bill, remains false -> skips to end
	char proceed;                       //used for Y/N input at several stages in this function
	int  lastUnpaidIndex;               //holds index for last billData object with isPaid == 0

	for (int i = 0; i < myRecord.size(); i++)               //Parsing userRecord to see if unpaid exists
	{
		if (myRecord[i].isPaid == false)
		{
			unpaidBill = true;
		}
	}

	if (unpaidBill)                                         //if unpaid exists, find index
	{
		cout << "The following payment(s) are due:\t" << endl;
		printRow('-', ROW_SIZE);
		cout << setw(5) << "Entry" << setw(10) << "Month" << setw(10) << "Year" << setw(10)
			 << "Units" <<  setw(20) << "Amount" << setw(15) << "Status" << endl;
		for (int i = 0; i < myRecord.size(); i++)
		{
			if (myRecord[i].isPaid == false)
			{
				printBillData(myRecord[i]);
				lastUnpaidIndex = i;
				cout << endl;
			}
		}
		printRow('-', ROW_SIZE);
        do                                                 //ask if user would like to make payment
		{
			cout << "Would you like to make a payment for the most recent bill?" << endl;
			cout << "If yes, enter 'Y'. Otherwise, enter 'N'." << endl;
			cin >> proceed;
			if (!validContinueInput(proceed))
			{
				cout << "Invalid entry. Please try again." << endl;
			}
		}
		while(!validContinueInput(proceed));
		if (proceed == 'Y' || proceed == 'y')
		{
			do
			{
                //echoing amount to be paid so user knows what they are doing
				cout << "Make payment of PKR "  << myRecord[lastUnpaidIndex].amount
			     << " for month of " << myRecord[lastUnpaidIndex].monthName << " "
			     << myRecord[lastUnpaidIndex].year << "?" << endl;
				cout << "If yes, enter 'Y'. Otherwise, enter 'N'." << endl;
				cin >> proceed;
				if (!validContinueInput(proceed))
				{
					cout << "Invalid input. Please try again." << endl;
				}
			}
			while(!validContinueInput(proceed));
			if (proceed == 'Y' || proceed == 'y')
			{
				myRecord[lastUnpaidIndex].isPaid = 1;           //payment made - change isPaid for this object to 1
				printTextBox("PAYMENT SUCCESSFUL", '-', ROW_SIZE);
				cout << "Your current payment history is as follows:" << endl;
				paymentHistory(myRecord, myCategory);           //echo payment history to show change has been made
			}
		}
	}
	else        //nothing to be done if no bill to be paid i.e. flag stays false
	{
		cout << "All dues cleared." << endl;
	}
}
///*==========================================FUNCTION DEFINITIONS============================================*/
