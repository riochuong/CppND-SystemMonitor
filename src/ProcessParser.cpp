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
        if (!stream.good()) {
            throw std::runtime_error("Cannot read cmdline of process " + pid);
        }
    }   
    catch (std::runtime_error error) {
        std::cout << "Cannot read command line of process " << pid << std::endl;
    }
    return cmd;
}