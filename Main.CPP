/*
 * ============================================================
 *  DIGITAL ATTENDANCE SYSTEM FOR HOUR-BASED LECTURES
 *  Programme : HND Electrical Engineering (Level 200)
 *  Course    : EEE227 - Introduction to C++ Programming
 *  Developer : Franklin Sampson
 *  Version   : 1.2 - Admin Login + Complete Student Management
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

const string ADMIN_FILE = "admin.txt";

// ============================================================
//  UTILITY HELPERS
// ============================================================

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void printDivider(char ch = '-', int width = 90) {
    cout << string(width, ch) << "\n";
}

string toUpper(string s) {
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}

// ============================================================
//  STUDENT CLASS
// ============================================================
class Student {
private:
    string indexNumber;
    string firstName;
    string lastName;
    string programme;

public:
    Student() {}

    Student(string idx, string fName, string lName, string prog)
        : indexNumber(idx), firstName(fName), lastName(lName), programme(prog) {}

    string getIndexNumber() const { return indexNumber; }
    string getFirstName()   const { return firstName; }
    string getLastName()    const { return lastName; }
    string getFullName()    const { return firstName + " " + lastName; }
    string getProgramme()   const { return programme; }

    void setIndexNumber(string idx) { indexNumber = idx; }
    void setFirstName(string fName) { firstName   = fName; }
    void setLastName(string lName)  { lastName    = lName; }
    void setProgramme(string prog)  { programme   = prog; }

    void display() const {
        cout << left
             << setw(15) << indexNumber
             << setw(20) << firstName
             << setw(20) << lastName
             << setw(30) << programme
             << "\n";
    }
};

// Global student list
vector<Student> students;

// ============================================================
//  ADMIN CREDENTIALS
// ============================================================

bool adminAccountExists() {
    ifstream f(ADMIN_FILE);
    return f.good();
}

bool loadAdminCredentials(string& username, string& password) {
    ifstream f(ADMIN_FILE);
    if (!f.is_open()) return false;
    string line;
    if (!getline(f, line)) return false;
    line = trim(line);
    size_t p = line.find('|');
    if (p == string::npos) return false;
    username = trim(line.substr(0, p));
    password = trim(line.substr(p + 1));
    return true;
}

void saveAdminCredentials(const string& username, const string& password) {
    ofstream f(ADMIN_FILE);
    if (!f.is_open()) { cout << "  [ERROR] Could not save credentials.\n"; return; }
    f << username << "|" << password << "\n";
    f.close();
}

void createAdminAccount() {
    cout << "\n"; printDivider('=');
    cout << "  FIRST-TIME SETUP: CREATE ADMIN ACCOUNT\n"; printDivider('=');
    cout << "  No admin account found. Please create one.\n\n";

    string username, password, confirm;

    while (true) {
        cout << "  Enter admin username : ";
        getline(cin, username); username = trim(username);
        if (!username.empty()) break;
        cout << "  [ERROR] Username cannot be empty.\n";
    }

    while (true) {
        cout << "  Enter admin password : ";
        getline(cin, password); password = trim(password);
        if (password.empty()) { cout << "  [ERROR] Password cannot be empty.\n"; continue; }
        cout << "  Confirm password     : ";
        getline(cin, confirm);
        if (password == confirm) break;
        cout << "  [ERROR] Passwords do not match. Try again.\n";
    }

    saveAdminCredentials(username, password);
    cout << "\n  [SUCCESS] Admin account created! Username: " << username << "\n";
    printDivider('=');
}

bool adminLogin() {
    string storedUser, storedPass;
    if (!loadAdminCredentials(storedUser, storedPass)) {
        cout << "  [ERROR] Could not read admin credentials.\n";
        return false;
    }

    cout << "\n"; printDivider('=');
    cout << "  ADMIN LOGIN\n"; printDivider('=');

    int attempts = 0;
    while (attempts < 3) {
        string username, password;
        cout << "  Username : "; getline(cin, username);
        cout << "  Password : "; getline(cin, password);

        if (trim(username) == storedUser && trim(password) == storedPass) {
            cout << "\n  [SUCCESS] Welcome, " << storedUser << "!\n";
            printDivider('=');
            return true;
        }
        attempts++;
        int remaining = 3 - attempts;
        if (remaining > 0)
            cout << "  [ERROR] Incorrect credentials. " << remaining << " attempt(s) left.\n\n";
    }
    cout << "  [LOCKED] Too many failed attempts. Exiting.\n";
    return false;
}

// ============================================================
//  STUDENT FUNCTIONS
// ============================================================

bool indexExists(const string& idx) {
    for (const auto& s : students)
        if (toUpper(s.getIndexNumber()) == toUpper(idx)) return true;
    return false;
}

void printStudentTableHeader() {
    printDivider('=');
    cout << left
         << setw(5)  << "No."
         << setw(15) << "Index No."
         << setw(20) << "First Name"
         << setw(20) << "Last Name"
         << setw(30) << "Programme"
         << "\n";
    printDivider('=');
}

void registerStudent() {
    cout << "\n"; printDivider('=');
    cout << "  REGISTER NEW STUDENT\n";
    cout << "  (Enter 0 at any prompt to cancel)\n";
    printDivider('=');

    string idx, fName, lName, prog;

    cout << "  Enter Index Number : "; getline(cin, idx); idx = trim(idx);
    if (idx == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }
    if (idx.empty())      { cout << "  [ERROR] Index number cannot be empty.\n"; return; }
    if (indexExists(idx)) { cout << "  [ERROR] Index number already exists.\n";  return; }

    cout << "  Enter First Name   : "; getline(cin, fName); fName = trim(fName);
    if (fName == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }
    if (fName.empty()) { cout << "  [ERROR] First name cannot be empty.\n"; return; }

    cout << "  Enter Last Name    : "; getline(cin, lName); lName = trim(lName);
    if (lName == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }
    if (lName.empty()) { cout << "  [ERROR] Last name cannot be empty.\n"; return; }

    cout << "  Enter Programme    : "; getline(cin, prog); prog = trim(prog);
    if (prog == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }
    if (prog.empty()) { cout << "  [ERROR] Programme cannot be empty.\n"; return; }

    students.push_back(Student(idx, fName, lName, prog));
    cout << "\n  [SUCCESS] '" << fName << " " << lName << "' registered!\n";
}

void viewAllStudents() {
    cout << "\n"; printDivider('=');
    cout << "  ALL REGISTERED STUDENTS  (Total: " << students.size() << ")\n";
    if (students.empty()) {
        printDivider('=');
        cout << "  No students registered yet.\n";
        printDivider('=');
        return;
    }
    printStudentTableHeader();
    for (size_t i = 0; i < students.size(); i++) {
        cout << left << setw(5) << (to_string(i + 1) + ".");
        students[i].display();
    }
    printDivider('=');
}

void searchStudentByIndex() {
    cout << "\n"; printDivider('=');
    cout << "  SEARCH STUDENT\n";
    cout << "  (Enter 0 to cancel)\n"; printDivider('=');
    if (students.empty()) { cout << "  No students registered yet.\n"; return; }

    string idx;
    cout << "  Enter Index Number: "; getline(cin, idx); idx = trim(idx);
    if (idx == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }

    for (const auto& s : students) {
        if (toUpper(s.getIndexNumber()) == toUpper(idx)) {
            cout << "\n  STUDENT FOUND:\n";
            printStudentTableHeader();
            cout << left << setw(5) << " "; s.display();
            printDivider('=');
            return;
        }
    }
    cout << "  [NOT FOUND] No student with index '" << idx << "'.\n";
}

void studentManagementMenu() {
    int choice;
    do {
        cout << "\n"; printDivider('*');
        cout << "  STUDENT MANAGEMENT\n"; printDivider('*');
        cout << "  1. Register New Student\n";
        cout << "  2. View All Students\n";
        cout << "  3. Search Student by Index Number\n";
        cout << "  0. Back to Main Menu\n";
        printDivider('*');
        cout << "  Enter choice: ";
        if (!(cin >> choice)) { clearInput(); cout << "  [ERROR] Enter a number.\n"; continue; }
        clearInput();
        switch (choice) {
            case 1: registerStudent();      break;
            case 2: viewAllStudents();      break;
            case 3: searchStudentByIndex(); break;
            case 0: break;
            default: cout << "  [ERROR] Invalid choice.\n";
        }
    } while (choice != 0);
}

void mainMenu() {
    int choice;
    do {
        cout << "\n"; printDivider('#');
        cout << "  DIGITAL ATTENDANCE SYSTEM - EEE227\n";
        cout << "  HND Electrical Engineering, Level 200\n";
        printDivider('#');
        cout << "  1. Student Management\n";
        cout << "  0. Exit\n";
        printDivider('#');
        cout << "  Enter choice: ";
        if (!(cin >> choice)) { clearInput(); continue; }
        clearInput();
        switch (choice) {
            case 1: studentManagementMenu(); break;
            case 0: cout << "\n  Goodbye!\n\n"; break;
            default: cout << "  [ERROR] Invalid choice.\n";
        }
    } while (choice != 0);
}

int main() {
    cout << "\n"; printDivider('=');
    cout << "  DIGITAL ATTENDANCE SYSTEM - EEE227\n";
    cout << "  HND Electrical Engineering, Level 200\n";
    printDivider('=');

    if (!adminAccountExists()) createAdminAccount();
    if (!adminLogin()) return 1;

    mainMenu();
    return 0;
}