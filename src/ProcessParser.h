#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"

using namespace std;

class ProcessParser
{
private:
    std::ifstream stream;

public:
    // the the cmd that start the process 
    static string getCmd(string pid);
    
    // get list of all the pid 
    static vector<string> getPidList();

    // get virtual memory size of a specific process 
    static std::string getVmSize(string pid);

    static std::string getCpuPercent(string pid);
    
    // get the uptime of the process 
    static std::string getProcUpTime(string pid);

    // get system up time
    static long int getSysUpTime();
    
    static string getProcUser(string pid);
    // static vector<string> getSysCpuPercent(string coreNumber = "");
    // static float getSysRamPercent();
    // static string getSysKernelVersion();
    // static int getTotalThreads();
    // static int getTotalNumberOfProcesses();
    // static int getNumberOfRunningProcesses();
    // static string getOSName();
    // static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string> values2);
    // static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:
