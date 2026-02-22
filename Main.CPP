/*
 * ============================================================
 *  DIGITAL ATTENDANCE SYSTEM FOR HOUR-BASED LECTURES
 *  Programme : HND Electrical Engineering (Level 200)
 *  Course    : EEE227 - Introduction to C++ Programming
 *  Developer : Franklin Sampson
 *  Version   : 1.0 - Project Setup, Student Class
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

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

// Add a student (basic - no validation yet)
void registerStudent() {
    string idx, fName, lName, prog;
    cout << "\n  Enter Index Number : "; cin >> idx;
    cout << "  Enter First Name   : "; cin >> fName;
    cout << "  Enter Last Name    : "; cin >> lName;
    cout << "  Enter Programme    : "; cin >> prog;
    students.push_back(Student(idx, fName, lName, prog));
    cout << "\n  [SUCCESS] Student registered!\n";
}

// View all registered students
void viewAllStudents() {
    if (students.empty()) {
        cout << "\n  No students registered yet.\n";
        return;
    }
    cout << "\n" << string(85, '=') << "\n";
    cout << left << setw(15) << "Index No."
         << setw(20) << "First Name"
         << setw(20) << "Last Name"
         << setw(30) << "Programme" << "\n";
    cout << string(85, '-') << "\n";
    for (const auto& s : students)
        s.display();
    cout << string(85, '=') << "\n";
}

int main() {
    cout << "\n" << string(50, '=') << "\n";
    cout << "  DIGITAL ATTENDANCE SYSTEM - EEE227\n";
    cout << "  HND Electrical Engineering, Level 200\n";
    cout << string(50, '=') << "\n";

    int choice;
    do {
        cout << "\n  1. Register Student\n";
        cout << "  2. View All Students\n";
        cout << "  0. Exit\n";
        cout << "  Choice: ";
        cin >> choice;
        switch (choice) {
            case 1: registerStudent(); break;
            case 2: viewAllStudents(); break;
            case 0: cout << "\n  Goodbye!\n\n"; break;
            default: cout << "  Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}