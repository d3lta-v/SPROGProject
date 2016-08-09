// ------------------------------------------------------
// FIDB Database Management System v0.1a
// Made by Pan Ziyue, Sean Tin, Chai Yaw, Adrian, Jinyao
// Copyright 2016 (C) All Rights Reserved
// ------------------------------------------------------

#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

using namespace std;

// Global variables
// Primary database in RAM, constrained to 20 by 20 entries, totalling 8kb
char database[20][20][20] = {0x0}; //init to all NULL values for safety
char databaseName[20]; //global storage for the database name
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
void delColumn();
void delRow();
void modify();
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

/// Starting choice selection for the user
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

/// Starting point of the main program
void startDBWithFileName() {
    cout << "\nPlease enter file name of database file to manage (maximum 20 characters): ";
    cin >> databaseName;
    fstream dbFile; // initialise flatfile database
    fstream testDbFile(databaseName); // This is only for testing if the file exists

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

    // Start the manager here

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

/// Starts the command mode for the database, for an fstream
void initCommandForDatabaseFile(fstream& database) {
    char commandBuffer[30] = {0x0}; // maximum 30 characters in a single command
    cout << "\nDatabase loaded, command mode initialised\n";
    while (1) {
        cout << "> ";
        cin >> commandBuffer;
        // Parse the command the user entered, using strncmp for string comparison
        if (strncmp(commandBuffer, "HELP", 4) == 0) {
            printHelpFile();
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
        } else if (strncmp(commandBuffer, "DELCOL", 6) == 0) {
            // Delete column
            delColumn();
        } else if (strncmp(commandBuffer, "DELROW", 6) == 0) {
            // Delete row
            delRow();
        } else if (strncmp(commandBuffer, "MODIFY", 6) == 0) {
            // Modify at row/column
            modify();
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

/// Prints the contents of the database in RAM to screen
void printDBInRAM() {
    cout << "\nRow count: " << rowCount << endl;
    cout << "Column count: " << columnCount << endl << endl;

    if (rowCount > 1) {
        printSeparator(columnCount);
    }
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
    if (rowCount > 1) {
        printSeparator(columnCount);
    }
    cout << endl;
}

/// Adds column to database
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

/// Adds row to database
void addRow() {
    if (columnCount > 0) { // if there are columns
        // proceed with adding a row
        char identifier[20];
        cout << "Enter row identifier: ";
        cin >> identifier;
        rowCount++;
        strcpy(database[rowCount-1][0], identifier);
        cout << "Row added with identifier " << identifier << endl;
    } else {
        cout << "No columns to add rows to!";
    }
}

/// Deletes column from database
void delColumn() {
    if (columnCount > 0) {
        char choice;
        cout << "Are you sure you want to delete the last column? (y/n): ";
        cin >> choice;
        if (choice == 'y') {
            columnCount--;
            cout << "Column deleted" << endl;
        } else if (choice == 'n') {
            cout << "Database unmodified" << endl;
        } else {
            cout << "Invalid input. Not modifying database." << endl;
        }
    } else {
        cout << "No more columns to delete!" << endl;
    }
}

/// Deletes row from database
void delRow() {
    if (rowCount > 0) {
        char choice;
        cout << "Are you sure you want to delete the last row? (y/n): ";
        cin >> choice;
        if (choice == 'y') {
            rowCount--;
            cout << "Row deleted" << endl;
        } else if (choice == 'n') {
            cout << "Database unmodified" << endl;
        } else {
            cout << "Invalid input. Not modifying database." << endl;
        }
    } else {
        cout << "No more rows to delete!" << endl;
    }
}

/// Modifies a specific row/column combination in database
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

    char dataBuffer[20];
    cout << "Enter data for column " << column << ", row " << row << " : ";
    cin >> dataBuffer;
    strcpy(database[row-1][column-1], dataBuffer);
    cout << "Entry added" << endl;
}

/// Write changes to database to file
void commitChanges() {
    // Construct a single string that will write to the file, this process is essentially serialisation
    char flatStringForFile[1000] = {0x0};
    int currentFlatStrIndex = 0;
    // For loop for rows
    for (int i = 0; i < rowCount; i++) {
        // For loop for columns
        for (int j = 0; j < columnCount; j++) {
            // For loop for characters
            for (int c = 0; c < 20; c++) { //max 20 characters, have to hardcode due to limitations of C-style character arrays
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

/// Prints help file
void printHelpFile() {
    cout << "------------------------------------\n";
    cout << " FIDB Operator's Instruction Manual\n";
    cout << " For version v0.1a\n";
    cout << "------------------------------------\n\n";
    cout << "All available help topics:\n1. Introduction\n2. Commands\n3. File formats\n";
    cout << "Select help topic to view: ";
    int selection;
    cin >> selection;
    cout << endl;
    if (selection == 1) {
        // Introduction
        cout << "Introduction to FIDB:\n";
        cout << "FIDB is a simple database management system for flatfile database systems.\n";
        cout << "Written in C++, and uses only basic C++ headers, such as iostream, iomanip, fstream and cstring\n";
        cout << "The manager uses a command-line like system to interpret user input, and uses space delimited csv files.\n";
        cout << "However, this database only handles up to 20 rows by 20 columns by 20 characters, as a limitation of not\n being able to manually allocate memory due to time and library constraints" << endl;
    } else if (selection == 2) {
        cout << "Commands in FIDB:\n";
        cout << "DUMPDB:\nDumps the contents of the database in memory onto the screen.\n";
        cout << "MODIFY:\nModifies the database at a certain row/column combination.\n";
        cout << "COMMIT:\nSaves the database in memory to file.\n";
        cout << "ADDCOL/ADDROW:\nAppends a new row or column to the right/bottom of a table.\n";
        cout << "DELCOL/DELROW:\nDeletes the rightmost or bottomost row or column.\n";
        cout << "HELP:\nBrings up this help file.\n";
        cout << "EXIT/QUIT:\nExits the database manager. NOTE: This will NOT save the database to memory!" << endl;
    } else if (selection == 3) {
        cout << "File formats for FIDB:\n";
        cout << "FIDB uses a simple flatfile format called CSV. Originally known as \"Comma Separated Values\", the \nchoice of a delimiter (In the previous case, commas), are not limited to commas as CSV is an open arbitrary standard. Therefore, \nin FIDB, spaces are used instead to guarentee interoperability of the program. Newlines are used as row delimiters while spaces \nare used as column delimiters.\nFIDB accepts any ASCII encoded, space delimited CSV style document." << endl;
    } else {
        cout << "Invalid selection, exiting program" << endl;
    }
}
