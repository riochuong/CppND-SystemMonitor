#include <dirent.h>
#include "ProcessParser.h"
#include "util.h"

const string VMSIZE_PATTERN = "VmData";
const char STAT_DELIM = ' ';
const int UPTIME_IDX = 13;
const int SYS_UPTIME_IDX = 0;
const string UID_PATTERN = "Uid:";
const string DIGIT_PAT = "0123456789";
const int UID_IDX = 2;
const char UID_DELIM = ':';
const char UID_STAT_DELIM = '\t';
// core info
const string CPU_CORE_PATTERN = "cpu cores";
const char CPU_CORE_DELIM = ':';

// meminfo 
const string MEM_AVAIL_PATTERN = "MemAvailable:";
const string MEM_FREE_PATTERN = "MemFree:";
const string BUFFERS_PATTERN = "Buffers:";

// kernel version pattern
const string LINUX_VER_PATTERN = "Linux version";

// os release
const string OS_RELEASE_PATTERN = "PRETTY_NAME=";

// threads count
const string THREAD_PATTERN = "Threads:";

// processes count
const string PROCESSES_PATTERN = "processes";
const string PROCESSES_RUNNING_PATTERN = "procs_running";


std::string ProcessParser::getCmd(string pid)
{
    ifstream stream;
    std::string cmd;
    std::string cmd_path = Path::basePath() + "/" + pid + "/" + Path::cmdPath();
    // get the stream ready
    try
    {
        Util::getStream(cmd_path, stream);
        // getting here means stream is opened correctly
        std::getline(stream, cmd);
        if (!stream.good() && !stream.eof() && (stream.fail() || stream.bad()))
        {
            stream.close();
            throw std::runtime_error("Cannot read cmdline of process " + pid);
        }
    }
    catch (std::runtime_error error)
    {
        std::cout << "Cannot read command line of process " << pid << std::endl;
    }
    stream.close();
    return cmd;
}

vector<string> ProcessParser::getPidList()
{
    vector<string> pid_list;
    // open dir using the old way and match folder that represent
    // pid number and add to list
    DIR *dir = opendir(Path::basePath().c_str());
    struct dirent *entry;
    if (dir != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9')
            {
                std::string dirname(entry->d_name);
                pid_list.push_back(dirname);
            }
        }
        closedir(dir);
    }
    else
    {
        throw std::runtime_error("Cannot open /proc folder to read pid list");
    }
    return pid_list;
}

std::string ProcessParser::getVmSize(string pid)
{
    std::string status_path = Path::basePath() + pid + Path::statusPath();
    ifstream stream;
    string line;
    Util::getStream(status_path, stream);
    if (stream.is_open())
    {
        while (stream.good())
        {
            std::getline(stream, line);
            if (!line.compare(0, VMSIZE_PATTERN.length(), VMSIZE_PATTERN))
            {
                // found the VMSIZE line need to grep the size
                int start_idx = line.find_first_of("0123456789");
                int end_idx = line.find_last_of("0123456789");
                return line.substr(start_idx, end_idx - start_idx + 1);
            }
        }
    }
    return "N/A";
}

/**
 * Return the string token at the specific index of the original string read from a  
 * specific file path and delimit by a specific character
 * @param path: file to read the one-line string
 * @param idx: order of the token to return
 * @param delim: separator
 * @return: string at the index or "N/A" if not available
 */
std::string getTokenStringAtIdx(const string &path, int idx, const char delim) {
    std::string stat_path = path;
    ifstream stream;
    string line;
    Util::getStream(stat_path, stream);
    if (stream.good())
    {
        std::getline(stream, line);
        string token;
        istringstream token_stream(line);
        int count = 0;
        try
        {
            while (getline(token_stream, token, STAT_DELIM))
            {   
                // per man proc, uptime is at index 13
                if (count == idx)
                {
                    return token;
                }
                count++;
            }
        }
        catch (invalid_argument e) {
            return "N/A";
        }
    }
    return "N/A";
}

std::string ProcessParser::getProcUpTime(string pid)
{
    std::string stat_path = Path::basePath() + pid + "/" + Path::statPath();
    string uptime_str = getTokenStringAtIdx(stat_path, UPTIME_IDX, STAT_DELIM);
    long clock_ticks_per_sec = sysconf(_SC_CLK_TCK);
    if (uptime_str.find_first_not_of("0123456789") == string::npos) {
        // uptime is a good numerical value
        return to_string(float(stof(uptime_str) / clock_ticks_per_sec));
    }
    return "N/A";
}

/**
 * Return number of seconds the system has been up.
 * @ return: long int number of second the system has been up
 */
long int ProcessParser::getSysUpTime() {
    std::string sys_uptime_path = Path::basePath() + Path::upTimePath();
    string sys_uptime_str = getTokenStringAtIdx(sys_uptime_path, SYS_UPTIME_IDX, STAT_DELIM);
    if (sys_uptime_str.find_first_not_of("0123456789.") == string::npos) {
        // sys_uptime is a good numerical value
        return stoi(sys_uptime_str);
    }
    return -1;
}

