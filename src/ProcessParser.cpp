#include <dirent.h>
#include "ProcessParser.h"
#include "util.h"


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