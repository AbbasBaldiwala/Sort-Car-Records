#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;
const int MAX_NUM_RECORDS = 30,
SETW_ID = 15, SETW_MODEL = 9, SETW_QUANTITY = 12, SETW_PRICE = 14, SETPRECISION = 2,
TABLE_LENGTH = SETW_ID + SETW_MODEL + SETW_PRICE + SETW_QUANTITY,
MIN_PRICE = 5000, MIN_QUANTITY = 0, MIN_MODEL_LEN = 3,
ID_CRIT_1_NUM_LETTERS = 2, ID_CRIT_2_NUM_ALPHANUMERIC = 4, ID_CRIT_3_NUM_DIGITS = 3,
CORRECT_ID_LEN = ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC + ID_CRIT_3_NUM_DIGITS,
LETTER_OFFSET_IN_ASCII = 32;
const double SEARCH_PRICE_RANGE = 0.15;

class CarRecord {
public:
	CarRecord() { SetRecord("", "", 0, 0); }

	void SetRecord(string ID, string modelName, int quantityOnHand, double cost);

	string ToString() const; //returns formatted output as a string

	double GetPrice() const { return price; }

	string GetModel() const { return model; }

	string GetID() const { return carID; }

	int GetQuantity() const { return quantity; }

private:
	string carID, model;
	int quantity;
	double price;
};

class Inventory {
public:

	Inventory() { SetNumRec(0); }
	
	void AddRecord(CarRecord rec);

	void SetNumRec(int numRecords) { numRecs = numRecords; }

	CarRecord* GetInventory() { return inventory; }

	int GetNumRecs() const { return numRecs; }

	void SortInventory(int sortOption);

	void PrintUnsortedInventory(string border, string header) const;

	void PrintSortedInventory(string border, string header) const;

private:
	CarRecord inventory[MAX_NUM_RECORDS];
	CarRecord* pInventory[MAX_NUM_RECORDS];
	int numRecs;
};

enum Menu {
	PRINT_VALID = 1, PRINT_INVALID = 2, RETRIEVE_RECORD = 3, SORT = 4, QUIT = 5
};

enum SearchMenu {
	SEARCH_CAR_ID_OR_NAME = 1, SEARCH_PRICE = 2, SEARCH_GO_BACK = 3, SEARCH_QUIT = 4
};

enum SortMenu {
	SORT_CAR_ID = 1, SORT_MODEL_NAME = 2, SORT_QUANTITY = 3, SORT_PRICE = 4, SORT_GO_BACK = 5, SORT_QUIT = 6
};


//LAB #1
//Gets the data from the input file and writes all invalid data to error file
void GetData(Inventory& inventory, int& numInvalidRec, string errorFileName, string inputFileName);

//prints invalid records to screen in table format
void PrintInvalidRecords(string border, string header, string errorFileName, int numInvalid);

bool IsValidID(string carID, string& errorMessage); //checks if the Car ID is valid

bool IsValidModel(string model, string& errorMessage); //checks if the Model Name is valid

bool IsValidQuantity(int quantity, string& errorMessage); //checks if the quantity is valid

bool IsValidPrice(double price, string& errorMessage); //checks if the price is valid

bool IsValidRecord(string id, string model, int quantity, double price, string& errMsg); //checks if the record is valid

string ToUpper(string str); //returns string converted to upper case

void ClearInvalidInput(string errMsg); //clears cin, clears the keyboard buffer, prints an error message


//LAB #2
//Creates the submenu for searching for a record
void EnterSearchSubMenu(Inventory inventory, int& userChoiceMainMenu, string border, string header);

//searches the records by Car ID or Model Name
string SearchByIdOrModel(Inventory inventory, string keyWord);

//searches the records by price
string SearchByPrice(Inventory inventory, double lowerBound, double upperBound);

//Outputs any records that matches the search
void PrintSearchResultsIdOrModel(Inventory inventory, string border, string header);

//Outputs any record that matches the search
void PrintSearchResultsPrice(Inventory inventory, string border, string header);


//LAB #5
//Creates the submenu for sorting the inventory
void EnterSortSubmenu(Inventory& inventory, int& userChoiceMainMenu, string border, string header);



