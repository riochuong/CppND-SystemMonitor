#include "ProcessParser.h"
#include "util.h"

std::string ProcessParser::getCmd(string pid) {
    ifstream stream;
    std::string cmd_path = Path::basePath() + Path::cmdPath();
    // get the stream ready 
    try{
        Util::getStream(cmd_path, stream);
    }   
    catch (std::runtime_error error) {
        std::cout << "Cannot read command line of process " << pid << std::endl;
    }
}