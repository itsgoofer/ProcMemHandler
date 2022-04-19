////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Copyright 2022 goofer																								  //
//
Permission is hereby granted, free of charge, to any person																		  //
obtaining a copy of this software and associated documentation                                                                    //
files(the "Software"), to deal in the Software without restriction,                                                               //
including without limitation the rights to use, copy, modify, merge,                                                              //
publish, distribute, sublicense, and/or sell copies of the Software,                                                              //
and to permit persons to whom the Software is furnished to do so,                                                                 //
subject to the following conditions :                                                                                             //
                                                                                                                                  //
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.    //
                                                                                                                                  //
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,																	  //
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF																  //
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.                                                            //
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,                                                       //
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR                                                             //
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR                                                             //
THE USE OR OTHER DEALINGS IN THE SOFTWARE.																						  //
*/																																  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <windows.h>
#include <TlHelp32.h>

HANDLE hProc = NULL;
DWORD pID;


// Scans through all running processes
bool attachProc(char* procName) {
    
    PROCESSENTRY32 procEntry32{};
    procEntry32.dwSize = sizeof(PROCESSENTRY32);

    auto hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcSnap == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to take snapshot of processes..." << std::endl;
        return false;
    }

    std::cout << "Reading List of Running Processes..." << std::endl;

    while (Process32Next(hProcSnap, &procEntry32)) {

        std::cout << procEntry32.szExeFile << std::endl;

        if (!strcmp(procName, procEntry32.szExeFile)) {

            std::cout << "\nFound Process " << procEntry32.szExeFile << " with process ID " << procEntry32.th32ProcessID << std::endl;
            hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procEntry32.th32ProcessID);
            pID = procEntry32.th32ProcessID;

            if (hProc == NULL) {
                std::cout << "Failed getting handle to process." << std::endl;
            }

            CloseHandle(hProcSnap);
            return true;
        }
    }

    std::cout << "Couldn't find " << procName << " in the process snapshot" << std::endl;
    CloseHandle(hProcSnap);
    return false;
}

//
// writes a value in the target memory address
//
template <class dataType> 
void wpm(dataType valToWrite, DWORD64 targetAddress) {
    WriteProcessMemory(hProc, (PVOID)targetAddress, &valToWrite, sizeof(dataType), 0);
}


//
// reads the value in the target memory address
//
template <class dataType> 
dataType rpm(DWORD64 targetAddress) {
    dataType rpmBuffer;
    ReadProcessMemory(hProc, (PVOID)targetAddress, &rpmBuffer, sizeof(dataType), 0);
    return rpmBuffer;
}

int main() {

    DWORD64 memAddress = 0x1800CFF5C4;

    if (attachProc((char*)"FakeGame.exe") == true) {
        std::cout << "Attached to process..." << std::endl;
    }

    while (true) {
        Sleep(1000);
        wpm(1000, memAddress);
    }
}