int main() {
	int userChoiceMainMenu, numInvalidRec = 0;
	stringstream borderSS, headerSS;
	Inventory inventory;
	string inputFileName = "test.txt", errorFileName = "InvalidRecords.txt", border, header;
	GetData(inventory, numInvalidRec, errorFileName, inputFileName);

	cout << fixed << setprecision(SETPRECISION);

	borderSS << setfill('-') << setw(TABLE_LENGTH) << "";
	headerSS << setw(SETW_ID) << left << "Car ID" << setw(SETW_MODEL) << "Model" << setw(SETW_QUANTITY) << right << "Quantity" << setw(SETW_PRICE + 1) << "Price\n" << endl;

	border = borderSS.str();
	header = headerSS.str();

	do {
		cout << "\n\nMENU: \n"
			"1. PRINT VALID RECORDS\n"
			"2. PRINT INVALID RECORDS\n"
			"3. SEARCH FOR CAR RECORD\n"
			"4. SORT\n"
			"5. QUIT\n\n";
		cin >> userChoiceMainMenu;
		switch (userChoiceMainMenu) {
		case PRINT_VALID:
			inventory.PrintUnsortedInventory(border, header);
			break;
		case PRINT_INVALID:
			PrintInvalidRecords(border, header, errorFileName, numInvalidRec);
			break;
		case RETRIEVE_RECORD:
			EnterSearchSubMenu(inventory, userChoiceMainMenu, border, header);
			break;
		case SORT:
			EnterSortSubmenu(inventory, userChoiceMainMenu, border, header);
		case QUIT:
			break;
		default:
			ClearInvalidInput("INVALID MENU SELECTION");
		}
	} while (userChoiceMainMenu != QUIT);
	cout << "\nQUITTING..." << endl;
	return 0;
}



//CAR RECORD METHODS
void CarRecord::SetRecord(string ID, string modelName, int quantityOnHand, double cost) {
	carID = ID;
	model = modelName;
	quantity = quantityOnHand;
	price = cost;
}

string CarRecord::ToString() const {
	stringstream ss;
	ss << setw(SETW_ID) << left << carID
		<< setw(SETW_MODEL) << model
		<< setw(SETW_QUANTITY) << right << quantity
		<< setw(SETW_PRICE) << fixed << setprecision(SETPRECISION) << price;
	return ss.str();
}


//INVENTORY METHODS
void Inventory::AddRecord(CarRecord rec) {
	numRecs++;
	inventory[numRecs - 1] = rec;
	pInventory[numRecs - 1] = &inventory[numRecs - 1];
	
}

void Inventory::SortInventory(int sortOption) {
	for (int i = 0; i < numRecs - 1; i++) {
		int maxIndex = i;
		for (int j = i + 1; j < numRecs; j++) {
			bool toSwap = false;

			switch (sortOption) {
			case SORT_CAR_ID:
				toSwap = pInventory[j]->GetID() > pInventory[maxIndex]->GetID();
				break;
			case SORT_MODEL_NAME:
				toSwap = pInventory[j]->GetModel() > pInventory[maxIndex]->GetModel();
				break;
			case SORT_QUANTITY:
				toSwap = pInventory[j]->GetQuantity() > pInventory[maxIndex]->GetQuantity();
				break;
			case SORT_PRICE:
				toSwap = pInventory[j]->GetPrice() > pInventory[maxIndex]->GetPrice();
				break;
			}

			if (toSwap) {
				maxIndex = j;
			}
		}

		if (maxIndex != i) {
			CarRecord* pTemp = pInventory[i];
			pInventory[i] = pInventory[maxIndex];
			pInventory[maxIndex] = pTemp;
		}
	}
}

void Inventory::PrintUnsortedInventory(string border, string header) const  {
	if (numRecs == 0) {
		cout << "NO VALID RECORDS\n";
	}
	else {
		cout << border << "\n" << header;
		for (int i = 0; i < numRecs; i++) {
			cout << inventory[i].ToString() << "\n";
		}
		cout << border;
	}
}

