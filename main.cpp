#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include <ctime>

#include <cstdio>

#include <dirent.h>
#include <conio.h>
using namespace std;


// Security----------------------------------------------------------------

struct User {
    char username[50];
    char password[50];
    char recoveryQuestion[100]; // For storing the recovery answer (e.g., birthday)
};

// Function to check if the directory exists, and create it if not
bool ensureDirectoryExists(const string &path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        // Directory doesn't exist, create it
        if (mkdir(path.c_str()) == 0) {
            //cout << "Directory created at: " << path << endl;
            cout << "Setup Sucessfull";
            return true;
        } else {
            cerr << "Failed to create directory.\n";
            return false;
        }
    } else if (info.st_mode & S_IFDIR) {
        // Directory exists
        return true;
    } else {
        cerr << path << " is not a directory.\n";
        return false;
    }
}

// Function to check if the binary file exists
bool doesFileExist(const string &filename) {
    ifstream file(filename, ios::binary);
    return file.good();
}

// Function for sign-up (create username, password, and recovery question)
void signUp(const string &filename) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error creating the file.\n";
        return;
    }

    User user;
    cout << "Sign-Up (First-Time User)\n";
    cout << "Enter a username: ";
    cin >> user.username;
    cout << "Enter a password: ";
    cin >> user.password;
    cin.ignore(); // Clear input buffer
    cout << "Enter a recovery answer (e.g., Your Birthday - DD/MM/YYYY): ";
    cin.getline(user.recoveryQuestion, 100);

    file.write((char*)&user, sizeof(User));
    file.close();
    cout << "Sign-up successful! Please restart to log in.\n";
}

// Function for password recovery
void recoverPassword(const string &filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening the file.\n";
        return;
    }

    User userFromFile;
    file.read((char*)&userFromFile, sizeof(User));
    file.close();

    string recoveryAnswer;
    cout << "Password Recovery\n";
    cout << "Answer the recovery question: What is your birthday (DD/MM/YYYY)? ";
    cin.ignore(); // Clear input buffer
    getline(cin, recoveryAnswer);

    // Validate the recovery answer
    if (recoveryAnswer == string(userFromFile.recoveryQuestion)) {
        cout << "Recovery successful! Your password is: " << userFromFile.password << "\n";
    } else {
        cout << "Incorrect recovery answer. Password recovery failed.\n";
    }
}

// Function for login (verify username and password)
bool login(const string &filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening the file.\n";
        return false;
    }

    User userFromFile, userInput;
    file.read((char*)&userFromFile, sizeof(User));
    file.close();

    cout << "Login (Returning User)\n";
    cout << "Enter your username: ";
    cin >> userInput.username;
    cout << "Enter your password: ";
    cin >> userInput.password;

    // Validate credentials
    if (string(userInput.username) == string(userFromFile.username) &&
        string(userInput.password) == string(userFromFile.password)) {
        cout << "Login successful! Welcome back, " << userInput.username << ".\n";
        return true;
    } else {
        cout << "Invalid username or password.\n";
        return false;
    }
}
//Add Entry Files---------------------------------------------------------------------

// Utility function to get the current date in DD/MM/YYYY format
string getCurrentDate() {
    time_t now = time(nullptr);
    tm *localTime = localtime(&now);

    char date[11];
    strftime(date, sizeof(date), "%d/%m/%Y", localTime);
    return string(date);
}

// Function to check if a file exists
bool fileExists(const string& fileName) {
    ifstream file(fileName);
    return file.good(); // Returns true if the file can be opened
}

// Add Entry function
void addEntry() {
    string entryName;

    while (true) {
        cout << "\n\nEnter the name of the entry (To cancel entry, type 'CANCEL'): ";
        cin.ignore(); // Clear input buffer
        getline(cin, entryName);
        //cout<<entryName;
        if(entryName != "CANCEL"){
		
		const string folderPath = "C:/Users/Public/SilentPages";
        string fileName = folderPath+"/"+entryName + ".bin";

        // Check if the file already exists
        if (fileExists(fileName)) {
            cout << "\nAn entry with the name \"" << entryName << "\" already exists. Please choose a different name.\n";
        } else {
            // No conflict; proceed with entry creation
            ofstream file(fileName, ios::binary);
            if (!file) {
                cerr << "Error creating the file.\n";
                return;
            }
			cout << "-------------------------------------------------------------------------";
            cout << "\nWrite your entry (type 'SAVE' on a new line to save and exit):\n\n\n";

            string line;
            while (true) {
                getline(cin, line);

                // Check for "CTRL+S" keyword to stop
                if (line == "SAVE") {
                    break;
                }

                // Write line to the binary file
                file.write(line.c_str(), line.size());
                file.write("\n", 1); // Add a newline character
            }

            // Append the current date to the file
            string date = getCurrentDate();
            string dateFooter = "\n--- Entry Date: " + date + " ---\n";
            file.write(dateFooter.c_str(), dateFooter.size());

            file.close();
            cout << "Entry saved successfully: " << entryName << "\n";
            break; // Exit the loop after successful addition
        }
    }
    else{
    	cout << "\n Entry Cancelled.\n";
    	break;
	}
}
}

// Delete Entry Files --------------------------------------------------------------------------------

