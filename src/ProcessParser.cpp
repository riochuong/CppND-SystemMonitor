#include <dirent.h>
#include "ProcessParser.h"
#include "util.h"

const string VMSIZE_PATTERN = "VmData";

std::string ProcessParser::getCmd(string pid) {
    ifstream stream;
    std::string cmd;
    std::string cmd_path = Path::basePath() + "/" + pid + "/" + Path::cmdPath();
    // get the stream ready 
    try{
        Util::getStream(cmd_path, stream);
        // getting here means stream is opened correctly
        std::getline(stream, cmd);
        if (!stream.good() && !stream.eof() && (stream.fail() || stream.bad())) {
            stream.close();
            throw std::runtime_error("Cannot read cmdline of process " + pid);
        }
    }   
    catch (std::runtime_error error) {
        std::cout << "Cannot read command line of process " << pid << std::endl;
    }
    stream.close();
    return cmd;
}

vector<string> ProcessParser::getPidList() {
    vector<string> pid_list;
    // open dir using the old way and match folder that represent
    // pid number and add to list
    DIR *dir = opendir(Path::basePath().c_str());
    struct dirent *entry;
    if (dir != NULL) {
        while((entry = readdir(dir)) != NULL){
            if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9') {
                std::string dirname(entry->d_name);
                pid_list.push_back(dirname);
            }
        }
        closedir(dir);
    }
    else {
        throw std::runtime_error("Cannot open /proc folder to read pid list");
    }
    return pid_list;
}

std::string ProcessParser::getVmSize(string pid) {
    std::string status_path = Path::basePath() + pid + Path::statusPath();
    ifstream stream;
    string line;
    Util::getStream(status_path, stream);
    if (stream.is_open()) {
        while(stream.good()) {
            std::getline(stream, line);
            if (!line.compare(0, VMSIZE_PATTERN.length(), VMSIZE_PATTERN)) {    
                // found the VMSIZE line need to grep the size
                int start_idx = line.find_first_of("0123456789");
                int end_idx = line.find_last_of("0123456789");
                return line.substr(start_idx, end_idx - start_idx + 1);
            }
        }
    }
    return "N/A";
}