

#include <iostream>
#include <string>

#include "BGUtil.h"

#include <locale>
#include <codecvt>
#include <AppLogger.h>


//check this for wide string
bool OpenFileWithUtf8Support(HWND hwnd, char* fpath)
{
    OPENFILENAMEW ofn = { 0 };
    wchar_t szFile[MAX_PATH] = { 0 }; // Buffer for the selected file path

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    //ofn.lpstrFilter = L"All Files (*.*)\0*.*\0Text Files (*.txt)\0*.txt\0"; // Example filter
    ofn.lpstrFilter = L"pf Files (*.pf)\0";

    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn) == TRUE) {
        // The selected file path is in szFile (UTF-16)
        std::wstring selected_file_path_utf16 = szFile;

        // Convert back to UTF-8 if needed for your application's internal logic
        std::string selected_file_path_utf8 = Utf16ToUtf8(selected_file_path_utf16);

        // Now you can use selected_file_path_utf8
        strcpy(fpath, selected_file_path_utf8.c_str());
        //Load_LoadFile(szFile);
        return false;
    }
    return true;
}

bool SaveFileWithUtf8Support(HWND hwnd, char* fname)
{
    std::string utf8_path = fname; //"C:\\My Documents\\a.pf";
    int buffer_size = MultiByteToWideChar(CP_UTF8, 0, utf8_path.c_str(), -1, NULL, 0);
    std::vector<wchar_t> wide_path_buffer(buffer_size);
    MultiByteToWideChar(CP_UTF8, 0, utf8_path.c_str(), -1, wide_path_buffer.data(), buffer_size);

    // Now wide_path_buffer contains the UTF-16 representation
    // You can use wide_path_buffer.data() as lpstrFile in OPENFILENAMEW


    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = nullptr; //hWnd; // Handle to your parent window
    ofn.lpstrFilter = L"Load Files (*.pf)\0";
    ofn.lpstrFile = wide_path_buffer.data();
    ofn.nMaxFile = wide_path_buffer.size();
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;  //OFN_NOCHANGEDIR

    if (0 != GetSaveFileNameW(&ofn))  //non 0 means good to go
    {
        printf("All GOOD\r\n");
        // User selected a file
        // The selected path is in wide_path_buffer.data()
        //Save_LoadFile(wide_path_buffer.data());
    }
    else
    {
        printf("GetSaveFileNameW returned not TRUE");
    }

    return false;
}




bool g_OpenFile(HWND hwnd, char* fpath, char *filter)
{
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;  // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter = filter;//"Text Files\0*.txt\0Any File\0*.*\0";
    ofn.lpstrFile = fpath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Select a File to open";
    ofn.Flags = OFN_DONTADDTORECENT;// | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn))
    {
        std::cout << "You chose the file \"" << fpath << "\"\n";
        return false; //means no error
    }
    else
    {
        // All this stuff below is to tell you exactly how you messed up above. 
        // Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
        switch (CommDlgExtendedError())
        {
        case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
        case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
        case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
        case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
        case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
        case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
        case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
        case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
        case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
        case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
        case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
        case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
        case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
        case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
        case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
        default: std::cout << "You cancelled.\n";
        }
    }
    return true;
}

bool g_SaveFile(HWND hwnd, char* fpath, char * filter)
{
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;  // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter = filter; //"Text Files\0*.txt\0Any File\0*.*\0";
    ofn.lpstrFile = fpath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Select a Save File name";
    ofn.Flags = OFN_DONTADDTORECENT;

    if (GetSaveFileNameA(&ofn))
    {
        std::cout << "You chose the file \"" << fpath << "\"\n";
        return false;
    }
    else
    {
        // All this stuff below is to tell you exactly how you messed up above. 
        // Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
        switch (CommDlgExtendedError())
        {
        case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
        case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
        case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
        case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
        case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
        case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
        case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
        case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
        case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
        case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
        case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
        case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
        case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
        case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
        case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
        default: std::cout << "You cancelled.\n";
        }
    }
    return true;
}


std::string GetExeDirectory()
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    std::string fullPath = exePath;
    size_t lastSlashPos = fullPath.find_last_of("\\");

    if (lastSlashPos != std::string::npos) {
        return fullPath.substr(0, lastSlashPos + 1); // +1 to include the backslash
    }
    return ""; // Or handle error appropriately
}



std::wstring convertUTF8ToWstring(const std::string& utf8_string)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8_string);
}


