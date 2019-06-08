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