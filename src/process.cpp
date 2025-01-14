#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid){
    pid_ = pid;
    command_ = LinuxParser::Command(pid);
    string ram = LinuxParser::Ram(pid);
    ram_ = std::stol(ram);
    uptime_ = LinuxParser::UpTime(pid);
    user_ = LinuxParser::User(pid);
    long totalTime = LinuxParser::ActiveJiffies(pid);
    long sec = LinuxParser::UpTime() - uptime_;
    try {cpu_utilization_ = float(totalTime) / float(sec);}catch(...){cpu_utilization_ = 0;}
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return cpu_utilization_; }

string Process::Command() { return command_; }

string Process::Ram() { return std::to_string(ram_); }

int Process::getRam(){return ram_;}

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_; }

bool Process::operator<(Process const& a) const { 
    return CpuUtilization() < a.CpuUtilization(); 
}