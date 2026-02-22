/*
 * ============================================================
 *  DIGITAL ATTENDANCE SYSTEM FOR HOUR-BASED LECTURES
 *  Programme : HND Electrical Engineering (Level 200)
 *  Course    : EEE227 - Introduction to C++ Programming
 *  Developer : Franklin Sampson
 *  Version   : 1.1 - Input Validation + Search Student
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

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

    // Getters
    string getIndexNumber() const { return indexNumber; }
    string getFirstName()   const { return firstName; }
    string getLastName()    const { return lastName; }
    string getFullName()    const { return firstName + " " + lastName; }
    string getProgramme()   const { return programme; }

    // Setters
    void setIndexNumber(string idx) { indexNumber = idx; }
    void setFirstName(string fName) { firstName   = fName; }
    void setLastName(string lName)  { lastName    = lName; }
    void setProgramme(string prog)  { programme   = prog; }

    // Display as a formatted table row
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

// Check if index number already exists
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

// Register student with full input validation
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

// View all students in a formatted table
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

// Search for a student by index number
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

int main() {
    cout << "\n"; printDivider('=');
    cout << "  DIGITAL ATTENDANCE SYSTEM - EEE227\n";
    cout << "  HND Electrical Engineering, Level 200\n";
    printDivider('=');

    int choice;
    do {
        cout << "\n"; printDivider('#');
        cout << "  MAIN MENU\n"; printDivider('#');
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

    return 0;
}