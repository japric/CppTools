#include <string>
#include <functional>

namespace japric {

enum MONI_TYPE {
    MONI_FILE,
    MONI_FOLDER,
    MONI_END,
};
class FileMonitor {
public:
    FileMonitor();
    FileMonitor(std::string str);
    ~FileMonitor();
    void monitor();
    void registerHandle(std::function<void(std::string)>);
    void setTargetFileName(std::string name);
    static void stopRun();
private:
    int fd;
    int wd;
    std::string path;
    std::string targetfileName;
    MONI_TYPE moniType;
    static bool run;
    void init();
    void exit();
    void checkType();
    void addDeviceInotify();
    bool match(std::string, int);
    std::string getRealPath();
    std::function<void(std::string)> callBack;
};

}