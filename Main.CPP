/*
 * ============================================================
 *  DIGITAL ATTENDANCE SYSTEM FOR HOUR-BASED LECTURES
 *  Programme : HND Electrical Engineering (Level 200)
 *  Course    : EEE227 - Introduction to C++ Programming
 *  Developer : Franklin Sampson
 *  Version   : 3.1 - File saving and loading implemented
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

const string ADMIN_FILE    = "admin.txt";
const string STUDENTS_FILE = "students.txt";
const string SESSIONS_FILE = "sessions.txt";

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

    // Serialise to pipe-delimited string for file storage
    string serialise() const {
        return indexNumber + "|" + firstName + "|" + lastName + "|" + programme;
    }

    // Deserialise from a pipe-delimited file line
    static Student deserialise(const string& line) {
        Student s;
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        if (p1 == string::npos || p2 == string::npos || p3 == string::npos)
            return s;
        s.indexNumber = trim(line.substr(0, p1));
        s.firstName   = trim(line.substr(p1 + 1, p2 - p1 - 1));
        s.lastName    = trim(line.substr(p2 + 1, p3 - p2 - 1));
        s.programme   = trim(line.substr(p3 + 1));
        return s;
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

    string getSessionID()  const { return sessionID; }
    string getCourseCode() const { return courseCode; }
    string getDate()       const { return date; }
    string getStartTime()  const { return startTime; }
    int    getDuration()   const { return durationHours; }

    vector<AttendanceRecord>&       getRecords()       { return records; }
    const vector<AttendanceRecord>& getRecords() const { return records; }

    void setCourseCode(string code) { courseCode    = code; }
    void setDate(string dt)         { date          = dt; }
    void setStartTime(string st)    { startTime     = st; }
    void setDuration(int dur)       { durationHours = dur; }

    void rebuildSessionID() {
        string d = date;
        replace(d.begin(), d.end(), '-', '_');
        sessionID = courseCode + "_" + d;
    }

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

    // Serialise session block for file storage
    string serialise() const {
        ostringstream oss;
        oss << "SESSION|" << courseCode << "|" << date
            << "|" << startTime << "|" << durationHours << "\n";
        for (const auto& r : records)
            oss << "RECORD|" << r.indexNumber << "|"
                << r.studentName << "|" << r.status << "\n";
        oss << "END_SESSION\n";
        return oss.str();
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
//  FILE HANDLING - STUDENTS
// ============================================================

void saveStudentsToFile() {
    ofstream file(STUDENTS_FILE);
    if (!file.is_open()) { cout << "\n  [ERROR] Cannot open " << STUDENTS_FILE << "\n"; return; }
    for (const auto& s : students) file << s.serialise() << "\n";
    file.close();
}

void loadStudentsFromFile() {
    ifstream file(STUDENTS_FILE);
    if (!file.is_open()) return;
    students.clear();
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (!line.empty()) {
            Student s = Student::deserialise(line);
            if (!s.getIndexNumber().empty()) students.push_back(s);
        }
    }
    file.close();
}

// ============================================================
//  FILE HANDLING - SESSIONS
// ============================================================

void saveAllSessionsToFile() {
    ofstream file(SESSIONS_FILE);
    if (!file.is_open()) { cout << "\n  [ERROR] Cannot open " << SESSIONS_FILE << "\n"; return; }
    for (const auto& sess : sessions) file << sess.serialise();
    file.close();
}

void loadAllSessionsFromFile() {
    sessions.clear();
    ifstream file(SESSIONS_FILE);
    if (!file.is_open()) return;

    int count = 0;
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line.size() >= 8 && line.substr(0, 8) == "SESSION|") {
            size_t p1 = line.find('|');
            size_t p2 = line.find('|', p1 + 1);
            size_t p3 = line.find('|', p2 + 1);
            size_t p4 = line.find('|', p3 + 1);
            if (p4 == string::npos) continue;

            AttendanceSession sess;
            sess.setCourseCode(trim(line.substr(p1 + 1, p2 - p1 - 1)));
            sess.setDate      (trim(line.substr(p2 + 1, p3 - p2 - 1)));
            sess.setStartTime (trim(line.substr(p3 + 1, p4 - p3 - 1)));
            sess.setDuration  (stoi(trim(line.substr(p4 + 1))));
            sess.rebuildSessionID();

            string recLine;
            while (getline(file, recLine)) {
                recLine = trim(recLine);
                if (recLine.empty()) continue;
                if (recLine == "END_SESSION") break;
                if (recLine.size() >= 7 && recLine.substr(0, 7) == "RECORD|") {
                    size_t rp1 = recLine.find('|');
                    size_t rp2 = recLine.find('|', rp1 + 1);
                    size_t rp3 = recLine.find('|', rp2 + 1);
                    if (rp3 == string::npos) continue;
                    AttendanceRecord r;
                    r.indexNumber = trim(recLine.substr(rp1 + 1, rp2 - rp1 - 1));
                    r.studentName = trim(recLine.substr(rp2 + 1, rp3 - rp2 - 1));
                    r.status      = trim(recLine.substr(rp3 + 1));
                    sess.getRecords().push_back(r);
                }
            }
            sessions.push_back(sess);
            count++;
        }
    }
    file.close();
    if (count > 0) cout << "  " << count << " session(s) loaded.\n";
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
    saveStudentsToFile();
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
    saveAllSessionsToFile();
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
//  ATTENDANCE MARKING
// ============================================================

void markAttendance() {
    cout << "\n"; printDivider('=');
    cout << "  MARK ATTENDANCE\n";
    cout << "  (Enter 0 when selecting session to cancel)\n"; printDivider('=');

    if (sessions.empty()) {
        cout << "  [WARNING] No sessions found. Create a session first.\n"; return;
    }

    viewAllSessions();
    cout << "  Enter session NUMBER (or 0 to cancel): ";
    string input; getline(cin, input); input = trim(input);
    if (input == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }

    int sessionChoice;
    try { sessionChoice = stoi(input); } catch (...) {
        cout << "  [ERROR] Invalid input.\n"; return;
    }
    if (sessionChoice < 1 || sessionChoice > (int)sessions.size()) {
        cout << "  [ERROR] Invalid session number.\n"; return;
    }

    AttendanceSession& session = sessions[sessionChoice - 1];
    cout << "\n  Marking attendance for:\n";
    session.displayHeader();
    cout << "  For each student:  1 = Present   2 = Absent   3 = Late\n\n";
    printDivider('-');

    vector<AttendanceRecord>& records = session.getRecords();

    for (size_t i = 0; i < records.size(); i++) {
        cout << "  [" << i + 1 << "/" << records.size() << "] "
             << left << setw(15) << records[i].indexNumber
             << setw(25) << records[i].studentName
             << " Current: " << records[i].status << "\n";

        int mark = 0;
        while (mark < 1 || mark > 3) {
            cout << "  Status (1=Present, 2=Absent, 3=Late): ";
            if (!(cin >> mark)) {
                clearInput(); cout << "  [ERROR] Enter 1, 2, or 3.\n"; mark = 0; continue;
            }
            clearInput();
            if (mark < 1 || mark > 3) cout << "  [ERROR] Enter 1, 2, or 3.\n";
        }
        if      (mark == 1) records[i].status = "Present";
        else if (mark == 2) records[i].status = "Absent";
        else                records[i].status = "Late";
        cout << "  Marked: " << records[i].status << "\n\n";
    }

    saveAllSessionsToFile();
    cout << "  [SUCCESS] Attendance marked and saved!\n"; printDivider('=');
}

void updateAttendance() {
    cout << "\n"; printDivider('=');
    cout << "  UPDATE ATTENDANCE RECORD\n";
    cout << "  (Enter 0 when selecting session to cancel)\n"; printDivider('=');
    if (sessions.empty()) { cout << "  [WARNING] No sessions found.\n"; return; }

    viewAllSessions();
    cout << "  Enter session NUMBER (or 0 to cancel): ";
    string input; getline(cin, input); input = trim(input);
    if (input == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }

    int sessionChoice;
    try { sessionChoice = stoi(input); } catch (...) {
        cout << "  [ERROR] Invalid input.\n"; return;
    }
    if (sessionChoice < 1 || sessionChoice > (int)sessions.size()) {
        cout << "  [ERROR] Invalid session number.\n"; return;
    }

    AttendanceSession& session = sessions[sessionChoice - 1];
    session.displayHeader();

    cout << "  Enter student Index Number (or 0 to cancel): ";
    string idx; getline(cin, idx); idx = trim(idx);
    if (idx == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }

    vector<AttendanceRecord>& records = session.getRecords();
    bool found = false;

    for (size_t i = 0; i < records.size(); i++) {
        if (toUpper(records[i].indexNumber) == toUpper(idx)) {
            found = true;
            cout << "\n  Student Found : " << records[i].studentName << "\n";
            cout << "  Current Status: " << records[i].status << "\n\n";

            int mark = 0;
            while (mark < 1 || mark > 3) {
                cout << "  New status (1=Present, 2=Absent, 3=Late): ";
                if (!(cin >> mark)) {
                    clearInput(); cout << "  [ERROR] Enter 1, 2, or 3.\n"; mark = 0; continue;
                }
                clearInput();
                if (mark < 1 || mark > 3) cout << "  [ERROR] Enter 1, 2, or 3.\n";
            }

            string old = records[i].status;
            if      (mark == 1) records[i].status = "Present";
            else if (mark == 2) records[i].status = "Absent";
            else                records[i].status = "Late";

            saveAllSessionsToFile();
            cout << "\n  [SUCCESS] Updated '" << records[i].studentName
                 << "' from '" << old << "' to '" << records[i].status << "'\n";
            break;
        }
    }

    if (!found) cout << "  [NOT FOUND] No student with index '" << idx << "' in this session.\n";
    printDivider('=');
}

// ============================================================
//  REPORTS
// ============================================================

void generateReport() {
    cout << "\n"; printDivider('=');
    cout << "  ATTENDANCE REPORT\n";
    cout << "  (Enter 0 to cancel)\n"; printDivider('=');
    if (sessions.empty()) { cout << "  [WARNING] No sessions found.\n"; return; }

    viewAllSessions();
    cout << "  Enter session NUMBER (or 0 to cancel): ";
    string input; getline(cin, input); input = trim(input);
    if (input == "0") { cout << "  [CANCELLED] Returning to menu.\n"; return; }

    int sessionChoice;
    try { sessionChoice = stoi(input); } catch (...) {
        cout << "  [ERROR] Invalid input.\n"; return;
    }
    if (sessionChoice < 1 || sessionChoice > (int)sessions.size()) {
        cout << "  [ERROR] Invalid session number.\n"; return;
    }

    const AttendanceSession& session = sessions[sessionChoice - 1];
    session.displayHeader();

    cout << left
         << setw(15) << "Index No."
         << setw(30) << "Student Name"
         << setw(10) << "Status"
         << "\n";
    printDivider('-');

    int presentCount = 0, absentCount = 0, lateCount = 0;
    const vector<AttendanceRecord>& records = session.getRecords();

    for (const auto& r : records) {
        cout << left
             << setw(15) << r.indexNumber
             << setw(30) << r.studentName
             << setw(10) << r.status
             << "\n";
        if      (r.status == "Present") presentCount++;
        else if (r.status == "Late")    lateCount++;
        else                            absentCount++;
    }

    printDivider('=');
    cout << "  SUMMARY\n"; printDivider('-');
    cout << "  Total Students : " << records.size()  << "\n";
    cout << "  Present        : " << presentCount    << "\n";
    cout << "  Late           : " << lateCount       << "\n";
    cout << "  Absent         : " << absentCount     << "\n";
    if (!records.empty()) {
        double rate = ((double)(presentCount + lateCount) / records.size()) * 100.0;
        cout << fixed << setprecision(1);
        cout << "  Attendance Rate: " << rate << "% (Present + Late)\n";
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

void attendanceMenu() {
    int choice;
    do {
        cout << "\n"; printDivider('*');
        cout << "  MARK / UPDATE ATTENDANCE\n"; printDivider('*');
        cout << "  1. Mark Attendance for a Session\n";
        cout << "  2. Update a Student's Attendance\n";
        cout << "  0. Back to Main Menu\n";
        printDivider('*');
        cout << "  Enter choice: ";
        if (!(cin >> choice)) { clearInput(); cout << "  [ERROR] Enter a number.\n"; continue; }
        clearInput();
        switch (choice) {
            case 1: markAttendance();   break;
            case 2: updateAttendance(); break;
            case 0: break;
            default: cout << "  [ERROR] Invalid choice.\n";
        }
    } while (choice != 0);
}

void reportsMenu() {
    int choice;
    do {
        cout << "\n"; printDivider('*');
        cout << "  REPORTS\n"; printDivider('*');
        cout << "  1. View Attendance Report (Screen)\n";
        cout << "  0. Back to Main Menu\n";
        printDivider('*');
        cout << "  Enter choice: ";
        if (!(cin >> choice)) { clearInput(); cout << "  [ERROR] Enter a number.\n"; continue; }
        clearInput();
        switch (choice) {
            case 1: generateReport(); break;
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
        cout << "  3. Mark / Update Attendance\n";
        cout << "  4. Reports\n";
        cout << "  0. Exit\n";
        printDivider('#');
        cout << "  Enter choice: ";
        if (!(cin >> choice)) { clearInput(); cout << "  [ERROR] Enter a number.\n"; continue; }
        clearInput();
        switch (choice) {
            case 1: studentManagementMenu(); break;
            case 2: sessionManagementMenu(); break;
            case 3: attendanceMenu();        break;
            case 4: reportsMenu();           break;
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

    cout << "\n  Loading student records...\n";
    loadStudentsFromFile();
    cout << "  " << students.size() << " student(s) loaded.\n";
    cout << "  Loading session records...\n";
    loadAllSessionsFromFile();
    cout << "\n";

    mainMenu();
    return 0;
}