/**
 * Get user name of the process id
 * @param pid: string represent process id
 * @return string: user name of a specific process
 */
string ProcessParser::getProcUser(string pid) {
    string proc_path = Path::basePath() + pid + Path::statusPath();
    ifstream stream;
    string line;
    string ruid;
    Util::getStream(proc_path, stream);
    // get the numberical value of the real Uid
    while (stream.good()) {
        std::getline(stream, line);
        if (line.compare(0, UID_PATTERN.length(), UID_PATTERN) == 0) {
            // found the Uid: pattern here
            istringstream str_stream(line);     
            while(std::getline(str_stream, ruid, UID_STAT_DELIM)) {
                if (ruid.find_first_not_of(DIGIT_PAT) == string::npos){
                    // first non string token should be real UID
                    break;
                }
            }
        }
    }

    // now find the corresponding username 
    stream.close(); // close to reuse it 
    stream.clear();
    Util::getStream(Path::uidPath(), stream);
    string uid_line;
    string uid_name;
    string pass;
    string uid_num;
    while(stream.good()) {
        std::getline(stream, uid_line);
        istringstream str_stream(uid_line);
        // if any of this fail skip the line
        if (!std::getline(str_stream, uid_name, UID_DELIM)) { continue; }
        if (!std::getline(str_stream, pass, UID_DELIM)) {continue;}
        if (!std::getline(str_stream, uid_num, UID_DELIM)) { continue; }
        if (uid_num.compare(ruid) == 0){
            return uid_name;    
        }
    }
    return "N/A";
}

string ProcessParser::getCpuPercent(string pid) {
    string proc_path = Path::basePath() + pid + "/" + Path::statPath();
    float tick_per_sec = sysconf(_SC_CLK_TCK);
    ifstream stream;
    string line;
    Util::getStream(proc_path, stream);
    if (stream.good()) {
        getline(stream, line);
    }
    // parse the line to compute CPU usage percentage
    istringstream sstream(line);
    try {
        vector<string> tokens (std::istream_iterator<string>{sstream}, std::istream_iterator<string>());
        float utime = stof(ProcessParser::getProcUpTime(pid));
        float stime = stof(tokens[14]);
        float cutime = stof(tokens[15]);
        float cstime = stof(tokens[16]);
        float starttime = stof(tokens[21]);
        float sys_up_time = ProcessParser::getSysUpTime();
        float total_run_time = utime + stime + cutime + cstime;
        float percent_usage = 100.0 * ((total_run_time / tick_per_sec)/(sys_up_time - starttime/tick_per_sec));
        return to_string(percent_usage);
    } catch (const std::exception &e) {
        std::cout << "Invalid value for conversion " << e.what() << std::endl;
        return "N/A";
    }
}

/**
 * Return number of physical core available in this computer
 * @return: int - number of physical cores
 */
int ProcessParser::getNumberOfCores() {
    string cpu_info_path = Path::basePath() + Path::cpuInfoPath();
    ifstream stream;
    string line;
    Util::getStream(cpu_info_path, stream);
    while(stream.good()) {
        string title;
        getline(stream, line);
        istringstream str_stream(line);
        getline(str_stream, title, CPU_CORE_DELIM);
        if (title.find(CPU_CORE_PATTERN, 0) != string::npos) {
            string numCore;
            getline(str_stream, numCore, CPU_CORE_DELIM);
            return stoi(numCore);
        }
    }
    return -1;

}

/**
 *  Return list of all system time related to how cpu spend time doing various type of works
 * @return: vector<string> all values related to a specific core 
 */
vector<string> ProcessParser::getSysCpuPercent(string coreNumber) {
    if (coreNumber.find_first_not_of(DIGIT_PAT) != string::npos){
        throw std::invalid_argument("Core number must be a valid number");
    }

    string cpu_name = "cpu" + coreNumber;
    string core_path = Path::basePath() + Path::statPath();
    ifstream stream;
    string line;
    Util::getStream(core_path, stream);
    while(stream.good()) {
        getline(stream, line);
        if (line.find(cpu_name, 0) == 0) {
            istringstream str_stream(line);
            vector<string> values(
                std::istream_iterator<string>{str_stream},
                std::istream_iterator<string>()
            );
            return values;
        }
    }
    // return empty list here 
    return vector<string>();
}

static float getSysActiveCpuTime(vector<string> values)
{
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

static float getSysIdleCpuTime(vector<string>values)
{
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

/**
 * Calculate active time of CPU over two certain point in time
 * @return: string value of percentage of active time over a certain periods.
 */ 
std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string> values2) {
    float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
    float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    if (activeTime < 0.0 || idleTime < 0.0) {
        throw std::runtime_error("Invalid time for calculating CPU stats");
    }
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime / totalTime);
    return to_string(result);
}

