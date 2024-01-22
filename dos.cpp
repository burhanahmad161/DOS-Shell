#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <iomanip>
#include <list>
#include <string>
#include <wtypes.h>
#include <sstream>
#include <stack>
#include <queue>
#include <vector>
#include <limits>
#include <regex>
using namespace std;
double version = 1.0;
string start = "v:\\";
char finish = '>';
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
bool checkForBackSlashes(string name)
{
    for (int i = 0; i < name.length(); i++)
    {
        if (name[i] == '\\')
        {
            return false;
        }
    }
    return true;
}
void setCursorPosition(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
}
string PostSpaces(string s)
{
    regex pattern("\\s+$");
    return regex_replace(s, pattern, "");
}
string PreSpaces(string &inputString)
{
    regex pattern("^\\s*");
    return regex_replace(inputString, pattern, "");
}
int getArrowKey()
{
    int ch = _getch();
    if (ch == 0 || ch == 0xE0)
    {
        return _getch();
    }
    else
    {
        return ch;
    }
}
double parsItems(string itemName, int itemRate)
{
    int commaCount = 1;
    string item;
    for (int x = 0; x < itemName.length(); x++)
    {
        if (itemName[x] == ',')
        {
            commaCount = commaCount + 1;
        }
        else if (commaCount == itemRate)
        {
            item = item + itemName[x];
        }
    }
    return stod(item);
}
void storeVersion(double ver)
{
    fstream gross;
    gross.open("version.txt", ios::out);
    gross << ver << endl;
    gross.close();
}
void loadVersion()
{
    fstream gross;
    string word;
    gross.open("version.txt", ios::in);
    while (!gross.eof())
    {
        getline(gross, word);
        if (word == "")
            break;
        version = parsItems(word, 1);
    }
    gross.close();
}
void gotoxy(int x, int y)
{
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = y;
    scrn.Y = x;
    SetConsoleCursorPosition(hOuput, scrn);
}
void ConvertTime(string &Time)
{
    string AM_PM;
    string Date = Time.substr(8, 2);
    string Month = Time.substr(4, 3);
    string Day = Time.substr(0, 3);
    string Hour = Time.substr(11, 2);
    int Hr = stoi(Hour);
    if (Hr > 12)
    {
        Hr = Hr - 12;
        AM_PM = "PM";
    }
    else
    {
        AM_PM = "AM";
    }
    Hour = to_string(Hr);
    string Minute = Time.substr(14, 2);
    string Seconds = Time.substr(17, 2);
    string Year = Time.substr(20, 4);
    Time = "";
    Time = Month + " " + Date + "," + Year + "  " + Hour + ":" + Minute + ":" + Seconds + " " + AM_PM;
}
class File
{
    friend class Folders;
    friend class Doss;
    friend class Tree;

private:
    string Name;
    time_t Creation_Time;
    string Path;
    string Time;
    string extension;
    bool Hidden;
    void creationOfFile()
    {
        ofstream FileCreator(Name + extension);
        if (FileCreator.is_open())
        {
            FileCreator.close();
        }
        else
            cout << "File not created";
    }

public:
    File()
    {
    }
    File(string name, bool filler)
    {
        Name = name;
        extension = ".txt";
        Creation_Time = time(0);
    }
    File(string _Name, time_t _Creation_Time = time(0), bool _Hiddden = false, string _Extension = ".txt")
    {
        Name = _Name;
        extension = _Extension;
        Creation_Time = _Creation_Time;
        Time = ctime(&Creation_Time);
        ConvertTime(Time);
        Hidden = _Hiddden;
        creationOfFile();
    }
    void saveToFile(const string filename, const string &content)
    {
        ofstream file(filename);
        if (file.is_open())
        {
            file << content;
            file.close();
        }
        else
        {
            cout << "Unable to open file for saving: " << filename << "\n";
        }
    }
    void navigateAndEditFile(string fileName)
    {
        SetConsoleTextAttribute(hConsole, 240);
        fstream file(fileName);
        if (!file.is_open())
        {
            cout << "File does not exist";
            return;
        }
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();
        int currentPosition = 0;
        int totalCols = 80;
        stack<string> undoStack;
        stack<string> redoStack;
        auto pushToUndoStack = [&undoStack](const string &currentContent)
        {
            undoStack.push(currentContent);
            while (undoStack.size() > 5)
            {
                undoStack.pop();
            }
        };
        while (true)
        {
            system("cls");
            SetConsoleTextAttribute(hConsole, 240);
            for (int i = 0; i < content.length(); i++)
            {
                if (i == currentPosition)
                    cout << "|";
                cout << content[i];
            }
            int currentRow = 0;
            int currentCol = 0;
            for (int i = 0; i < currentCol; ++i)
            {
                if (content[i] == '\n' || currentCol == totalCols)
                {
                    currentCol = 0;
                    currentRow++;
                }
                else
                {
                    currentCol++;
                }
            }
            setCursorPosition(currentCol, currentRow);
            int key = getArrowKey();
            switch (key)
            {
            case 75: // Left arrow
                currentPosition = max(0, currentPosition - 1);
                break;
            case 77: // Right arrow
                currentPosition = min(static_cast<int>(content.size()), currentPosition + 1);
                break;
            case 80: // Down Key
                while (currentPosition < content.size() && content[currentPosition] != '\n')
                {
                    currentPosition++;
                }
                if (currentPosition < content.size())
                {
                    currentPosition++;
                }
                break;
            case 72: // Up key
                while (currentPosition > 0 && content[currentPosition - 1] != '\n')
                {
                    currentPosition--;
                }
                if (currentPosition > 0)
                {
                    currentPosition--;
                }
                break;
            case 8: // Backspace key
                if (currentPosition > 0)
                {
                    content.erase(content.begin() + currentPosition - 1);
                    currentPosition--;
                    pushToUndoStack(content);
                }
                break;
            case 27:                           // Escape key
                saveToFile(fileName, content); // Save changes before exiting
                return;
            case 13: // Enter key
                content.insert(content.begin() + currentPosition, '\n');
                currentPosition++;
                pushToUndoStack(content);
                break; // Exit the function on escape key
            case 26:   // Ctrl + Z key (undo)
                if (!undoStack.empty())
                {
                    redoStack.push(content);
                    content = undoStack.top();
                    undoStack.pop();
                    currentPosition = min(currentPosition, static_cast<int>(content.size()));
                }
                break;
            case 25: // Ctrl + Y key (redo)
                if (!redoStack.empty())
                {
                    undoStack.push(content);
                    content = redoStack.top();
                    redoStack.pop();
                    currentPosition = min(currentPosition, static_cast<int>(content.size()));
                }
                break;
            default:
                if (isprint(key))
                {
                    pushToUndoStack(content);
                    content.insert(content.begin() + currentPosition, static_cast<char>(key));
                    currentPosition++;
                }
                break;
            }
        }
    }
    bool searchInFile(string searchString, string fileName)
    {
        ifstream file(fileName);
        if (!file.is_open())
        {
            cout << "File does not exist\n";
            return false;
        }
        string line;
        while (getline(file, line))
        {
            if (line.find(searchString) != string::npos)
            {
                file.close();
                return true;
            }
        }
        cout << "String not found in file: " << Name << extension << "\n";
        file.close();
        return false;
    }
};
class Folders
{
    friend class File;
    friend class Tree;
    friend class Doss;

private:
    string Name;
    string Extension;
    Folders *Parent;
    time_t Creation_Time;
    string Time;
    list<Folders *> SubFolders;
    list<File *> Files;
    bool Hidden;

public:
    list<string> pQueue;
    list<string> printQueue;
    string Path;
    Folders(string _Name = "null", Folders *_Parent = nullptr, string _Path = " ", list<Folders *> _Folders = list<Folders *>(), list<File *> _Files = list<File *>(), bool _Hidden = false, time_t _Creation_Time = time(0))
    {
        Name = _Name;
        Parent = _Parent;
        Path = _Path;
        Files = _Files;
        Hidden = _Hidden;
        SubFolders = _Folders;
        Creation_Time = _Creation_Time;
        Time = ctime(&Creation_Time);
        ConvertTime(Time);
        string s = "";
        auto i = this;
        stack<string> lala;
        while (i != nullptr)
        {
            lala.push(i->Name + "\\");
            i = i->Parent;
        }
        while (!lala.empty())
        {
            s += lala.top();
            lala.pop();
        }
        s = s.substr(0, s.length() - 1);
        Path = s;
    }
    void insertSubFolder(string name)
    {
        string alpha;
        if (Name != "v:")
        {
            alpha = "\\";
        }
        Folders *insert = new Folders(name, this, Path + alpha + name);
        SubFolders.push_back(insert);
    }
    void insertSubFiles(string name)
    {
        File *insert = new File(name);
        Files.push_back(insert);
    }
    void removeSubFolder(string name)
    {
        list<Folders *>::iterator i;
        for (i = SubFolders.begin(); i != SubFolders.end(); i++)
        {
            if (name == (*i)->Name)
            {
                i = SubFolders.erase(i);
                break;
            }
        }
    }
    void printSubFolder()
    {
        list<Folders *>::iterator i;
        cout << "Directory of " << Name << " is:\n\n";
        for (i = SubFolders.begin(); i != SubFolders.end(); i++)
        {
            cout << left << setw(30) << (*i)->Time << " <DIR> " << (*i)->Name;
            cout << endl;
        }
    }
    void printFiles()
    {
        list<File *>::iterator i;
        for (i = Files.begin(); i != Files.end(); i++)
        {
            cout << left << setw(30) << (*i)->Time << " <FILE> " << (*i)->Name << (*i)->extension;
            cout << endl;
        }
    }
    Folders *findFolder(string name)
    {
        list<Folders *>::iterator i;
        for (i = SubFolders.begin(); i != SubFolders.end(); i++)
        {
            if ((*i)->Name == name)
            {
                return (*i);
            }
        }
        return nullptr;
    }
    File *findFile(string name)
    {
        list<File *>::iterator i;
        for (i = Files.begin(); i != Files.end(); i++)
        {
            if ((*i)->Name == name)
            {
                return (*i);
            }
        }
        return nullptr;
    }
    bool findFile2(string name)
    {
        list<File *>::iterator i;
        for (i = Files.begin(); i != Files.end(); i++)
        {
            if ((*i)->Name == name)
            {
                return true;
            }
        }
        return false;
    }
    void removeFile(string name)
    {
        for (auto it = Files.begin(); it != Files.end();)
        {
            if ((*it)->Name == name)
            {
                it = Files.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    void DeleteFolder(Folders *Root)
    {
        for (auto it = SubFolders.begin(); it != SubFolders.end();)
        {
            if (*it == Root)
            {
                (*it)->DeleteFolder();
                it = SubFolders.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    void DeleteFolder()
    {
        DeleteFolder(this);
    }
    Folders *searchFolder(string name)
    {
        list<Folders *>::iterator i;
        for (i = SubFolders.begin(); i != SubFolders.end(); i++)
        {
            if ((*i)->Name == name)
            {
                return (*i);
            }
        }
        return nullptr;
    }
    void changeExtension(string name1, string name2)
    {
        list<File *>::iterator i;
        for (i = Files.begin(); i != Files.end(); i++)
        {
            if ((*i)->extension == name1)
                (*i)->extension = name2;
            else if ((*i)->extension == name2)
                (*i)->extension = name1;
        }
    }
    void renameFile(string name, string rename)
    {
        list<File *>::iterator i;
        for (i = Files.begin(); i != Files.end(); i++)
        {
            if ((*i)->Name == name)
            {
                (*i)->Name = rename;
            }
        }
    }
    void addToQueue(string name)
    {
        pQueue.push_back(name);
    }
    void displayQueue()
    {
        if (pQueue.empty())
        {
            cout << "Print queue is empty." << endl;
        }
        else
        {
            cout << "Print Queue:" << endl;
            for (const auto &fileName : pQueue)
            {
                cout << fileName << endl;
            }
        }
    }
    void displayPriorityPrintQueueWithTime()
    {
        if (pQueue.empty())
        {
            cout << "Priority print queue is empty." << endl;
        }
        else
        {
            cout << "Priority Print Queue with Time Left:" << endl;
            time_t currentTime = time(nullptr);
            for (const auto &fileName : pQueue)
            {
                int timeLeft = rand() % 60; // Random time left in seconds
                cout << fileName << " - Time Left: " << timeLeft << " seconds" << endl;
            }
        }
    }
    void addToPriorityPrintQueue(string fileName)
    {
        if (findFile2(fileName))
        {
            addToQueue(fileName);
            displayQueue();
            cout << "File '" << fileName << "' added to the priority print queue.\n";
        }
        else
        {
            cout << "File '" << fileName << "' not found in the current directory.\n";
        }
    }

    void addToPrintQueue(string fileName)
    {
        printQueue.push_back(fileName);
    }

    void displayPrintQueue()
    {
        if (printQueue.empty())
        {
            cout << "Print queue is empty." << endl;
        }
        else
        {
            cout << "Print Queue:" << endl;
            for (const auto &fileName : printQueue)
            {
                cout << fileName << endl;
            }
        }
    }
    void displayPrintQueueWithTime()
    {
        if (printQueue.empty())
        {
            cout << "Print queue is empty." << endl;
        }
        else
        {
            cout << "Print Queue with Time Left:" << endl;
            time_t currentTime = time(nullptr);
            for (const auto &fileName : printQueue)
            {
                int timeLeft = rand() % 60;
                cout << fileName << " - Time Left: " << timeLeft << " seconds" << endl;
            }
        }
    }
    void changePrompt(char s)
    {
        finish = s;
    }
    string getLastFolder(string &path)
    {
        size_t lastSlash = path.find_last_of("\\");
        if (lastSlash != string::npos)
        {
            return path.substr(lastSlash + 1);
        }
        return path;
    }
};
class Tree
{
    friend class Dos;
    friend class Folders;
    Folders F;

private:
    Folders *findFolderByNameInFolder(Folders *currentFolder, const string &name)
    {
        if (currentFolder == nullptr)
        {
            return nullptr;
        }

        if (currentFolder->Name == name)
        {
            return currentFolder;
        }
        for (auto subfolder : currentFolder->SubFolders)
        {
            Folders *result = findFolderByNameInFolder(subfolder, name);
            if (result != nullptr)
            {
                return result;
            }
        }
        return nullptr;
    }
    Folders *findFolderByNameInSubfolders(Folders *currentFolder, const string &name)
    {
        for (auto subfolder : currentFolder->SubFolders)
        {
            Folders *result = findFolderByNameInFolder(subfolder, name);
            if (result != nullptr)
            {
                return result;
            }
            result = findFolderByNameInSubfolders(subfolder, name);
            if (result != nullptr)
            {
                return result;
            }
        }

        return nullptr;
    }
    void printTree(Folders *currentFolder, int depth)
    {
        if (currentFolder == nullptr)
            return;
        cout << setw(depth * 4) << "" << currentFolder->Name << endl;
        for (auto subfolder : currentFolder->SubFolders)
        {
            printTree(subfolder, depth + 1);
        }
        for (auto file : currentFolder->Files)
        {
            cout << setw((depth + 1) * 4) << "" << file->Name << file->extension << endl;
        }
    }

public:
    Folders *root;
    Folders *currentFolder;
    Tree()
    {
        root = currentFolder = new Folders("v:", nullptr, start);
    }
    void printTree()
    {
        printTree(root, 0);
    }
    Folders *findFolderByName(const string &name)
    {
        Folders *folderInRoot = findFolderByNameInFolder(root, name);

        if (folderInRoot != nullptr)
        {
            return folderInRoot;
        }
        return findFolderByNameInSubfolders(root, name);
    }
};
class Doss
{
    friend class Tree;
    friend class Folder;
    friend class File;

private:
    Tree T;
    Folders F;
    File file;
    string command;
    File *copiedFile;

public:
    Doss()
    {
        command = "";
    }
    bool isLoadTree()
    {
        T.currentFolder->SubFolders.clear();
        T.currentFolder->Files.clear();
        if (command != "load")
            return false;
        ifstream file("Tree.txt");
        if (file.is_open())
        {
            string s;
            while (getline(file, s))
            {
                stringstream path = stringstream(s);
                string folder;
                Folders *folderTraversal = T.root;
                int count = 0;
                for (char c : s)
                {
                    if (c == '\\')
                        count++;
                }
                for (int i = 0; i < count; i++)
                {
                    getline(path, folder, '\\');
                    folderTraversal = folderTraversal->findFolder(folder);
                }
                getline(path, folder, '\\');
                Folders *lala = new Folders(folder, folderTraversal, "");
                folderTraversal->SubFolders.push_back(lala);
            }
        }
        else
            return false;
        file.close();
        loadFiles();
        return true;
    }
    void preorderTraversal(Folders *folder, queue<string> &st)
    {
        if (folder == nullptr)
            return;
        st.push(folder->Path);
        for (auto i : folder->SubFolders)
        {
            preorderTraversal(i, st);
        }
    }
    void preorderTraversalFiles(Folders *folder, queue<string> &st)
    {
        if (folder == nullptr)
            return;
        for (auto i : folder->Files)
        {
            st.push(folder->Path + "\\" + i->Name);
        }
        for (auto i : folder->SubFolders)
        {
            preorderTraversalFiles(i, st);
        }
    }
    bool SaveTree()
    {
        if (command != "save")
            return false;
        SaveFile();
        auto x = T.root;
        queue<string> st;
        preorderTraversal(x, st);
        ofstream file("Tree.txt");
        while (!st.empty())
        {
            string i = st.front();
            st.pop();
            if (i.length() < 3)
                continue;
            i = i.substr(3);
            file << i << endl;
        }
        file.close();
    }
    void loadFiles()
    {
        ifstream file("Files.txt");
        string s;
        while (getline(file, s))
        {
            stringstream path = stringstream(s);
            string folder;
            Folders *folderTraversal = T.root;
            int count = 0;
            for (char c : s)
            {
                if (c == '\\')
                    count++;
            }
            for (int i = 0; i < count; i++)
            {
                getline(path, folder, '\\');
                folderTraversal = folderTraversal->findFolder(folder);
            }
            getline(path, folder, '\\');
            File *file = new File(folder, false);
            folderTraversal->Files.push_back(file);
        }
    }
    bool SaveFile()
    {
        if (command != "save")
            return false;
        auto x = T.root;
        queue<string> st;
        preorderTraversalFiles(x, st);
        ofstream file("Files.txt");
        while (!st.empty())
        {
            string i = st.front();
            st.pop();
            if (i.length() < 3)
                continue;
            i = i.substr(3);
            file << i << endl;
        }
        file.close();
    }
    void color(int k)
    {
        if (k > 255)
        {
            k = 1;
        }
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, k);
    }
    void profile()
    {
        storeVersion(version + 0.1);
        gotoxy(0, 72);
        color(9);
        cout << "Burhan Ahmad    "
             << "2022-CS-191\n";
    }
    bool IsMakeDirectory()
    {
        if (command.length() < 7)
            return false;
        string ChangedDirectory = command.substr(0, 5);
        if (ChangedDirectory == "mkdir")
        {
            string File_Name = command.substr(6, command.length());
            File_Name = PostSpaces(File_Name);
            File_Name = PreSpaces(File_Name);
            if (checkForBackSlashes(File_Name))
            {
                if (T.currentFolder->findFolder(File_Name))
                {
                    cout << "This folder already exist in current directory";
                    return true;
                }
                T.currentFolder->insertSubFolder(File_Name);
                return true;
            }
            else
            {
                cout << "Directory name can't contanin backslahes :)";
                return true;
            }
        }
        return false;
    }
    bool IsPrintDirectory()
    {
        if (command == "dir")
        {
            T.currentFolder->printSubFolder();
            T.currentFolder->printFiles();
            return true;
        }
        return false;
    }
    bool IsChangeDirectory()
    {
        if (command.length() < 4)
            return false;
        string ChangeDirectory = command.substr(0, 2);
        if (ChangeDirectory == "cd")
        {
            ChangeDirectory = command.substr(3, command.length());
            ChangeDirectory = PostSpaces(ChangeDirectory);
            ChangeDirectory = PreSpaces(ChangeDirectory);
            Folders *F = T.currentFolder->findFolder(ChangeDirectory);
            cout << T.currentFolder->Path;
            if (F != nullptr)
            {
                T.currentFolder = F;
                return true;
            }
            return false;
        }
        return false;
    }
    bool printTree()
    {
        if (command == "tree")
        {
            T.printTree();
            return true;
        }
        return false;
    }
    bool printVersion()
    {
        if (command == "ver")
        {
            cout << "Version is: " << version;
            return true;
        }
        return false;
    }
    bool IsClearScreen()
    {
        if (command == "cls")
        {
            system("cls");
            gotoxy(0, 72);
            color(9);
            cout << "Burhan Ahmad"
                 << " | "
                 << "2022-CS-191\n";
            return true;
        }
        return false;
    }
    bool IsGoToRoot()
    {
        if (command == "cd\\")
        {
            if (T.currentFolder == T.root)
            {
                cout << "You are already at root";
                return true;
            }
            T.currentFolder = T.root;
            return true;
        }
        return false;
    }
    bool IsGoToParent()
    {
        if (command == "cd..")
        {
            if (T.currentFolder == T.root)
            {
                cout << "You are already at root node";
                return true;
            }
            T.currentFolder = T.currentFolder->Parent;
            return true;
        }
        return false;
    }
    bool isPrintAttrib()
    {
        string name = command.substr(0, 6);
        if (name == "attrib")
        {
            name = command.substr(7, command.length());
            File *newFile = T.currentFolder->findFile(name);
            if (newFile != nullptr)
            {
                cout << "Attributes of file " << newFile->Name << " are: " << endl;
                cout << "Creation time:                " << newFile->Time << endl;
                cout << "Hidden property:              " << newFile->Hidden << endl;
                cout << "Extension:                    " << newFile->extension << endl;
                return true;
            }
            return false;
        }
        return false;
    }
    bool IsCreateFile()
    {
        if (command.length() < 8)
            return false;
        string CreateFile = command.substr(0, 6);
        if (CreateFile == "create")
        {
            CreateFile = command.substr(7, command.length());
            CreateFile = PostSpaces(CreateFile);
            CreateFile = PreSpaces(CreateFile);
            if (checkForBackSlashes(CreateFile))
            {
                if (T.currentFolder->findFile2(CreateFile))
                {
                    cout << "This file already exist in the current direcyory";
                    return true;
                }
                T.currentFolder->insertSubFiles(CreateFile);
                return true;
            }
            else
            {
                cout << "File name can't contanin backslahes :)";
                return true;
            }
        }
        return false;
    }
    bool IsEditFile()
    {
        if (command.length() < 6)
            return false;
        string name = command.substr(0, 4);
        if (name == "edit")
        {
            name = command.substr(5, command.length());
            name = PostSpaces(name);
            if (T.currentFolder->findFile2(name))
            {
                file.navigateAndEditFile(name + ".txt");
                IsClearScreen();
                return true;
            }
            else
                return false;
        }
        return false;
    }
    bool IsExit()
    {
        if (command == "exit")
        {
            exit(0);
        }
        return false;
    }
    bool isRemoveFile()
    {
        if (command.length() < 5)
            return false;
        string name = command.substr(0, 3);
        if (name == "del")
        {
            string namee = command.substr(4, command.length());
            namee = PostSpaces(namee);
            namee = PreSpaces(namee);
            if (T.currentFolder->findFile2(namee))
            {
                T.currentFolder->removeFile(namee);
                return true;
            }
            return false;
        }
        return false;
    }
    bool isFindFile()
    {
        if (command.length() < 6)
            return false;
        string name = command.substr(0, 4);
        if (name == "search")
        {
            name = command.substr(5, command.length());
            name = PostSpaces(name);
            name = PreSpaces(name);
            File *foundFile = T.currentFolder->findFile(name);
            if (foundFile != nullptr)
            {
                cout << "Found: " << foundFile->Name << endl;
            }
            else
            {
                cout << "Not found" << endl;
            }
            return true;
        }
        return false;
    }

    void ToLower()
    {
        for (int ri = 0; ri < command.size(); ri++)
        {
            if (65 <= command[ri] && command[ri] <= 90)
            {
                command[ri] = command[ri] + 32;
            }
        }
    }
    bool isDeleteDirectory()
    {
        if (command.length() < 7)
            return false;
        string name = command.substr(0, 5);
        if (name == "rmdir")
        {
            name = command.substr(6, command.length());
            Folders *newF = T.currentFolder->findFolder(name);
            if (newF != nullptr)
            {
                T.currentFolder->removeSubFolder(name);
                return true;
            }
            else
            {
                cout << "Such directory does not exist";
                return false;
            }
        }
        return false;
    }
    bool isCopy()
    {
        if (command.length() < 6)
            return false;
        Folders F;
        istringstream iss(command);
        string action, source, destination;
        iss >> action >> source >> destination;
        if (action != "copy")
            return false;
        destination = T.currentFolder->getLastFolder(destination);
        Folders *copiedPath = T.findFolderByName(destination);
        Folders *oldPath = T.currentFolder;
        if (copiedPath != nullptr)
        {
            T.currentFolder = copiedPath;
            T.currentFolder->insertSubFiles(source);
            T.currentFolder = oldPath;
            return true;
        }
        return false;
    }
    bool isMove()
    {
        if (command.length() < 7)
            return false;
        Folders F;
        istringstream iss(command);
        string action, source, destination;
        iss >> action >> source >> destination;
        if (action != "move")
            return false;
        destination = T.currentFolder->getLastFolder(destination);
        Folders *copiedPath = T.findFolderByName(destination);
        Folders *oldPath = T.currentFolder;
        T.currentFolder->removeFile(source);
        if (copiedPath != nullptr)
        {
            T.currentFolder = copiedPath;
            T.currentFolder->insertSubFiles(source);
            T.currentFolder = oldPath;
            return true;
        }
        return false;
    }
    bool isFormatDirectory()
    {
        if (command == "format")
        {
            string namee = T.currentFolder->Name;
            Folders *foundFolder = T.currentFolder->Parent;
            if (foundFolder)
            {
                foundFolder->removeSubFolder(namee);
                T.currentFolder = foundFolder;
            }
            else
            {
                cout << "Such directory don't exist";
            }
        }
    }
    bool isChangeExtension()
    {
        string extension1, extension2;
        if (command == "convert")
        {
            cout << "Enter first extension: ";
            cin >> extension1;
            extension1 = "." + extension1;
            cout << "Enter second extension: ";
            cin >> extension2;
            extension2 = "." + extension2;
            T.currentFolder->changeExtension(extension1, extension2);
            return true;
        }
        return false;
    }
    bool isHelp()
    {
        if (command.length() < 6)
            return false;
        string name = command.substr(0, 4);
        if (name == "help")
        {
            name = command.substr(5, command.length());
            if (name == "mkdir")
            {
                cout << "Helps to create a directory provided the directory name is given without slashes i.e mkdir burhan";
                return true;
            }
            else if (name == "create")
            {
                cout << "Helps to create a file provided the directory name is given without slashes i.e create burhan";
                return true;
            }
            else if (name == "attrib")
            {
                cout << "Displays all the propeties of the given file i.e attrib burhan";
                return true;
            }
            else if (name == "cd.")
            {
                cout << "Displays the current working directory";
                return true;
            }
            else if (name == "cd..")
            {
                cout << "Moves back to the parent node";
                return true;
            }
            else if (name == "cd\\")
            {
                cout << "Moves back to the root node";
                return true;
            }
            else if (name == "convert")
            {
                cout << "Asks for two extension and interchange all the extensions of current file";
                return true;
            }
            else if (name == "copy")
            {
                cout << "Copy a given file to the provided source path i.e copy burhan V:\\burhan\\ahmad";
                return true;
            }
            else if (name == "move")
            {
                cout << "Move a given file to the provided source path i.e move burhan V:\\burhan\\ahmad";
                return true;
            }
            else if (name == "del")
            {
                cout << "Delete a given file from the current directoy i.e del burhan";
                return true;
            }
            else if (name == "dir")
            {
                cout << "Displays all the directories and files of the current folder";
                return true;
            }
            else if (name == "edit")
            {
                cout << "Open a file for editing provided the name of file is given i.e edit burhan";
                return true;
            }
            else if (name == "exit")
            {
                cout << "Terminates the program";
                return true;
            }
            else if (name == "findf")
            {
                cout << "Ask the user for a string and searches for that string in the provided file";
                return true;
            }
            else if (name == "findstr")
            {
                cout << "Ask the user for a string and searches for that string in all files of current directory";
                return true;
            }
            else if (name == "format")
            {
                cout << "Delete the current directory and its subdirectories";
                return true;
            }
            else if (name == "cls")
            {
                cout << "Clear the screen";
                return true;
            }
            else if (name == "load")
            {
                cout << "Load tree from file";
                return true;
            }
            else if (name == "save")
            {
                cout << "Save tree to file";
                return true;
            }
            else if (name == "pprint")
            {
                cout << "Adds a text file to the priority based print queue, and displays the current priority queue.";
                return true;
            }
            else if (name == "pqueue")
            {
                cout << "Shows current state of the priority based print queue, with time left for each element";
                return true;
            }
            else if (name == "queue")
            {
                cout << "Shows current state of the print queue, with time left for each element";
                return true;
            }
            else if (name == "tree")
            {
                cout << "Displays the complete structure in the form of a tree";
                return true;
            }
            else if (name == "prompt")
            {
                cout << "Asks for a symbol and change symbol of working path i.e V:\\ to V&";
                return true;
            }
            else if (name == "rename")
            {
                cout << "Ask for the file whose name is to be changes and new filen name and change its name";
                return true;
            }
            else if (name == "ver")
            {
                cout << "Displays the version of DOS Shell";
                return true;
            }
            else if (name == "pwd")
            {
                cout << "Displays the current working directory.";
                return true;
            }
            else if (name == "rmdir")
            {
                cout << "Asks for a file name and delete that file";
                return true;
            }
            else
            {
                cout << "Such command does not exist";
                return true;
            }
        }
        return false;
    }
    bool isPrintCD()
    {
        if (command == "cd.")
        {
            cout << "Current working directory is: " << T.currentFolder->Name;
            return true;
        }
        return false;
    }
    bool isPrintPWD()
    {
        if (command == "pwd")
        {
            cout << "Current working directory is: " << T.currentFolder->Name;
            return true;
        }
        return false;
    }
    bool isRename()
    {
        if (command.length() < 8)
            return false;
        string re = "";
        string name = command.substr(0, 6);
        if (name == "rename")
        {
            name = command.substr(7, command.length());
            command = PostSpaces(command);
            cout << "Enter new name of file: ";
            cin >> re;
            re = PostSpaces(re);
            if (T.currentFolder->findFile2(name))
            {
                T.currentFolder->renameFile(name, re);
                return true;
            }
            else
            {
                cout << "Such file don't exist";
                return true;
            }
        }
        return false;
    }
    bool isPrompt()
    {
        char sym;
        if (command == "prompt")
        {
            cout << "Enter a sybmbol for prompt: ";
            cin >> sym;
            F.changePrompt(sym);
            system("cls");
            InputCommand();
            return true;
        }
        return false;
    }
    bool isSearchInFile()
    {
        string s_name, f_name;
        if (command == "findf")
        {
            cout << "Enter the string you want to seach: ";
            getline(cin, s_name);
            cout << "Enter the name of file in which you want to search: ";
            cin >> f_name;
            if (file.searchInFile(s_name, f_name + ".txt"))
            {
                cout << "String found";
                return true;
            }
            else
                return false;
        }
        return false;
    }
    bool isSearchInAllFiles()
    {
        string s_name;
        if (command == "findstr")
        {
            cout << "Enter the string you want to search: ";
            getline(cin, s_name);
            for (auto file : T.currentFolder->Files)
            {
                if (file->searchInFile(s_name, file->Name + file->extension))
                {
                    cout << "String found in file: " << file->Name << file->extension << endl;
                    return true;
                }
            }
            cout << "String not found in any file in the current directory." << endl;
            return true;
        }
        return false;
    }
    bool isAddtoPriorityQueue()
    {
        string name = command.substr(0, 6);
        if (name == "pprint")
        {
            name = command.substr(7, command.length());
            name = PostSpaces(name);
            name = PreSpaces(name);
            T.currentFolder->addToPriorityPrintQueue(name);
            return true;
        }
        return false;
    }
    bool isAddtoPrintQueue()
    {
        string name = command.substr(0, 5);
        if (name == "print")
        {
            name = command.substr(6, command.length());
            name = PostSpaces(name);
            name = PreSpaces(name);
            T.currentFolder->addToPrintQueue(name);
            T.currentFolder->displayPrintQueue();
            return true;
        }
        return false;
    }
    bool isPrintQueue()
    {
        if (command == "pqueue")
        {
            T.currentFolder->displayPriorityPrintQueueWithTime();
            return true;
        }
        return false;
    }
    bool isPrintQueue2()
    {
        if (command == "queue")
        {
            T.currentFolder->displayPrintQueueWithTime();
            return true;
        }
        return false;
    }
    bool errorStatement()
    {
        cout << "'" << command << "' is not recognized as an internal or external command," << endl
             << "operable program or batch file.";
        return true;
    }
    void InputCommand()
    {
        cout << T.currentFolder->Path << finish;
        getline(cin >> ws, command);
        command = PreSpaces(command);
        command = PostSpaces(command);
        ToLower();
        if (isPrintCD())
        {
        }
        else if (IsExit())
        {
        }
        else if (SaveTree())
        {
        }
        else if (IsCreateFile())
        {
        }
        else if (IsPrintDirectory())
        {
        }
        else if (IsMakeDirectory())
        {
        }
        else if (IsGoToRoot())
        {
        }
        else if (IsGoToParent())
        {
        }
        else if (IsChangeDirectory())
        {
        }
        else if (IsClearScreen())
        {
        }
        else if (IsEditFile())
        {
        }
        else if (printTree())
        {
        }
        else if (isRemoveFile())
        {
        }
        else if (isFindFile())
        {
        }
        else if (printVersion())
        {
        }
        else if (isDeleteDirectory())
        {
        }
        else if (isCopy())
        {
        }
        else if (isMove())
        {
        }
        else if (isPrintAttrib())
        {
        }
        else if (isFormatDirectory())
        {
        }
        else if (isChangeExtension())
        {
        }
        else if (isHelp())
        {
        }
        else if (isPrintPWD())
        {
        }
        else if (isRename())
        {
        }
        else if (isPrompt())
        {
        }
        else if (isSearchInFile())
        {
        }

        else if (isSearchInAllFiles())
        {
        }
        else if (isAddtoPriorityQueue())
        {
        }
        else if (isAddtoPrintQueue())
        {
        }
        else if (isPrintQueue())
        {
        }
        else if (isPrintQueue2())
        {
        }
        else if (isLoadTree())
        {
        }
        else if (errorStatement())
        {
        }
        cout << endl;
    }
};
int main()
{
    loadVersion();
    system("cls");
    Doss D;
    Tree T;
    D.profile();
    while (true)
    {
        D.InputCommand();
    }
}