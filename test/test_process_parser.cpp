#include "gtest/gtest.h"
#include "../src/ProcessParser.h"

const string DIGIT_PATTERN = "0123456789";

TEST(ProcessParserTest, GetCmdLine) {
    std::string cmd = ProcessParser::getCmd("1");
    std::cout << "Cmd: " << cmd << std::endl;
    ASSERT_TRUE(cmd.length() > 0);
}

TEST(ProcessParserTest, GetPidList) {
    std::vector<string> pid_list = ProcessParser::getPidList();
    for (string pid_str: pid_list) {
        std::cout << pid_str << std::endl;
        ASSERT_TRUE(pid_str.find_first_not_of(DIGIT_PATTERN) == string::npos);
    }
}

TEST(ProcessParserTest, GetVmSize) {
    string vmsize = ProcessParser::getVmSize("1");
    std::cout << "VM size of PID 1: " << vmsize << std::endl;
    ASSERT_TRUE(vmsize.find_first_not_of(DIGIT_PATTERN) == string::npos);
    vmsize = ProcessParser::getVmSize("17");
    std::cout << "VM size of PID 17: " << vmsize << std::endl;
    ASSERT_TRUE(vmsize.compare("N/A") == 0);
}