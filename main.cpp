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
    char fileName[20];
    cout << "\nPlease enter file name of database file to manage (maximum 30 characters): ";
    cin >> fileName;
    fstream dbFile(fileName); // initialise flatfile database

    // Check if db file exists and if not prompt to create new file
    if (!fileExists(dbFile)) {
        char choice;
        cout << "File does not exist! Create empty database? (y/n): ";
        while (choice != 'y' || choice != 'n') {
            cin >> choice;
            if (choice=='y') {
                break;
            } else if (choice == 'n') {
                cout << "Quitting database manager..." << endl;
                return; // quit the entire program directly
            } else {
                cout << "Invalid choice. Please re-enter choice: ";
            }
        }
    }

    // Start the manager proper

    // Check if file is empty
    if (fileEmpty(dbFile)) {
        cout << "Database file is empty" << endl;
    } else {
        // Parse and insert database into RAM
        // Getting by rows
        string line;
        while (getline(dbFile, line)) {
            int localColumnCount = 0; // reset column count every time when the program gets to a new row
            // Turn the string into a C string (char array) for compliance with the cstring library
            char lineWithCString[50];
            strcpy(lineWithCString, line.c_str());
            char * pch; //pointer to the position of characters split by a delimiter
            pch = strtok(lineWithCString, " "); //space as a delimiter, and yes, we need " " because it accepts only a const char *, NOT const char (in other words, char array, not char)
            while (pch != NULL) {
                strcpy(database[rowCount][localColumnCount], pch);
                localColumnCount++;
                if (rowCount == 0) { // if it's first row
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
        if (i==0) {
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
    strcpy(database[0][columnCount-1], identifier);
    cout << "Column added with identifier " << identifier << endl;
}

void commitChanges() {
    // Construct a single string that will write to the file
    char flatStringForFile[500] = {0x0};

}

// Helper functions

/*
    The input sanitiser "sanitises" all character arrays by making sure that the last character is null terminated

    - parameter string: the string to "sanitise"
    - returns: a string as a character array
*/
char* sanitisedString(char* string) {
    string[strlen(string)-1] = '\0';
    return string;
}

int fileExists(fstream& pFile) {
    return pFile.good();
}

int fileEmpty(fstream& pFile)
{
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
