#include "gtest/gtest.h"
#include "../src/ProcessParser.h"


class ProcessParserTests : public testing::Test {
    
};

TEST(ProcessParserTest, GetCmdLine) {
    std::string cmd = ProcessParser::getCmd("512");
    std::cout << "Cmd: " << cmd << std::endl;
    ASSERT_TRUE(cmd.length() > 0);
}