// Function to convert UTF-8 to UTF-16
std::wstring Utf8ToUtf16(const std::string& utf8_string) {
    if (utf8_string.empty()) {
        return L"";
    }
    int num_chars = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, NULL, 0);
    std::vector<wchar_t> wide_buffer(num_chars);
    MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, &wide_buffer[0], num_chars);
    return std::wstring(&wide_buffer[0]);
}

// Function to convert UTF-16 to UTF-8
std::string Utf16ToUtf8(const std::wstring& utf16_string) {
    if (utf16_string.empty()) {
        return "";
    }
    int num_bytes = WideCharToMultiByte(CP_UTF8, 0, utf16_string.c_str(), -1, NULL, 0, NULL, NULL);
    std::vector<char> utf8_buffer(num_bytes);
    WideCharToMultiByte(CP_UTF8, 0, utf16_string.c_str(), -1, &utf8_buffer[0], num_bytes, NULL, NULL);
    return std::string(&utf8_buffer[0]);
}




float degreesToMils(float deg)
{
    float retVal = 6400 * deg / 360.0;
    return retVal;
}


/*
void Convert_LatLng_To_MGRS(double lat, double lng, int precision, char* m_MGRS)
{
    double latRad = DEG2RAD(lat);// *PI / 180;
    double lngRad = DEG2RAD(lng);// *PI / 180;
    long retVal = Convert_Geodetic_To_MGRS(latRad, lngRad, 4, m_MGRS);
}
*/



std::string utf16_to_utf8_winapi(const std::wstring& utf16_string) {
    if (utf16_string.empty())   return "";

    int utf8_length = WideCharToMultiByte(CP_UTF8, 0, utf16_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8_length == 0) {
        return ""; // Handle error
    }
    std::vector<char> utf8_buffer(utf8_length);
    WideCharToMultiByte(CP_UTF8, 0, utf16_string.c_str(), -1, utf8_buffer.data(), utf8_length, nullptr, nullptr);
    char s[100];
    int i = 0;
    for (auto a : utf8_buffer)
    {
        s[i++] = a;
    }
    s[i] = 0;
    printf("Oh my its: %s\r\n", s);
    return std::string(utf8_buffer.begin(), utf8_buffer.end() - 1); // Remove null terminator
}

int utf16_to_utf8(const std::wstring& utf16_string, char* dest) {
    if (utf16_string.empty())   return 0;

    int utf8_length = WideCharToMultiByte(CP_UTF8, 0, utf16_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8_length == 0) return 0; // Handle error

    std::vector<char> utf8_buffer(utf8_length);
    WideCharToMultiByte(CP_UTF8, 0, utf16_string.c_str(), -1, utf8_buffer.data(), utf8_length, nullptr, nullptr);

    int i = 0;
    for (auto a : utf8_buffer) dest[i++] = a;
    dest[i] = 0;
    return i;
}




void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
    // Note that LONGLONG is a 64-bit value
    LONGLONG ll;

    ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD)ll;
    pft->dwHighDateTime = ll >> 32;
}



std::uintmax_t calculateTotalFileSize(const fs::path& directory)
{
    fs::file_status s = fs::file_status{};
    if (fs::status_known(s) ? fs::exists(s) : fs::exists(directory))
    {
        std::uintmax_t totalSize = 0;
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (fs::is_regular_file(entry)) {
                totalSize += fs::file_size(entry);
            }
        }
        return totalSize;
    }


    return 0;
}




int GetSecondsSinceMidnightZulu()
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    //printf("The system time is: %02d:%02d\n", st.wHour, st.wMinute);
    double elapsed = st.wHour * 3600 + st.wMinute * 60 + st.wSecond;
    return (int)elapsed;
}


void GetHMSSinceMidnightZulu(int& hrs, int& mins, int& secs)
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    //printf("The system time is: %02d:%02d\n", st.wHour, st.wMinute);
    double elapsed = st.wHour * 3600 + st.wMinute * 60 + st.wSecond;
    hrs = st.wHour;
    mins = st.wMinute;
    secs = st.wSecond;

}


bool getFileNameNoExtension(const char* path, char* dest)
{
    char path_buffer[_MAX_PATH];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    errno_t err;

    err = _splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
    if (err != 0)
    {
        printf("Error splitting the path. Error code %d.\n", err);
        return true;
    }
    //printf("Path extracted with _splitpath_s:\n");
    //printf("   Drive: %s\n", drive);
    //printf("   Dir: %s\n", dir);
    //printf("   Filename: %s\n", fname);
    //printf("   Ext: %s\n", ext);

    strcpy(dest, fname);

    return false;
}


