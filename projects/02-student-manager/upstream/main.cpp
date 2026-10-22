#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

// ============================================================
// STUDENT CLASS
// ============================================================

class Student
{
private:
    int rollNumber;
    string name;
    int age;
    string department;
    double gpa;

public:
    // Default constructor
    Student()
    {
        rollNumber = 0;
        age = 0;
        gpa = 0.0;
    }

    // Parameterized constructor
    Student(int roll, string studentName, int studentAge,
            string dept, double studentGpa)
    {
        rollNumber = roll;
        name = studentName;
        age = studentAge;
        department = dept;
        gpa = studentGpa;
    }

    // Getters
    int getRollNumber() const
    {
        return rollNumber;
    }

    string getName() const
    {
        return name;
    }

    int getAge() const
    {
        return age;
    }

    string getDepartment() const
    {
        return department;
    }

    double getGpa() const
    {
        return gpa;
    }

    // Setters
    void setName(const string &newName)
    {
        name = newName;
    }

    void setAge(int newAge)
    {
        age = newAge;
    }

    void setDepartment(const string &newDept)
    {
        department = newDept;
    }

    void setGpa(double newGpa)
    {
        gpa = newGpa;
    }

    // Calculate Letter Grade based on GPA
    string getGrade() const
    {
        if (gpa >= 3.70) return "A+";
        if (gpa >= 3.30) return "A";
        if (gpa >= 3.00) return "B+";
        if (gpa >= 2.70) return "B";
        if (gpa >= 2.30) return "C+";
        if (gpa >= 2.00) return "C";
        if (gpa >= 1.00) return "D";
        return "F";
    }

    // Display single student detailed record
    void displayStudent() const
    {
        cout << "\n---------------------------------------------\n";
        cout << "Roll Number : " << rollNumber << endl;
        cout << "Name        : " << name << endl;
        cout << "Age         : " << age << endl;
        cout << "Department  : " << department << endl;
        cout << "GPA         : " << fixed << setprecision(2) << gpa << endl;
        cout << "Grade       : " << getGrade() << endl;
        cout << "---------------------------------------------\n";
    }

    // Display single student row in tabular format
    void displayTableRow() const
    {
        cout << left
             << setw(10) << rollNumber
             << setw(24) << name
             << setw(8)  << age
             << setw(22) << department
             << setw(8)  << fixed << setprecision(2) << gpa
             << setw(8)  << getGrade()
             << endl;
    }

    // Save record to file in pipe-separated format
    void saveToFile(ofstream &file) const
    {
        file << rollNumber << "|"
             << name << "|"
             << age << "|"
             << department << "|"
             << fixed << setprecision(2) << gpa << endl;
    }
};


// ============================================================
// STUDENT MANAGEMENT SYSTEM CLASS
// ============================================================

class StudentManagementSystem
{
private:
    vector<Student> students;
    const string dataFile = "students.txt";

    // Helper: Find student by Roll Number (returns pointer)
    Student* findStudentByRoll(int rollNumber)
    {
        for (auto &student : students)
        {
            if (student.getRollNumber() == rollNumber)
            {
                return &student;
            }
        }
        return nullptr;
    }

    // Helper: Check if roll number already exists
    bool rollNumberExists(int rollNumber) const
    {
        for (const auto &student : students)
        {
            if (student.getRollNumber() == rollNumber)
            {
                return true;
            }
        }
        return false;
    }

    // Helper: Save all students to flat text file
    void saveToFile()
    {
        ofstream file(dataFile);

        if (!file)
        {
            cout << "Error: Unable to open file for saving.\n";
            return;
        }

        for (const auto &student : students)
        {
            student.saveToFile(file);
        }

        file.close();
    }

public:
    // ========================================================
    // LOAD STUDENTS FROM FILE
    // ========================================================

    void loadFromFile()
    {
        ifstream file(dataFile);

        if (!file)
        {
            // File does not exist yet; will be created on first save
            return;
        }

        students.clear();
        string line;

        while (getline(file, line))
        {
            if (line.empty())
                continue;

            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);
            size_t pos3 = line.find('|', pos2 + 1);
            size_t pos4 = line.find('|', pos3 + 1);

            if (
                pos1 == string::npos ||
                pos2 == string::npos ||
                pos3 == string::npos ||
                pos4 == string::npos
            )
            {
                continue;
            }

            try
            {
                int roll = stoi(line.substr(0, pos1));
                string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
                int age = stoi(line.substr(pos2 + 1, pos3 - pos2 - 1));
                string dept = line.substr(pos3 + 1, pos4 - pos3 - 1);
                double gpa = stod(line.substr(pos4 + 1));

                students.emplace_back(roll, name, age, dept, gpa);
            }
            catch (...)
            {
                continue;
            }
        }

