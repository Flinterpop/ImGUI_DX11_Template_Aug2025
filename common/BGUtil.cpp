

#include <iostream>
#include <string>

#include "BGUtil.h"

extern char g_StartupFolder[MAX_PATH];

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



bool GetOpenFileNameWithUtf8Support(HWND hwnd, char* fpath, char *initialFolder)
{
    //use the A functions for 8bit
    OPENFILENAMEA ofn = { 0 };
    char szFile[MAX_PATH] = { 0 }; // Buffer for the selected file path

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = fpath; //szFile;
    ofn.nMaxFile = MAX_PATH; //sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = "pf Files (*.pf)\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (nullptr != initialFolder) ofn.lpstrInitialDir = initialFolder;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        //std::string selected_file_path_utf8 = szFile;
        //strcpy(fpath, szFile);
        return false;
    }
    return true;
}



//FIX this - check verson in SQL_ACLIst.cpp
bool GetSaveFileNameWithUtf8Support(HWND hwnd, char* fname, char* initialFolder)
{
    OPENFILENAMEA ofn = { 0 };
    char szFileTitle[60] = {"Save Aircraft List"};
    char szFile[MAX_PATH];
    szFile[0] = '\0';
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = nullptr; //hWnd; // Handle to your parent window
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrFilter = "SQL files (*.db)\0"; 
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = fname; //szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(*szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);

    if (nullptr != initialFolder) ofn.lpstrInitialDir = initialFolder;
 
    if (0 != GetSaveFileNameA(&ofn))
    {
        return false;
    }
    else
    {
        putsRed("Error in trying to save Aircraft List");
        return true;
    }
    return false;
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

