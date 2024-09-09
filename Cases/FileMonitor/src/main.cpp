#include <iostream>
#include <string>
#include "FileMonitor.h"

#include <fstream>
#include <csignal>

void CallBack(std::string str) 
{
    std::string line;
    std::ifstream ifs(str);
    if (!ifs.good()) {
        std::cerr << "Failed to open file: " << str << std::endl;
        return;
    }
    // just read first for test
    if (std::getline(ifs, line)) {
        std::cout << "Read line: " << line << std::endl;
    }
}

using namespace japric;

void signalHandle(int sig) 
{
    std::cout << "Receive signal: " << sig << std::endl;
    FileMonitor::stopRun();
}

int main()
{
    /* 
     * monitor a folder, or a targetFile that is not exist
     */

    struct sigaction act;
    act.sa_handler = signalHandle;
    sigemptyset(&act.sa_mask);
    // act.sa_flags |= SA_RESTART; // Don't use this for it will restart system call, so read() won't exit by signal
    sigaction(SIGINT, &act, NULL);

    std::string pathStr = "/home/vincent/";
    FileMonitor monitor(pathStr);
    monitor.registerHandle(CallBack);
    std::string targetFileName = "test.txt";
    monitor.setTargetFileName(targetFileName);

    // /* 
    //  * if monitor a exist file
    //  */
    // std::string pathStr = "/home/vincent/test.txt";
    // FileMonitor monitor(pathStr);
    // monitor.registerHandle(CallBack);


    monitor.monitor();

    return 0;
}