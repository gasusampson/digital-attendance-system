/*
 * ============================================================
 *  DIGITAL ATTENDANCE SYSTEM FOR HOUR-BASED LECTURES
 *  Programme : HND Electrical Engineering (Level 200)
 *  Course    : EEE227 - Introduction to C++ Programming
 *  Developer : Franklin Sampson
 *  Version   : 2.1 - AttendanceSession class added
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <sstream>
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

// ============================================================
//  ATTENDANCE RECORD STRUCT
// ============================================================
struct AttendanceRecord {
    string indexNumber;
    string studentName;
    string status;   // "Present", "Absent", or "Late"

    AttendanceRecord() : status("Absent") {}
    AttendanceRecord(string idx, string name)
        : indexNumber(idx), studentName(name), status("Absent") {}
};

// ============================================================
//  ATTENDANCE SESSION CLASS
// ============================================================
class AttendanceSession {
private:
    string sessionID;
    string courseCode;
    string date;
    string startTime;
    int    durationHours;
    vector<AttendanceRecord> records;

public:
    AttendanceSession() : durationHours(0) {}

    AttendanceSession(string code, string dt, string start, int dur)
        : courseCode(code), date(dt), startTime(start), durationHours(dur) {
        string d = dt;
        replace(d.begin(), d.end(), '-', '_');
        sessionID = code + "_" + d;
    }

    // Getters
    string getSessionID()  const { return sessionID; }
    string getCourseCode() const { return courseCode; }
    string getDate()       const { return date; }
    string getStartTime()  const { return startTime; }
    int    getDuration()   const { return durationHours; }

    vector<AttendanceRecord>&       getRecords()       { return records; }
    const vector<AttendanceRecord>& getRecords() const { return records; }

    // Setters
    void setCourseCode(string code) { courseCode    = code; }
    void setDate(string dt)         { date          = dt; }
    void setStartTime(string st)    { startTime     = st; }
    void setDuration(int dur)       { durationHours = dur; }

    void rebuildSessionID() {
        string d = date;
        replace(d.begin(), d.end(), '-', '_');
        sessionID = courseCode + "_" + d;
    }

    // Populate records from student list (all default to Absent)
    void initialiseRecords(const vector<Student>& students) {
        records.clear();
        for (const auto& s : students)
            records.push_back(AttendanceRecord(s.getIndexNumber(), s.getFullName()));
    }

    void displayHeader() const {
        printDivider('=');
        cout << "  Session ID  : " << sessionID      << "\n";
        cout << "  Course Code : " << courseCode     << "\n";
        cout << "  Date        : " << date           << "\n";
        cout << "  Start Time  : " << startTime      << "\n";
        cout << "  Duration    : " << durationHours  << " hour(s)\n";
        cout << "  Students    : " << records.size() << "\n";
        printDivider('=');
    }
};

// ============================================================
//  GLOBAL STORES
// ============================================================
vector<Student>           students;
vector<AttendanceSession> sessions;

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

// ============================================================
//  SESSION HELPERS
// ============================================================

bool isValidDate(const string& d) {
    if (d.size() != 10) return false;
    if (d[4] != '-' || d[7] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(d[i])) return false;
    }
    return true;
}

bool isValidTime(const string& t) {
    if (t.size() != 5 || t[2] != ':') return false;
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!isdigit(t[i])) return false;
    }
    int hh = stoi(t.substr(0, 2));
    int mm = stoi(t.substr(3, 2));
    return (hh >= 0 && hh <= 23 && mm >= 0 && mm <= 59);
}

bool sessionExists(const string& id) {
    for (const auto& sess : sessions)
        if (toUpper(sess.getSessionID()) == toUpper(id)) return true;
    return false;
}

// ============================================================
//  SESSION FUNCTIONS
// ============================================================

void createSession() {
    cout << "\n"; printDivider('=');
    cout << "  CREATE NEW LECTURE SESSION\n";
    cout << "  (Enter 0 at any prompt to cancel)\n"; printDivider('=');

    if (students.empty()) {
        cout << "  [WARNING] No students registered yet. Register students first.\n";
        return;
    }

    string code, date, startTime;
    int duration = 0;

    cout << "  Enter Course Code (e.g. EEE227) or 0 to cancel: ";
    getline(cin, code); code = trim(code);
    if (code == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }
    if (code.empty()) { cout << "  [ERROR] Course code cannot be empty.\n"; return; }

    while (true) {
        cout << "  Enter Date (YYYY-MM-DD) or 0 to cancel: ";
        getline(cin, date); date = trim(date);
        if (date == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }
        if (isValidDate(date)) break;
        cout << "  [ERROR] Use YYYY-MM-DD e.g. 2026-02-23\n";
    }

    while (true) {
        cout << "  Enter Start Time (HH:MM, 24hr) or 0 to cancel: ";
        getline(cin, startTime); startTime = trim(startTime);
        if (startTime == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }
        if (isValidTime(startTime)) break;
        cout << "  [ERROR] Use HH:MM e.g. 08:00\n";
    }

    while (duration < 1 || duration > 8) {
        cout << "  Enter Duration in Hours (1-8) or 0 to cancel: ";
        string d; getline(cin, d); d = trim(d);
        if (d == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }
        try {
            duration = stoi(d);
            if (duration < 1 || duration > 8)
                cout << "  [ERROR] Duration must be 1-8 hours.\n";
        } catch (...) {
            cout << "  [ERROR] Enter a valid number.\n";
            duration = 0;
        }
    }

    AttendanceSession newSession(code, date, startTime, duration);
    if (sessionExists(newSession.getSessionID())) {
        cout << "  [ERROR] Session for " << code << " on " << date << " already exists.\n";
        return;
    }

    newSession.initialiseRecords(students);
    sessions.push_back(newSession);
    cout << "\n  [SUCCESS] Session created!\n";
    sessions.back().displayHeader();
}

void viewAllSessions() {
    cout << "\n"; printDivider('=');
    cout << "  ALL LECTURE SESSIONS  (Total: " << sessions.size() << ")\n"; printDivider('=');

    if (sessions.empty()) {
        cout << "  No sessions created yet.\n"; printDivider('='); return;
    }

    cout << left
         << setw(5)  << "No."
         << setw(28) << "Session ID"
         << setw(12) << "Course"
         << setw(14) << "Date"
         << setw(10) << "Start"
         << setw(12) << "Duration"
         << setw(10) << "Students"
         << "\n";
    printDivider('-');

    for (size_t i = 0; i < sessions.size(); i++) {
        const auto& s = sessions[i];
        cout << left
             << setw(5)  << (to_string(i + 1) + ".")
             << setw(28) << s.getSessionID()
             << setw(12) << s.getCourseCode()
             << setw(14) << s.getDate()
             << setw(10) << s.getStartTime()
             << setw(12) << (to_string(s.getDuration()) + " hr(s)")
             << setw(10) << s.getRecords().size()
             << "\n";
    }
    printDivider('=');
}

// ============================================================
//  MENUS
// ============================================================

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

void sessionManagementMenu() {
    int choice;
    do {
        cout << "\n"; printDivider('*');
        cout << "  SESSION MANAGEMENT\n"; printDivider('*');
        cout << "  1. Create New Lecture Session\n";
        cout << "  2. View All Sessions\n";
        cout << "  0. Back to Main Menu\n";
        printDivider('*');
        cout << "  Enter choice: ";
        if (!(cin >> choice)) { clearInput(); cout << "  [ERROR] Enter a number.\n"; continue; }
        clearInput();
        switch (choice) {
            case 1: createSession();   break;
            case 2: viewAllSessions(); break;
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
        cout << "  2. Session Management\n";
        cout << "  0. Exit\n";
        printDivider('#');
        cout << "  Enter choice: ";
        if (!(cin >> choice)) { clearInput(); cout << "  [ERROR] Enter a number.\n"; continue; }
        clearInput();
        switch (choice) {
            case 1: studentManagementMenu(); break;
            case 2: sessionManagementMenu(); break;
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