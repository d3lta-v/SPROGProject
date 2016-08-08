// ---------------------------------------
// FIDB Database Management System v0.1a
// Made by Pan Ziyue, Sean Tin,
// Copyright 2016 (C) All Rights Reserved
// ---------------------------------------

#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

using namespace std;

// Global variables
// Primary database in RAM, constrained to 20 by 20 entries, totalling 8kb
char database[20][20][20] = {0x0}; //init to all NULL values for safety
char databaseName[20];
// Database row and column counts
int rowCount = 0;
int columnCount = 0;

// Function prototypes
void printInitialisationMessage();
int choiceSelection();
void startDBWithFileName();
void initCommandForDatabaseFile(fstream& database);
void printDBInRAM();
void addColumn();
void addRow();
void commitChanges();

char* sanitisedString(char* string);
int fileExists(fstream& pFile);
int fileEmpty(fstream& pFile);

void printHelpFile();
void printSeparator(int columnCount);

int main() {
    printInitialisationMessage();
    int choice = choiceSelection();
    switch (choice) {
        case 1:
        startDBWithFileName();
        break;
        case 2:
        printHelpFile();
        // For case 3 (exit), we do not need to have a condition here as the switch statement will fall through
    }
    return 0;
}

// Program primary functions

void printInitialisationMessage() {
    cout << "----------------------------------------------------\n";
    cout << " FIDB Flatfile CSV Database Management System v0.1a\n";
    cout << " Copyright (C) 2016 All Rights Reserved\n";
    cout << "----------------------------------------------------\n" << endl;
}

int choiceSelection() {
    int choice;
    cout << "Select command to perform:\n";
    cout << "1. Start DB Manager\n2. Show help file\n3. Exit\nChoice: ";
    while (choice != 1 || choice != 2 || choice != 3) {
        cin >> choice;
        if (choice==1||choice==2||choice==3) {
            return choice;
        } else {
            cout << "Invalid choice. Please re-enter choice: ";
        }
    }
}

void startDBWithFileName() {
    cout << "\nPlease enter file name of database file to manage (maximum 20 characters): ";
    cin >> databaseName;
    fstream dbFile; // initialise flatfile database
    fstream testDbFile(databaseName);

    // Check if db file exists and if not prompt to create new file
    if (!fileExists(testDbFile)) {
        char choice;
        cout << "File does not exist! Create empty database? (y/n): ";
        while (choice != 'y' || choice != 'n') {
            cin >> choice;
            if (choice == 'y') {
                // Make new empty file
                dbFile.open(databaseName);
                dbFile.close();
                break;
            } else if (choice == 'n') {
                cout << "Quitting database manager..." << endl;
                return; // quit the entire program directly
            } else {
                cout << "Invalid choice. Please re-enter choice: ";
            }
        }
    }
    testDbFile.close();

    // Start the manager proper

    // Check if file is empty
    dbFile.open(databaseName);
    if (fileEmpty(dbFile)) {
        cout << "\nNOTE: Database file is empty" << endl;
    } else {
        // Parse and insert database into RAM
        // Getting by rows
        string line;
        while (getline(dbFile, line)) {
            int localColumnCount = 0; // reset column count every time when the program gets to a new row
            // Turn the string into a C string (char array) for compliance with the cstring library
            char lineWithCString[100]; //I have to use a 100 for this as anything lower will cause an array out of bounds
            strcpy(lineWithCString, line.c_str());
            char * pch; //pointer to the position of characters split by a delimiter
            pch = strtok(lineWithCString, " "); //space as a delimiter, and yes, we need " " because it accepts only a const char *, NOT const char (in other words, char array, not char)
            // Getting by columns
            while (pch != NULL) {
                strcpy(database[rowCount][localColumnCount], pch);
                localColumnCount++;
                if (rowCount == 0) { // if it's first row, we populate the global column count as well
                    columnCount++;
                }
                pch = strtok(NULL, " ");
            }
            rowCount++;
        }
    }

    // Enter command mode for database
    initCommandForDatabaseFile(dbFile);
}

void initCommandForDatabaseFile(fstream& database) {
    char commandBuffer[30] = {0x0}; // maximum 30 characters in a single command
    cout << "\nDatabase loaded, command mode initialised\n";
    while (1) {
        cout << "> ";
        cin >> commandBuffer;
        // Parse the command the user entered, using strncmp for string comparison
        if (strncmp(commandBuffer, "HELP", 4) == 0) {
            cout << "Help file not implemented" << endl;
        } else if (strncmp(commandBuffer, "EXIT", 4) == 0 || strncmp(commandBuffer, "QUIT", 4) == 0) {
            cout << "Quitting database manager..." << endl;
            break;
        } else if (strncmp(commandBuffer, "DUMPDB", 6) == 0) {
            printDBInRAM();
        } else if (strncmp(commandBuffer, "ADDCOL", 6) == 0) {
            // Add column
            addColumn();
        } else if (strncmp(commandBuffer, "ADDROW", 6) == 0) {
            // Add row
            addRow();
        } else if (strncmp(commandBuffer, "MODIFY", 6) == 0) {
            // Modify at row/column

        } else if (strncmp(commandBuffer, "COMMIT", 6) == 0) {
            // Commit changes to file
            commitChanges();
        } else {
            cout << "Invalid command. Please consult the operator's manual for more information\n";
        }
        strcpy(commandBuffer, "\0"); // clear buffer
    }
}

