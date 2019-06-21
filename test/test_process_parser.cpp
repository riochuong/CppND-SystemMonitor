#include "gtest/gtest.h"
#include "../src/ProcessParser.h"

const string DIGIT_PATTERN = "0123456789";
const string DIGIT_FLOAT_PATTERN = "0123456789.";


static float get_sys_active_cpu_time_test(string corenumber) {
    vector<string> values = ProcessParser::getSysCpuPercent(corenumber);
    if (values.size() == 0) return -1.0;
    std::cout << "get active time" << std::endl;
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}
static float get_sys_idle_cpu_time_test(string corenumber) {
    vector<string> values = ProcessParser::getSysCpuPercent(corenumber);
    if (values.size() == 0) return -1.0;
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

TEST(ProcessParserTest, GetCmdLine) {
    std::string cmd = ProcessParser::getCmd("1");
    std::cout << "Cmd: " << cmd << std::endl;
    ASSERT_TRUE(cmd.length() > 0);
}

TEST(ProcessParserTest, GetPidList) {
    std::vector<string> pid_list = ProcessParser::getPidList();
    for (string pid_str: pid_list) {
        //std::cout << pid_str << std::endl;
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

TEST(ProcessParserTest, GetProcUpTime) {
    string uptime = ProcessParser::getProcUpTime("1");
    std::cout << "Uptime of process " << 1 << " is " << uptime << " seconds" << std::endl;
    ASSERT_TRUE(uptime.find_first_not_of(DIGIT_FLOAT_PATTERN) == string::npos);
}

TEST(ProcessParserTest, GetSysUpTime) {
    long int uptime = ProcessParser::getSysUpTime();
    std::cout << "System Uptime: " << uptime << " seconds" << std::endl;
    ASSERT_TRUE(uptime > 0);
}

TEST(ProcessParserTest, GetProcUser) {
    string proc_user = ProcessParser::getProcUser("1");
    std::cout << "Proc username of 1 : " << proc_user << std::endl;
    ASSERT_TRUE(proc_user.compare("root") == 0);
}

TEST(ProcessParserTest, GetCpuPercent) {
    string pid = "28404";
    string usage = ProcessParser::getCpuPercent(pid);
    std::cout << "CPU usage of process " << pid << " : " << usage << " \%" << std::endl;
    ASSERT_TRUE(stof(usage) < 100.0);
    ASSERT_TRUE(usage.find_first_not_of("0123456789.") == string::npos);
}

TEST(ProcessParserTest, GetNumCores) {
    int numCores = ProcessParser::getNumberOfCores();
    std::cout << "Number of Core: " << numCores << std::endl;
    // varied on different type of machines
    ASSERT_TRUE(numCores== 8);
}

TEST(ProcessParserTest, GetSystemPercentCPU) {
    string core = "0";
    float active_time = get_sys_active_cpu_time_test(core);
    float idle_time = get_sys_idle_cpu_time_test(core);
    ASSERT_TRUE(active_time > 0.0);
    ASSERT_TRUE(idle_time > 0.0);
    std::cout << "active time " << active_time << " idle time " << idle_time << std::endl;
    active_time = get_sys_active_cpu_time_test("");
    idle_time = get_sys_idle_cpu_time_test("");
    std::cout << "active time " << active_time << " idle time " << idle_time << std::endl;
    ASSERT_TRUE(active_time > 0.0);
    ASSERT_TRUE(idle_time > 0.0);
    // nagative test case
    active_time = get_sys_active_cpu_time_test("100");
    idle_time = get_sys_idle_cpu_time_test("100");
    std::cout << "active time " << active_time << " idle time " << idle_time << std::endl;
    ASSERT_TRUE(active_time < 0.0);
    ASSERT_TRUE(idle_time < 0.0);
}

TEST(ProcessParserTest, PrintCPUStats) {
    vector<string> stat1 = ProcessParser::getSysCpuPercent("7");
    sleep(2);
    vector<string> stat2 = ProcessParser::getSysCpuPercent("7");
    string percent_active = ProcessParser::PrintCpuStats(stat1, stat2);
    std::cout << "Active time percentage over 5 sec: " << percent_active << std::endl;
    ASSERT_TRUE(stof(percent_active) > 0.000000000000000000000000000000);
}

TEST(ProcessParserTest, GetSysRamPercentage) {
    float ramPercentage = ProcessParser::getSysRamPercent();
    std::cout << "Total memory usage is at : " << ramPercentage << "\%" << std::endl;
    ASSERT_TRUE(ramPercentage > 0.0 && ramPercentage < 100.0);    
}

TEST(ProcessParserTest, GetKernelVersion) {
    string kernelVersion = ProcessParser::getSysKernelVersion();
    std::cout << "Kernel Version: " << kernelVersion << std::endl;
    ASSERT_TRUE(kernelVersion.compare("N/A") != 0);
    ASSERT_TRUE(kernelVersion[0] == '4');
}

TEST(ProcessParserTest, GetOsRelease) {
    string os_version = ProcessParser::getOSName();
    std::cout << "OS name: " << os_version << std::endl;
    ASSERT_TRUE(os_version.find("Ubuntu") != string::npos);
}

TEST(ProcessParserTest, GetTotalThreads) {
    int total_thread = ProcessParser::getTotalThreads();
    std::cout << "Total Threads: " << total_thread << std::endl;
    ASSERT_TRUE(total_thread > 1);
}

TEST(ProcessParserTest, GetTotalProcesses) {
    int total_process = ProcessParser::getTotalNumberOfProcesses();
    std::cout << "Total Processes: " << total_process << std::endl;
    ASSERT_TRUE(total_process >= 1);
}

TEST(ProcessParserTest, GetNumRunningProcess) {
    int total_running_process = ProcessParser::getNumberOfRunningProcesses();
    std::cout << "Total Number of Running Processes: " << total_running_process << std::endl;
    ASSERT_TRUE(total_running_process >= 1);
}

TEST(ProcessParserTest, IsPidExist) {
    string pid = "1";
    bool is_pid_exists = ProcessParser::isPidExisting(pid);
    std::cout << "PID " << pid << " exists :" << is_pid_exists << std::endl;
    ASSERT_TRUE(is_pid_exists);
    is_pid_exists = ProcessParser::isPidExisting("-123");
    ASSERT_FALSE(is_pid_exists);
}