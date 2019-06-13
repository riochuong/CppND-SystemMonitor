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
    
    // retreive user id name of a process
    static string getProcUser(string pid);

    // get number of available cpu cores
    static int getNumberOfCores();
    
    static vector<string> getSysCpuPercent(string coreNumber = "");

    // print cpu stats at two specific point in time
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string> values2);

    // get total percentage ram usage on device
    static float getSysRamPercent();

    // get version of the linux kernel
    static string getSysKernelVersion();

    // return pretty os name at /etc/os-release
    static string getOSName();

    // get total number of threads used by all processes in the system
    static int getTotalThreads();
    
    // get total number of processes
    static int getTotalNumberOfProcesses();
    
    // get total number of running processes
    static int getNumberOfRunningProcesses();
    
    // check if pid existing
    static bool isPidExisting(string pid);
};
