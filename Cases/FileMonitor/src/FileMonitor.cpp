#include <iostream>
#include <string>
#include <sys/inotify.h>
#include <unistd.h>
#include <filesystem>
#include <functional>
#include <csignal>
#include "FileMonitor.h"


namespace fs = std::filesystem;

namespace japric {

#define INOTIFY_FILE_MASK (IN_MODIFY)
#define INOTIFY_FOLDER_MASK (IN_CREATE | IN_MODIFY | IN_DELETE)
static int g_MoniMasks[MONI_END] = {INOTIFY_FILE_MASK, INOTIFY_FOLDER_MASK};

bool FileMonitor::run = false;

FileMonitor::FileMonitor(): FileMonitor("")
{
}

FileMonitor::FileMonitor(std::string str): fd(-1), wd(-1), path(str)
{
    init();
}

FileMonitor::~FileMonitor() 
{
    exit();
}

void FileMonitor::init() 
{
    std::cout << "Init file monitor:" << path << std::endl;
    fd = inotify_init();
    if (fd == -1) {
        std::cerr << "Failed to initialize inotify" << std::endl;
        return;
    }
    checkType();
    addDeviceInotify();
}

void FileMonitor::exit() 
{
    if (fd == -1) {
        return;
    }
    if (wd != -1) {
        inotify_rm_watch(fd, wd);
    }
    close(fd);
    std::cout << "Exit file monitor" << std::endl;
}

void FileMonitor::checkType() 
{
    if (fs::is_regular_file(path)) {
        moniType = MONI_FILE;
    } else if (fs::is_directory(path)) {
        moniType = MONI_FOLDER;
    } else {
        std::cerr << "illegal path!" << std::endl;
        return;
    }
    std::cout << "moniType:" << moniType << std::endl;
}

void FileMonitor::addDeviceInotify() 
{
    wd = inotify_add_watch(fd, path.c_str(), g_MoniMasks[moniType]);
    if (wd == -1) {
        std::cerr << "Failed to add watch to [" << path << "]" << ", error:" + std::to_string(errno) << std::endl;
        return;
    }
    run = true;
}

void FileMonitor::monitor() 
{
    while (run) {
        char buf[1024];
        int len = read(fd, buf, sizeof(buf));
        if (len == -1) {
            if (errno == EINTR) {
                std::cerr << "Interrupted by signal" << std::endl;
            }
            break;
        }
        std::cout << "read ok, len:" << len << std::endl;
        for (int i = 0; i < len;) {
            struct inotify_event *event = (struct inotify_event *)&buf[i];
            i += sizeof(struct inotify_event) + event->len;
            if (event->mask & IN_CREATE) {
                std::cout << "File created: " << event->name << std::endl;
            } else if (event->mask & IN_MODIFY) {
                std::cout << "File modified: " << event->name << std::endl;
                if (!match(event->name, event->wd)) {
                    continue;
                }
                if (callBack) {
                    callBack(getRealPath());
                } else {
                    std::cerr << "No handle function registered" << std::endl;
                }
            } else if (event->mask & IN_DELETE) {
                std::cout << "File deleted: " << event->name << std::endl;
            }
        }
    }
}

void FileMonitor::registerHandle(std::function<void(std::string)> func) 
{
    callBack = func;
}

bool FileMonitor::match(std::string name, int wd)
{
    if (moniType == MONI_FILE) {
        return wd == this->wd;
    } else if (moniType == MONI_FOLDER) {
        return name == targetfileName;
    }
    return false;
}

std::string FileMonitor::getRealPath() 
{
    if (moniType == MONI_FILE) {
        return path;
    } else if (moniType == MONI_FOLDER) {
        return path + "/" + targetfileName;
    }
    return "";
}

void FileMonitor::setTargetFileName(std::string name) 
{
    targetfileName = name;
}

void FileMonitor::stopRun() 
{
    run = false;
}

} // namespace japric