// Command functions

void printDBInRAM() {
    cout << "\nRow count: " << rowCount << endl;
    cout << "Column count: " << columnCount << endl << endl;

    printSeparator(columnCount);
    for (int i = 0; i < rowCount; i++) {
        cout << "|";
        for (int j = 0; j < columnCount; j++) {
            cout << setw(20) << database[i][j] << setfill(' ') << " | ";
        }
        if (i==0 && rowCount > 1) {
            // Add a separator at the first row
            cout << "\n";
            printSeparator(columnCount);
        } else {
            cout << "\n";
        }
    }
    printSeparator(columnCount);
    cout << endl;
}

void addColumn() {
    char identifier[20];
    cout << "Enter column identifier: ";
    cin >> identifier;
    columnCount++;
    if (rowCount == 0) {
        // A column MUST exist with a row!
        rowCount++;
    }
    strcpy(database[0][columnCount-1], identifier);
    cout << "Column added with identifier " << identifier << endl;
}

void addRow() {
    if (columnCount > 0) { // if there are columns
        // proceed with adding a row
        char identifier[20];
        cout << "Enter row identifier: ";
        cin >> identifier;
        rowCount++;
        strcpy(database[rowCount-1][0], identifier);
        cout << "Row added with identier " << identifier << endl;
    } else {
        cout << "No columns to add rows to!";
    }
}

void modify() {
    //NOTE: row and column are indexed from 1, NOT 0!
    int row, column;
    cout << "Input row to modify: ";
    cin >> row;
    if (row > rowCount) {
        cout << "Row number is larger than total row count. Please create a new row first";
        return; //exit from the function
    }
    cout << "Input column to modify: ";
    cin >> column;
    if (column > columnCount) {
        cout << "Column number is larger than total row count. Please create a new column first";
        return;
    }
    //TODO:finish the modify function!
}

void commitChanges() {
    // Construct a single string that will write to the file, this process is essentially serialisation
    char flatStringForFile[500] = {0x0};
    int currentFlatStrIndex = 0;
    // For loop for rows
    for (size_t i = 0; i < rowCount; i++) {
        // For loop for columns
        for (size_t j = 0; j < columnCount; j++) {
            // For loop for characters
            for (size_t c = 0; c < 20; c++) { //max 20 characters, have to hardcode due to limitations of C-style character arrays
                if (database[i][j][c] != '\0') {
                    flatStringForFile[currentFlatStrIndex] = database[i][j][c];
                    currentFlatStrIndex++;
                } else {
                    // next column
                    flatStringForFile[currentFlatStrIndex] = ' ';
                    currentFlatStrIndex++;
                    break;
                }
            }
        }
        flatStringForFile[currentFlatStrIndex-1] = '\n';
    }

    cout << "Database preview (raw): \n" << flatStringForFile << endl;
    // Prompt user for confirmation
    char confirmation;
    cout << "Confirm commit to database? (y/n): ";
    cin >> confirmation;
    if (confirmation == 'y') {
        // commit changes to database
        fstream databaseFile;
        databaseFile.open(databaseName, fstream::out | fstream::trunc); //fstream::trunc as we need to wipe the previous file clean
        databaseFile << flatStringForFile;
        databaseFile.close();
        cout << "Finished writing to database" << endl;
    } else if (confirmation == 'n') {
        cout << "Not writing to database" << endl;
    } else {
        cout << "Invalid input. Assuming no\n";
        cout << "Not writing to database" << endl;
    }
}

// Helper functions

/*
    The input sanitiser "sanitises" all character arrays by making sure that the last character is null terminated
    This method is useful for sanitising database inputs

    - parameter string: the string to "sanitise"
    - returns: a string as a character array
*/
char* sanitisedString(char* string) {
    string[strlen(string)-1] = '\0';
    return string;
}

/*
    This method returns a boolean (int) that indicates if a file exists.

    - parameter pFile: the fstream to check if the file exists
    - returns: a boolean (int) stating if the file exists. 0 = does not exist, 1 = exists
*/
int fileExists(fstream& pFile) {
    return pFile.good();
}

/*
    This method returns a boolean (int) that indicates if a file is empty

    - parameter pFile: the fstream to check if the file is empty
    - returns: a boolean (int) stating if the file is empty (i.e. EOF character was found)
*/
int fileEmpty(fstream& pFile) {
    return pFile.peek() == fstream::traits_type::eof();
}

void printHelpFile() {
    cout << "Help file not implemented yet" << endl;
}

/*
    Prints the appropriate number of '-' characters for a certain number of columns

    - parameter columnCount: the number of columns to print a separator for
*/
void printSeparator(int columnCount) {
    for (int i = 0; i < 23*columnCount+1; i++) {
        if (i == 23*columnCount) {
            cout << "\n";
        } else {
            cout << "-";
        }
    }
}