float ProcessParser::getSysRamPercent() {
    ifstream stream;
    string meminfo_line;
    string meminfo_path = Path::basePath() + Path::memInfoPath();
    float mem_avail;
    float mem_free;
    float buffers;
    int delim_idx;
    Util::getStream(meminfo_path, stream);
    if (stream.good()) {
        try {
            int count = 0;
            while (count < 3 && stream.good()) {
                getline(stream, meminfo_line);
                if (meminfo_line.compare(0, MEM_AVAIL_PATTERN.size(), MEM_AVAIL_PATTERN) == 0) {
                    count++;
                    delim_idx = meminfo_line.find_first_of(':');
                    if (delim_idx == string::npos) throw std::runtime_error("Invalid Format");
                    mem_avail = stof(meminfo_line.substr(delim_idx + 1));
                }
                else if (meminfo_line.compare(0, MEM_FREE_PATTERN.size(), MEM_FREE_PATTERN) == 0) {
                    count++;
                    delim_idx = meminfo_line.find_first_of(':');
                    if (delim_idx == string::npos) throw std::runtime_error("Invalid Format");
                    mem_free = stof(meminfo_line.substr(delim_idx + 1));
                }
                else if (meminfo_line.compare(0, BUFFERS_PATTERN.size(), BUFFERS_PATTERN) == 0) {
                    count++;
                    delim_idx = meminfo_line.find_first_of(':');
                    if (delim_idx == string::npos) throw std::runtime_error("Invalid Format");
                    buffers = stof(meminfo_line.substr(delim_idx + 1));
                }
            }
            // in case sth is going really wrong
            if (count < 3) throw std::runtime_error("Failed to retreive all memory info");
            return 100.0 * (1 - mem_free / (mem_avail - buffers));
        }
        catch (const std::exception &e) {
            std::cout << "Invalid value for conversion: " << e.what() << std::endl;
            return -1.0;
        }
    }
    // sth wrong happen
    return -1.0;
}



string ProcessParser::getSysKernelVersion() {
    string version_path = Path::basePath() + Path::versionPath();
    ifstream stream;
    string version_string;
    Util::getStream(version_path, stream);
    if (stream.good()) {
        getline(stream, version_string);
        // this should be 0 but we still need to look for it just in case some weird thing happen
        int linux_version_idx = version_string.find(LINUX_VER_PATTERN, 0);
        if (linux_version_idx == string::npos) return "N/A";
        int second_space_idx = version_string.find(' ', LINUX_VER_PATTERN.length() + 1);
        if (second_space_idx == string::npos) return "N/A";
        string final_version = version_string.substr(LINUX_VER_PATTERN.length(), second_space_idx - LINUX_VER_PATTERN.length() + 1);
        return Util::rtrim(Util::ltrim(final_version));
    }
    return "N/A";
}

string ProcessParser::getOSName() {
    string os_release_path = Path::osReleaseFullPath();
    ifstream stream;
    string line;
    Util::getStream(os_release_path, stream);
    while(stream.good()) {
        getline(stream, line);
        if (line.compare(0, OS_RELEASE_PATTERN.size(), OS_RELEASE_PATTERN) == 0) {
            int equal_sign_idx = line.find_first_of('=');
            if (equal_sign_idx == string::npos) break;
            return line.substr(equal_sign_idx + 1, line.length() - equal_sign_idx + 1);
        }
    }
    return "N/A";
}

int ProcessParser::getTotalThreads() {
    vector<string> pid_list = ProcessParser::getPidList();
    int total_thread_count = 0;
    for (auto pid_str: pid_list) {
        string pid_status_path = Path::basePath() + pid_str + Path::statusPath();
        ifstream stream;
        Util::getStream(pid_status_path, stream);
        string line;
        while(stream.good()) {
            getline(stream, line);
            if (line.compare(0, THREAD_PATTERN.size(), THREAD_PATTERN) == 0) {
                int colon_idx = line.find_first_of(':');
                if (colon_idx != string::npos) {
                    total_thread_count += stoi(line.substr(colon_idx+1));
                }
            }
        }
    }
    return total_thread_count;
}

int ProcessParser::getTotalNumberOfProcesses() {
    string proc_stat_path = Path::basePath() + Path::statPath();
    ifstream stream;
    Util::getStream(proc_stat_path, stream);
    string line;
    while (stream.good()) {
        getline(stream, line);
        if (line.compare(0, PROCESSES_PATTERN.length(), PROCESSES_PATTERN) == 0){
            int sep_idx = line.find_first_of(' ');
            return stoi(line.substr(sep_idx + 1));
        }
    }
    return -1;
}

int ProcessParser::getNumberOfRunningProcesses() {
     string proc_stat_path = Path::basePath() + Path::statPath();
    ifstream stream;
    Util::getStream(proc_stat_path, stream);
    string line;
    while (stream.good()) {
        getline(stream, line);
        if (line.compare(0, PROCESSES_RUNNING_PATTERN.length(), PROCESSES_RUNNING_PATTERN) == 0){
            int sep_idx = line.find_first_of(' ');
            return stoi(line.substr(sep_idx + 1));
        }
    }
    return -1;
}

bool ProcessParser::isPidExisting(string pid) {
    vector<string> pid_list = ProcessParser::getPidList();
    for (auto pid_str : pid_list) {
        if (pid_str.compare(0, pid.length(), pid) == 0) return true;
    }
    return false;
}