bool getFileNameExtension(const char* path, char* dest)
{
    char path_buffer[_MAX_PATH];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    errno_t err;

    err = _splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
    if (err != 0)
    {
        printf("Error splitting the path. Error code %d.\n", err);
        return true;
    }
    //printf("Path extracted with _splitpath_s:\n");
    //printf("   Drive: %s\n", drive);
    //printf("   Dir: %s\n", dir);
    //printf("   Filename: %s\n", fname);
    //printf("   Ext: %s\n", ext);

    strcpy(dest, ext);

    return false;
}

#include <iostream>   
#include <direct.h>   
using namespace std;


//check for crashes
std::vector<std::string> LoadFileNamesInFolder(char * folderName, char * fileType)
{
    std::vector<std::string> fileList;
    
    const size_t size = 1024;
    char CurDir[size];
    if (getcwd(CurDir, size) != NULL) 
    {
        cout << "Current working directory: " << CurDir << endl;
    }
    else {
        // If _getcwd returns NULL, print an error message
        cerr << "Error getting current working directory" << endl;
        return fileList;
    }


    char bigBuf[200];
    sprintf(bigBuf, "%s\\%s\\", CurDir, folderName);
    printf("bigBuf: %s\r\n", bigBuf);


    //from https://www.geeksforgeeks.org/cpp-program-to-get-the-list-of-files-in-a-directory/
    // This structure would distinguish a file from a directory
    struct stat sb;
    // Looping until all the items of the directory are exhausted
    int count = 0;
    //for (const auto& entry : std::filesystem::directory_iterator(CurDir))
    for (const auto& entry : std::filesystem::recursive_directory_iterator(bigBuf))
    //for (const auto& entry : std::filesystem::recursive_directory_iterator("C:\\source\\git_Maps\\IM_MAPSDX11\\RunFolder\\MS2525C\\"))
        
    {
        // Converting the path to const char * in the subsequent lines
        std::filesystem::path outfilename = entry.path();
        std::string outfilename_str = outfilename.string();
        const char* path = outfilename_str.c_str();

        // Testing whether the path points to a non-directory or not If it does, displays path
        if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR))
        {
            char justTheExtension[40] = "";
            getFileNameExtension(path, justTheExtension);
            if (!strncmp(justTheExtension, fileType, 4))
            {
                printf("Filename: %s\n", path);
                fileList.push_back(path);
            }
            //if (fileList.size() > 40) break;
        }
    }
    return fileList;
}



void putsRed(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[200];
    vsprintf(buf, fmt, args);

    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, (char *)buf);
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "\r\n");

    va_end(args);
}

void putsGreen(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[200];
    vsprintf(buf, fmt, args);
    
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, (char*)buf);
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "\r\n");
    
    va_end(args);
}

void putsYellow(const char* fmt, ...)
{
    
    va_list args;
    va_start(args, fmt);
    char buf[200];
    vsprintf(buf, fmt, args);

    fmt::print(fg(fmt::color::yellow) | fmt::emphasis::bold, (char*)buf);
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "\r\n");
    
    va_end(args);
}

void putsSilver(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[200];
    vsprintf(buf, fmt, args);

    fmt::print(fg(fmt::color::silver) | fmt::emphasis::bold, (char*)buf);
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "\r\n");
    
    va_end(args);
}

void putsBlue(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[200];
    vsprintf(buf, fmt, args);

    fmt::print(fg(fmt::color::cadet_blue) | fmt::emphasis::bold, (char*)buf);
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "\r\n");

    va_end(args);
}

void putsColor(fmt::color col, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[200];
    vsprintf(buf, fmt, args);
   
    fmt::print(fg(col) | fmt::emphasis::bold, (char*)buf);
    fmt::print(fg(col) | fmt::emphasis::bold, "\r\n");

    va_end(args);
}





void HexDump(int bufLen, unsigned char* buf)
{

    bool error = false;
    printf("\r\nDumping %d bytes.\r\n", bufLen);
    char textLine[100] = "";
    for (int x = 0; x < bufLen; x++)
    {
        char c = (char)buf[x];
        //if (isalnum(c))
        //    textLine[(x % 16)] = c;
        //else
        textLine[(x % 16)] = '.';

        if (!((x) % 16)) printf("%02X: ", x);
        printf("%02X ", (unsigned short)buf[x]);

        if (!((x + 1) % 16)) {
            textLine[(x % 16) + 1] = 0;
            printf("   %s", textLine);
            printf("\r\n");
        }
    }
    printf("\r\n");
}