void Inventory::PrintSortedInventory(string border, string header) const {
	if (numRecs == 0) {
		cout << "NO VALID RECORDS\n";
	}
	else {
		cout << border << "\n" << header;
		for (int i = 0; i < numRecs; i++) {
			cout << pInventory[i]->ToString() << "\n";
		}
		cout << border;
	}
}


//LAB #1
void GetData(Inventory& inventory, int& numInvalidRec, string errorFileName, string inputFileName) {
	string tempCarID, tempModel, errMsg;
	int tempQuantity;
	inventory.SetNumRec(0);
	double tempPrice;
	CarRecord tempRec;
	ifstream inFile(inputFileName);
	ofstream errorFile(errorFileName);
	if (!inFile) {
		cout << "Input file not found. Exiting the program." << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	if (inFile.peek() == EOF) {
		cout << "The input file is empty. Quitting the program." << endl;
		inFile.close();
		system("pause");
		exit(EXIT_FAILURE);
	}
	while (!inFile.eof() && inventory.GetNumRecs() < MAX_NUM_RECORDS) {
		inFile >> tempCarID >> tempModel >> tempQuantity >> tempPrice;
		tempCarID = ToUpper(tempCarID);
		tempModel = ToUpper(tempModel);
		errMsg = "";
		if (IsValidRecord(tempCarID, tempModel, tempQuantity, tempPrice, errMsg)) { //separates valid and invalid records
			tempRec.SetRecord(tempCarID, tempModel, tempQuantity, tempPrice);
			inventory.AddRecord(tempRec);
			//numValidRec++;
		}
		else {
			errorFile << setw(SETW_ID) << left << tempCarID
				<< setw(SETW_MODEL) << tempModel
				<< setw(SETW_QUANTITY) << right << tempQuantity
				<< setw(SETW_PRICE) << fixed << setprecision(SETPRECISION) << tempPrice << " " << errMsg << "\n";
			numInvalidRec++;
		}
	}

	if (inventory.GetNumRecs() == MAX_NUM_RECORDS && !inFile.eof()) {
		cout << "Not all Records have been stored; only the first " << MAX_NUM_RECORDS << " were processed." << "\n";
	}
	inFile.close();
	errorFile.close();
}

void PrintInvalidRecords(string border, string header, string errorFileName, int numInvalidRec) {
	ifstream invalidRecFile(errorFileName);
	if (invalidRecFile.peek() == EOF) {
		cout << "\nNO INVALID RECORDS FOUND\n";
	}
	else {
		cout << "\nDISPLAYING " << numInvalidRec << " INVALID RECORDS...\n"
			<< border << "\n"
			<< header;
		string line;
		while (getline(invalidRecFile, line)) {
			cout << line << "\n";
		}
		cout << border << "\n";
	}
	invalidRecFile.close();
}

bool IsValidID(string carID, string& errorMessage) {
	bool meetsIDCriteria = true;
	string idErrorMessage = "";
	if (carID.length() != CORRECT_ID_LEN)
	{
		meetsIDCriteria = false;
		idErrorMessage += " Must be " + to_string(CORRECT_ID_LEN) + " characters long.";
	}
	else {
		for (int i = 0; i < ID_CRIT_1_NUM_LETTERS; i++) {
			char ch = carID[i];
			if (!(((ch >= 'A') && (ch <= 'Z'))) || ch == 'O' || ch == 'o') { //checks if ID meets criteria 1
				meetsIDCriteria = false;
				idErrorMessage += " First " + to_string(ID_CRIT_1_NUM_LETTERS) + " characters must be Alpha(A-Z, letter O is not allowed).";
				i = ID_CRIT_1_NUM_LETTERS;
			}
		}
		for (int i = ID_CRIT_1_NUM_LETTERS; i < (ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC); i++) { //checks if ID meets criteria 2
			char ch = carID[i];
			if (!((((ch >= 'A') && (ch <= 'Z'))) || ((ch >= '0') && (ch <= '9'))) || ch == 'O') {
				meetsIDCriteria = false;
				idErrorMessage += " Characters " + to_string(ID_CRIT_1_NUM_LETTERS + 1) +
					"-" + to_string(ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC) +
					" must be alphanumeric(A-Z, letter O is not allowed).";
				i = ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC;
			}
		}
		for (int i = (ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC); i < CORRECT_ID_LEN; i++) { //checks if ID meets criteria 3
			char ch = carID[i];
			if (!(ch >= '0' && ch <= '9')) {
				meetsIDCriteria = false;
				idErrorMessage += " Characters " + to_string(ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC + 1) +
					"-" + to_string(CORRECT_ID_LEN) + " must be numeric (0-9).";
				i = CORRECT_ID_LEN;
			}
		}
	}

	if (!meetsIDCriteria) {
		errorMessage += "INVALID ID [" + idErrorMessage + "] ";
	}
	return meetsIDCriteria;
}

bool IsValidModel(string model, string& errorMessage) {
	bool meetsModelCriteria = true;
	string modelErrorMessage = "";
	if (model.length() < MIN_MODEL_LEN) {
		meetsModelCriteria = false;
		modelErrorMessage += "Must be at least " + to_string(MIN_MODEL_LEN) + " characters long.";
	}
	if (model[0] < 'A' || model[0] > 'Z') { //checks if Model meets criteria 1
		meetsModelCriteria = false;
		modelErrorMessage += " Must start with a letter.";
	}
	size_t len = model.length();
	for (size_t i = 1; i < len; i++) { //checks if Model meets criteria 2
		char ch = model[i];
		if (!((((ch >= 'A') && (ch <= 'Z'))) || ((ch >= '0') && (ch <= '9')))) {
			meetsModelCriteria = false;
			modelErrorMessage += " Must consist of only alphanumeric characters.";
			i = len;
		}
	}
	if (!meetsModelCriteria) {
		errorMessage += "INVALID MODEL [" + modelErrorMessage + "] ";
	}
	return meetsModelCriteria;
}

bool IsValidQuantity(int quantity, string& errorMessage) {
	if (quantity < MIN_QUANTITY) {
		errorMessage += "INVALID QUANTITY [Can't be less than " + to_string(MIN_QUANTITY) + ".] ";
	}
	return quantity >= MIN_QUANTITY;
}

bool IsValidPrice(double price, string& errorMessage) {
	if (price <= MIN_PRICE) {
		errorMessage += "INVALID PRICE [Must be greater than $" + to_string(MIN_PRICE) + ".] ";
	}
	return price > MIN_PRICE;
}

bool IsValidRecord(string id, string model, int quantity, double price, string& errMsg) {
	bool isValidID, isValidModel, isValidQuantity, isValidPrice;

	isValidID = IsValidID(id, errMsg);
	isValidModel = IsValidModel(model, errMsg);
	isValidQuantity = IsValidQuantity(quantity, errMsg);
	isValidPrice = IsValidPrice(price, errMsg);

	return isValidID && isValidModel && isValidQuantity && isValidPrice;
}

string ToUpper(string str) {
	string upperCaseStr = "";
	for (int i = 0; i < str.length(); i++) {
		char ch = str[i];
		if (((ch >= 'a') && (ch <= 'z'))) {
			ch -= LETTER_OFFSET_IN_ASCII; //converts char to upper case
		}
		upperCaseStr += ch;
	}
	str = upperCaseStr;
	return upperCaseStr;
}

void ClearInvalidInput(string errMsg) {
	cout << "\n" << errMsg << "\n";
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}


//LAB #2
string SearchByIdOrModel(Inventory inventory, string keyWord) {
	string recordsFound = "";
	int numRecs = inventory.GetNumRecs();
	for (int i = 0; i < numRecs; i++) {
		string tempModel = inventory.GetInventory()[i].GetModel(),
			tempID = inventory.GetInventory()[i].GetID();
		if (tempModel.find(keyWord) != std::string::npos || tempID.find(keyWord) != std::string::npos) {
			recordsFound += inventory.GetInventory()[i].ToString() + "\n";
		}
	}
	return recordsFound;
}

string SearchByPrice(Inventory inventory, double lowerBound, double upperBound) {
	string recordsFound = "";
	int numRecs = inventory.GetNumRecs();
	for (int i = 0; i < numRecs; i++) {
		if (inventory.GetInventory()[i].GetPrice() <= upperBound && inventory.GetInventory()[i].GetPrice() >= lowerBound) {
			recordsFound += inventory.GetInventory()[i].ToString() + "\n";
		}
	}

	return recordsFound;
}

void PrintSearchResultsPrice(Inventory inventory, string border, string header) {
	double searchPrice;
	string recordsFound = "";
	cout << "\nEnter the price you are looking for(Range +/- " << SEARCH_PRICE_RANGE * 100 << " %): \n";
	while (!(cin >> searchPrice)) {
		ClearInvalidInput("INVALID INPUT: You must enter a number.\nRE-ENTER:");
	}
	double lowerBound = searchPrice * (1 - SEARCH_PRICE_RANGE), upperBound = searchPrice * (1 + SEARCH_PRICE_RANGE);

	recordsFound += SearchByPrice(inventory, lowerBound, upperBound);

	if (recordsFound != "") {
		cout << "\nRecords within the price range of " << "$ " << lowerBound << " to $ " << upperBound << " found: \n"
			<< border << "\n"
			<< header
			<< recordsFound
			<< border;
	}
	else {
		cout << "\nNo Records within the price range of " << "$ " << lowerBound << " to $ " << upperBound << " found\n";
	}
}

void PrintSearchResultsIdOrModel(Inventory inventory, string border, string header) {
	string keyWord, recordsFound = "";
	cout << "\nEnter the keyword you want to search by: \n";
	cin >> keyWord;
	keyWord = ToUpper(keyWord);

	recordsFound += SearchByIdOrModel(inventory, keyWord);

	if (recordsFound != "") {
		cout << "\nRecords containing \"" << keyWord << "\" found: \n"
			<< border << "\n"
			<< header
			<< recordsFound
			<< border;
	}
	else {
		cout << "\nNo Records containing \"" << keyWord << "\" found\n";
	}
}

void EnterSearchSubMenu(Inventory inventory, int& userChoiceMainMenu, string border, string header) {
	int userChoiceSearchMenu;
	do {
		cout << "\n\nSEARCH BY: \n"
			"1. CAR ID OR MODEL NAME\n"
			"2. PRICE\n"
			"3. GO BACK\n"
			"4. QUIT\n\n";
		cin >> userChoiceSearchMenu;
		switch (userChoiceSearchMenu) {
		case SEARCH_CAR_ID_OR_NAME:
			PrintSearchResultsIdOrModel(inventory, border, header);
			break;
		case SEARCH_PRICE:
			PrintSearchResultsPrice(inventory, border, header);
			break;
		case SEARCH_GO_BACK:
			cout << "GOING BACK TO MAIN MENU\n";
			break;
		case SEARCH_QUIT:
			userChoiceMainMenu = QUIT;
			break;
		default:
			ClearInvalidInput("INVALID MENU SELECTION");
		}
	} while (!(userChoiceSearchMenu == SEARCH_GO_BACK || userChoiceMainMenu == QUIT));
}


//LAB #5
void EnterSortSubmenu(Inventory& inventory, int& userChoiceMainMenu, string border, string header) {
	int userChoiceSortMenu;

	do {
		cout << "\n\nSORT BY: \n"
			"1. CAR ID\n"
			"2. MODEL NAME\n"
			"3. QUANTITY\n"
			"4. PRICE\n"
			"5. GO BACK\n"
			"6. QUIT\n\n";
		cin >> userChoiceSortMenu;
		
		switch (userChoiceSortMenu) {
		case SORT_CAR_ID:
		case SORT_MODEL_NAME:
		case SORT_QUANTITY:
		case SORT_PRICE:
			inventory.SortInventory(userChoiceSortMenu);
			inventory.PrintSortedInventory(border, header);
			break;
		case SORT_GO_BACK:
			cout << "GOING BACK TO MAIN MENU\n";
			break;
		case SORT_QUIT:
			userChoiceMainMenu = QUIT;
			break;
		default:
			ClearInvalidInput("INVALID MENU SELECTION");
		}
	} while (!(userChoiceSortMenu == SORT_GO_BACK || userChoiceMainMenu == QUIT));
}


//TEST 1 (Sort by ID)
/*


MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. SORT
5. QUIT

4


SORT BY:
1. CAR ID
2. MODEL NAME
3. QUANTITY
4. PRICE
5. GO BACK
6. QUIT

1
--------------------------------------------------
Car ID         Model        Quantity         Price

PLT6HJ764      LAFERRARI          20      16780.24
LP8E5V159      CAMERO              0      72380.81
LAX9NN745      MODELX             30      41490.00
KFUSG6765      FUSION5            22      19975.12
KDTYGA123      COROLLA6            0      22490.99
JGR5YG258      PRIUS              15      20090.54
IAT67T964      SENTRA            110      25840.99
HTG6T5679      LOTUS4              2     134535.73
DSN0DG715      CROSSOVER          75      26700.73
ALY13M398      BEATLE             90      19750.11
--------------------------------------------------

SORT BY:
1. CAR ID
2. MODEL NAME
3. QUANTITY
4. PRICE
5. GO BACK
6. QUIT

5
GOING BACK TO MAIN MENU


MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. SORT
5. QUIT

5

QUITTING...
*/

//TEST 2 (Sort by model)
/*

MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. SORT
5. QUIT

4


SORT BY:
1. CAR ID
2. MODEL NAME
3. QUANTITY
4. PRICE
5. GO BACK
6. QUIT

2
--------------------------------------------------
Car ID         Model        Quantity         Price

IAT67T964      SENTRA            110      25840.99
JGR5YG258      PRIUS              15      20090.54
LAX9NN745      MODELX             30      41490.00
HTG6T5679      LOTUS4              2     134535.73
PLT6HJ764      LAFERRARI          20      16780.24
KFUSG6765      FUSION5            22      19975.12
DSN0DG715      CROSSOVER          75      26700.73
KDTYGA123      COROLLA6            0      22490.99
LP8E5V159      CAMERO              0      72380.81
ALY13M398      BEATLE             90      19750.11
--------------------------------------------------

SORT BY:
1. CAR ID
2. MODEL NAME
3. QUANTITY
4. PRICE
5. GO BACK
6. QUIT

6

QUITTING...
*/

//TEST #3 (Sort by quantity)
/*
MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. SORT
5. QUIT

4


SORT BY:
1. CAR ID
2. MODEL NAME
3. QUANTITY
4. PRICE
5. GO BACK
6. QUIT

3
--------------------------------------------------
Car ID         Model        Quantity         Price

IAT67T964      SENTRA            110      25840.99
ALY13M398      BEATLE             90      19750.11
DSN0DG715      CROSSOVER          75      26700.73
LAX9NN745      MODELX             30      41490.00
KFUSG6765      FUSION5            22      19975.12
PLT6HJ764      LAFERRARI          20      16780.24
JGR5YG258      PRIUS              15      20090.54
HTG6T5679      LOTUS4              2     134535.73
LP8E5V159      CAMERO              0      72380.81
KDTYGA123      COROLLA6            0      22490.99
--------------------------------------------------

SORT BY:
1. CAR ID
2. MODEL NAME
3. QUANTITY
4. PRICE
5. GO BACK
6. QUIT

sdafa

INVALID MENU SELECTION


SORT BY:
1. CAR ID
2. MODEL NAME
3. QUANTITY
4. PRICE
5. GO BACK
6. QUIT

6

QUITTING...

*/

//TEST #4 (sort by price)
/*

MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. SORT
5. QUIT

4


SORT BY:
1. CAR ID
2. MODEL NAME
3. QUANTITY
4. PRICE
5. GO BACK
6. QUIT

4
--------------------------------------------------
Car ID         Model        Quantity         Price

HTG6T5679      LOTUS4              2     134535.73
LP8E5V159      CAMERO              0      72380.81
LAX9NN745      MODELX             30      41490.00
DSN0DG715      CROSSOVER          75      26700.73
IAT67T964      SENTRA            110      25840.99
KDTYGA123      COROLLA6            0      22490.99
JGR5YG258      PRIUS              15      20090.54
KFUSG6765      FUSION5            22      19975.12
ALY13M398      BEATLE             90      19750.11
PLT6HJ764      LAFERRARI          20      16780.24
--------------------------------------------------

SORT BY:
1. CAR ID
2. MODEL NAME
3. QUANTITY
4. PRICE
5. GO BACK
6. QUIT

6

QUITTING...
*/