// Delete Entry function
void deleteEntry() {
    string entryName;
    cout << "\n\nEnter the name of the entry to delete: ";
    cin.ignore(); // Clear input buffer
    getline(cin, entryName);
	const string folderPath = "C:/Users/Public/SilentPages";
    string fileName = folderPath+"/"+entryName + ".bin";

    // Check if the file exists
    ifstream file(fileName);
    if (!file) {
        cout << "No entry found with the name: " << entryName << "\n";
        return;
    }
    file.close();

    // Ask for confirmation
    char confirm;
    cout << "Are you sure you want to delete the entry \"" << entryName << "\"? (y/n): ";
    cin >> confirm;

    if (tolower(confirm) == 'y') {
        // Delete the file
        if (remove(fileName.c_str()) == 0) {
            cout << "Entry \"" << entryName << "\" deleted successfully.\n";
        } else {
            cerr << "Error deleting the file.\n";
        }
    } else {
        cout << "Deletion canceled.\n";
    }
}

// Open Entry Files -----------------------------------------------------------------------------------------------
// View and Edit Entry function
void OpenEntry() {
    string entryName;
    cout << "\n\nEnter the name of the entry to view/edit: ";
    cin.ignore(); // Clear input buffer
    getline(cin, entryName);
	const string folderPath = "C:/Users/Public/SilentPages";
    string fileName = folderPath+"/"+entryName + ".bin";

    // Check if the file exists
    fstream file(fileName, ios::in | ios::out | ios::binary | ios::app); // Open in read/write mode
    if (!file) {
        cout << "No entry found with the name: " << entryName << "\n";
        return;
    }
	cout <<"---------------------------------------------------------------------------------------------";
    cout << "\n-------------------------------------- MY DAIRY --------------------------------------------\n";

    // Read and display the content of the file
    file.seekg(0, ios::beg); // Go to the beginning for reading
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }

    cout << "------------------------------------- End of Entry ---------------------------------------------\n";

    // Allow the user to edit the entry
    cout << "\nYou can now add new content to the entry. Type 'SAVE' on a new line to save and exit.\n";

    string newLine;
        // Move the file pointer to the end before appending new content
    file.clear(); // Clear any EOF or error flags
    file.seekp(0, ios::end); // Move the pointer to the end for appending
    while (true) {
        getline(cin, newLine);

        if (newLine == "SAVE") {
            break;
        }

        // Append new content to the file
        file << newLine << endl;
    }
	// Append the current date to the file
            string date = getCurrentDate();
            string dateFooter = "\n--- Entry Date: " + date + " ---\n";
            file.write(dateFooter.c_str(), dateFooter.size());
    file.close();
    cout << "Changes saved successfully\n";
     
}

// View All Entries --------------------------------------------------------------------
// Function to list all .bin files in the current directory
void ViewAll() {
    DIR *dir;
    struct dirent *entry;
	const string folderPath = "C:/Users/Public/SilentPages";
    // Open the current directory
    dir = opendir(folderPath.c_str());
    if (!dir) {
        cerr << "Could not open directory.\n";
        return;
    }

    cout << "\n--- List of All Entries ---\n";
    bool found = false;

    // Loop through the files in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Check if the file ends with ".bin"
        string filename = entry->d_name;
        if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".bin") {
            found = true;
            cout << filename.substr(0, filename.size() - 4) << endl; // Display file name without the extension
        }
    }
    cout << "\n--------------------------------------------------\n";

    if (!found) {
        cout << "No entries found.\n";
    }

    closedir(dir);
}


// Main function to handle sign-up, login, or password recovery ----------------------------------------------------
int main() {
    const string folderPath = "C:/Users/Public/SilentPages"; // Path to store user data
    const string filename = folderPath + "/user_data.dat"; // Binary file for storing user credentials
    int choice;
    int entrytype;

    // Ensure the directory exists
    if (!ensureDirectoryExists(folderPath)) {
        return 1; // Exit if directory creation fails
    }

    if (!doesFileExist(filename)) {
        cout << "No user found. Starting sign-up process...\n";
        signUp(filename);
    } else {
	
        cout << "\n--- Diary Security System ---\n";
        cout << "1. Login\n";
        cout << "2. Recover Password\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                if (login(filename)) {
                    cout << "Access granted to the diary.\n";
                    
                    while(true){
                    	cout << "\n--------------------------------------------------\n";
                    	cout << "\n1. Add Entry.\n";
                    	cout << "2. Open Entry. \n";
                    	cout << "3. Delete Entry. \n";
                    	cout << "4. View All Entries. \n";
                    	cout << "5. Exit\n";
                    	cout << "\n--------------------------------------------------\n";
                    	cout << "Enter Your choice : ";
                    	cin >> entrytype;
                    	switch(entrytype){
                    		case 1:
                    			addEntry();
                    			break;
                    		case 2:
                    			OpenEntry();
                    			break;
                    		case 3:
                    			deleteEntry();
                    			break;
                    		case 4:
                    			ViewAll();
                    			break;
                    		case 5:
                    			cout << "Exiting Application.\n";
                    			exit(0);
                    			break;
                    		default:
                    			cout << "Invalid selection.\n";
                    			
                    			
						}
					}
                } else {
                    cout << "Access denied.\n";
                }
                break;
            case 2:
                recoverPassword(filename);
                break;
            case 3:
                cout << "Exiting the Application. Goodbye!\n";
                exit(0);
                break;
            default:
                cout << "Invalid choice.\n";
                break;
        }
    }


    return 0;
}
