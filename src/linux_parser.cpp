#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and returned the system memory utilization
float LinuxParser::MemoryUtilization() {
  string str;
  float total, free;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(!stream.is_open())return 0;
  while(std::getline(stream, str)){
    std::istringstream line_stream(str);
    string str_key;
    line_stream >> str_key;
    if(str_key == "MemTotal:")line_stream >> total;
    if(str_key == "MemAvailable:"){line_stream >> free; break;}
  }
  return total > 0 ? (total-free)/total : 0.0;
 }

// Read and returned the system uptime
long LinuxParser::UpTime() { 
  long upTime = 0;
  string str;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(!stream.is_open())return upTime;
  std::getline(stream, str);
  std::istringstream line_stream(str);
  line_stream >> upTime;
  return upTime;
 }

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
 }

// Read and returned the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  long result = 0, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(!stream.is_open())return result;
  vector<long>values;
  string str;
  std::getline(stream, str);
  std::istringstream line_stream(str);
  while(line_stream >> value)values.push_back(value);
  if(values.size() > 21){
    result += values[13] + values[14] + values[15] + values[16];
  }
  return result / sysconf(_SC_CLK_TCK); // division added to return active time in seconds instead of raw jiffies.
 }

// Read and returned the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  auto jiffies = CpuUtilization();
  vector<long> values; for(auto &e:jiffies)values.push_back(std::stol(e));
  if(values.size() >= LinuxParser::CPUStates::kSteal_){
    return values[LinuxParser::CPUStates::kUser_] + values[LinuxParser::CPUStates::kNice_] + values[LinuxParser::CPUStates::kSystem_] + values[LinuxParser::CPUStates::kIRQ_] + values[LinuxParser::CPUStates::kSoftIRQ_] + values[LinuxParser::CPUStates::kSteal_];
  }
  return 0;
 }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  auto jiffies = CpuUtilization();
  vector<long> values; for(auto &e:jiffies)values.push_back(std::stol(e)); 
  return values[LinuxParser::CPUStates::kIdle_] + values[LinuxParser::CPUStates::kIOwait_];
 }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(!stream.is_open())return {};
  string current_line, key;
  vector<string> values;
  while(std::getline(stream, current_line)){
    std::istringstream line_stream(current_line);
    line_stream >> key;
    if(key == "cpu"){
      string val;
      while(line_stream >> val)values.push_back(val);
      break;
    }
  }
  return values;
 }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int res = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(!stream.is_open())return 0;
  string line;
  while(std::getline(stream, line)){
    string key;
    std::istringstream line_stream(line);
    line_stream >> key;
    if(key == "processes"){line_stream >> res; return res;}
  }
  return res;
 }

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int res = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(!stream.is_open())return 0;
  string line;
  while(std::getline(stream, line)){
    string key;
    std::istringstream line_stream(line);
    line_stream >> key;
    if(key == "procs_running"){line_stream >> res; return res;}
  }
  return res;
 }

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string res;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(!stream.is_open())return "";
  std::getline(stream, res);
  return res;
 }

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string ram;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(!stream.is_open())return "";
  string str;
  while(std::getline(stream, str)){
    string key;
    std::istringstream line_stream(str);
    line_stream >> key;
    if(key == "VmSize:"){
      line_stream >> ram;
      try{
      int ram_kb = std::stoi(ram);
      return std::to_string(ram_kb/1024); // convert to MB
      }catch(...){return "0";}
    }
  }
  return "0";
 }

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string uid;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(!stream.is_open())return "";
  string str;
  while(std::getline(stream, str)){
    std::istringstream line_stream(str);
    string key; line_stream >> key;
    if(key == "Uid:"){line_stream >> uid; return uid;}
  }
  return uid;
 }

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string user_id = Uid(pid);
  std::ifstream stream(kPasswordPath);
  if(!stream.is_open())return "";
  string str;
  while(std::getline(stream, str)){
    std::replace(str.begin(), str.end(), ':', ' ');
    std::istringstream line_stream(str);
    string user, v2, id; line_stream >> user >> v2 >> id;
    if(id == user_id){return user;}
  }
  return "";
 }

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  vector<string>values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    string str, value; std::getline(stream, str);
    std::istringstream line_stream(str);
    while(line_stream >> value)values.push_back(value);
  }
  long result = 0;
  try{
    if(values.size() > 21) result = std::stol(values[21]) / sysconf(_SC_CLK_TCK);
  }catch(...){
    result = 0;
  }
  return result;
 }