        file.close();
    }


    // ========================================================
    // 1. ADD STUDENT
    // ========================================================

    void addStudent()
    {
        int roll;
        string name;
        int age;
        string department;
        double gpa;

        cout << "\n========== ADD NEW STUDENT ==========\n";

        cout << "Enter Roll Number (ID): ";
        cin >> roll;

        if (cin.fail() || roll <= 0)
        {
            cout << "Invalid roll number. Must be a positive integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        if (rollNumberExists(roll))
        {
            cout << "Error: A student with Roll Number " << roll << " already exists.\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter Student Name: ";
        getline(cin, name);

        if (name.empty())
        {
            cout << "Error: Name cannot be empty.\n";
            return;
        }

        cout << "Enter Age: ";
        cin >> age;

        if (cin.fail() || age < 10 || age > 100)
        {
            cout << "Invalid age. Must be between 10 and 100.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter Department: ";
        getline(cin, department);

        if (department.empty())
        {
            cout << "Error: Department cannot be empty.\n";
            return;
        }

        cout << "Enter GPA (0.00 - 4.00): ";
        cin >> gpa;

        if (cin.fail() || gpa < 0.0 || gpa > 4.0)
        {
            cout << "Invalid GPA. Must be between 0.00 and 4.00.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        Student newStudent(roll, name, age, department, gpa);
        students.push_back(newStudent);

        saveToFile();

        cout << "\nStudent record added successfully!\n";
        cout << "Roll Number: " << roll << " | Name: " << name << endl;
    }


    // ========================================================
    // 2. DISPLAY ALL STUDENTS
    // ========================================================

    void displayAllStudents() const
    {
        cout << "\n========== ALL STUDENT RECORDS ==========\n";

        if (students.empty())
        {
            cout << "No student records found.\n";
            return;
        }

        cout << "\n";
        cout << left
             << setw(10) << "Roll No"
             << setw(24) << "Name"
             << setw(8)  << "Age"
             << setw(22) << "Department"
             << setw(8)  << "GPA"
             << setw(8)  << "Grade"
             << endl;

        cout << string(80, '-') << endl;

        for (const auto &student : students)
        {
            student.displayTableRow();
        }

        cout << string(80, '-') << endl;
        cout << "Total Students: " << students.size() << endl;
    }


    // ========================================================
    // 3. SEARCH STUDENT
    // ========================================================

    void searchStudent()
    {
        cout << "\n========== SEARCH STUDENT ==========\n";
        cout << "1. Search by Roll Number\n";
        cout << "2. Search by Name\n";
        cout << "Enter your choice: ";

        int searchChoice;
        cin >> searchChoice;

        if (cin.fail() || (searchChoice != 1 && searchChoice != 2))
        {
            cout << "Invalid search choice.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        if (searchChoice == 1)
        {
            int roll;
            cout << "Enter Roll Number: ";
            cin >> roll;

            if (cin.fail())
            {
                cout << "Invalid roll number format.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }

            Student *student = findStudentByRoll(roll);

            if (student == nullptr)
            {
                cout << "Student with Roll Number " << roll << " not found.\n";
                return;
            }

            student->displayStudent();
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string searchName;
            cout << "Enter Student Name to search: ";
            getline(cin, searchName);

            if (searchName.empty())
            {
                cout << "Name cannot be empty.\n";
                return;
            }

            string lowerSearch = searchName;
            transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);

            bool found = false;

            for (const auto &student : students)
            {
                string studentNameLower = student.getName();
                transform(studentNameLower.begin(), studentNameLower.end(), studentNameLower.begin(), ::tolower);

                if (studentNameLower.find(lowerSearch) != string::npos)
                {
                    if (!found)
                    {
                        cout << "\nMatching Records Found:\n";
                        found = true;
                    }
                    student.displayStudent();
                }
            }

            if (!found)
            {
                cout << "No student found matching: \"" << searchName << "\"\n";
            }
        }
    }


    // ========================================================
    // 4. UPDATE STUDENT
    // ========================================================

    void updateStudent()
    {
        int roll;

        cout << "\n========== UPDATE STUDENT RECORD ==========\n";
        cout << "Enter Roll Number of student to update: ";
        cin >> roll;

        if (cin.fail())
        {
            cout << "Invalid roll number format.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        Student *student = findStudentByRoll(roll);

        if (student == nullptr)
        {
            cout << "Student with Roll Number " << roll << " not found.\n";
            return;
        }

        cout << "\nCurrent Details:\n";
        student->displayStudent();

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string newName;
        cout << "Enter New Name (leave blank to keep current): ";
        getline(cin, newName);
        if (!newName.empty())
        {
            student->setName(newName);
        }

        string ageInput;
        cout << "Enter New Age (leave blank to keep current): ";
        getline(cin, ageInput);
        if (!ageInput.empty())
        {
            try
            {
                int newAge = stoi(ageInput);
                if (newAge >= 10 && newAge <= 100)
                {
                    student->setAge(newAge);
                }
                else
                {
                    cout << "Invalid age ignored. Kept current age.\n";
                }
            }
            catch (...)
            {
                cout << "Invalid age format ignored. Kept current age.\n";
            }
        }

        string newDept;
        cout << "Enter New Department (leave blank to keep current): ";
        getline(cin, newDept);
        if (!newDept.empty())
        {
            student->setDepartment(newDept);
        }

        string gpaInput;
        cout << "Enter New GPA (leave blank to keep current): ";
        getline(cin, gpaInput);
        if (!gpaInput.empty())
        {
            try
            {
                double newGpa = stod(gpaInput);
                if (newGpa >= 0.0 && newGpa <= 4.0)
                {
                    student->setGpa(newGpa);
                }
                else
                {
                    cout << "Invalid GPA range ignored. Kept current GPA.\n";
                }
            }
            catch (...)
            {
                cout << "Invalid GPA format ignored. Kept current GPA.\n";
            }
        }

        saveToFile();

        cout << "\nStudent record updated successfully!\n";
        cout << "Updated Record:\n";
        student->displayStudent();
    }


    // ========================================================
    // 5. DELETE STUDENT
    // ========================================================

    void deleteStudent()
    {
        int roll;

        cout << "\n========== DELETE STUDENT RECORD ==========\n";
        cout << "Enter Roll Number of student to delete: ";
        cin >> roll;

        if (cin.fail())
        {
            cout << "Invalid roll number format.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        for (auto it = students.begin(); it != students.end(); ++it)
        {
            if (it->getRollNumber() == roll)
            {
                cout << "\nStudent Found:\n";
                it->displayStudent();

                char confirmation;
                cout << "Are you sure you want to delete this student record? (Y/N): ";
                cin >> confirmation;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (confirmation == 'Y' || confirmation == 'y')
                {
                    students.erase(it);
                    saveToFile();
                    cout << "Student record deleted successfully.\n";
                }
                else
                {
                    cout << "Deletion cancelled.\n";
                }

                return;
            }
        }

        cout << "Student with Roll Number " << roll << " not found.\n";
    }


    // ========================================================
    // MAIN MENU
    // ========================================================

    void menu()
    {
        int choice;

        do
        {
            cout << "\n";
            cout << "=============================================\n";
            cout << "          STUDENT MANAGEMENT SYSTEM\n";
            cout << "=============================================\n";
            cout << "1. Add New Student\n";
            cout << "2. Display All Students\n";
            cout << "3. Search Student\n";
            cout << "4. Update Student Record\n";
            cout << "5. Delete Student Record\n";
            cout << "6. Exit\n";
            cout << "=============================================\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (cin.fail())
            {
                cout << "Invalid input. Please enter a number between 1 and 6.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            switch (choice)
            {
                case 1:
                    addStudent();
                    break;

                case 2:
                    displayAllStudents();
                    break;

                case 3:
                    searchStudent();
                    break;

                case 4:
                    updateStudent();
                    break;

                case 5:
                    deleteStudent();
                    break;

                case 6:
                    cout << "\nThank you for using Student Management System.\n";
                    break;

                default:
                    cout << "Invalid choice. Please select 1-6.\n";
            }

        } while (choice != 6);
    }
};


// ============================================================
// MAIN FUNCTION
// ============================================================

int main()
{
    StudentManagementSystem system;

    // Load persisted student records from file
    system.loadFromFile();

    // Start menu-driven console interface
    system.menu();

    return 0;
}
