#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <experimental/filesystem>
#include "linux_parser.h"

// using namespace std;
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
  string chunk{""};
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> chunk)
      if (chunk == kVersionTag) {
        linestream >> chunk;
        return chunk;
      }
  }
  return chunk;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  const std::experimental::filesystem::path pathToProc{kProcDirectory};
  vector<int> pids{};
  if (std::experimental::filesystem::exists(pathToProc) && std::experimental::filesystem::is_directory(pathToProc)) {
    for (const auto& entry : std::experimental::filesystem::directory_iterator(pathToProc)) {
      auto filename = entry.path().filename();
      string aux = string(filename);
      if (std::experimental::filesystem::is_directory(entry.status())) {
        if (std::all_of(aux.begin(), aux.end(), [](char c){return std::isdigit(c);}))
          pids.push_back(std::stoi(aux));
      }
    }
  }
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key, value, kB;
  float total_mem{1}, free_mem{0}, buff_mem{0};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while(filestream >> key >> value >> kB) {
      if (key == kMemTotalTag) total_mem = stof(value);
      if (key == kMemFreeTag) free_mem = stof(value);
      if (key == kMemBuffersTag) buff_mem = stof(value);
      
    }
  }
  return (total_mem - free_mem) / (total_mem - buff_mem); 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line, totalUpTime;
  float tUpTime{0};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream stream(line);
    if (stream >> totalUpTime)
      tUpTime = stof(totalUpTime);
  }
  return tUpTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return UpTime() * sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string chunk, line;
  vector<string> values;
  long active{0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if( filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> chunk){
      //if (std::all_of(chunk.begin(), chunk.end(), [](char c){return std::isdigit(c);}))
      values.push_back(chunk);
    }
  }
  if (values.size() > 21) {
    active = stol(values[13]) + stol(values[14]) + stol(values[15]) + stol(values[16]); 
  }
  return active;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpuValues = CpuUtilization();
  long activeJifSys = stol(cpuValues[CPUStates::kUser_])    + stol(cpuValues[CPUStates::kNice_])
                    + stol(cpuValues[CPUStates::kGuest_])   + stol(cpuValues[CPUStates::kGuestNice_])
                    + stol(cpuValues[CPUStates::kSoftIRQ_])   + stol(cpuValues[CPUStates::kSteal_])
                    + stol(cpuValues[CPUStates::kSystem_])  + stol(cpuValues[CPUStates::kSteal_]);
  return activeJifSys;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpuValues = CpuUtilization();
  long idleJifSys = stol(cpuValues[CPUStates::kIdle_]) + stol(cpuValues[CPUStates::kIOwait_]);
  return idleJifSys;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string chunk, line;
  vector<string> values;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);  
      while (linestream >> chunk) {
        if (chunk == "cpu") {
          while(linestream >> chunk) {
            values.push_back(chunk);
          }
          return values;
        }
      }
    }
  }
  return values;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string chunk, line;
  int totalProcs{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);  
      if(linestream >> chunk) {
        if (chunk == "processes") {
          while(linestream >> chunk) {
            totalProcs = stoi(chunk);
          }
        }
      }
    }
  }
  return totalProcs; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string chunk, line;
  int runningProcs{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);  
      if(linestream >> chunk) {
        if (chunk == "procs_running") {
          while(linestream >> chunk) {
            runningProcs = stoi(chunk);
          }
        }
      }
    }
  }
  return runningProcs;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  string cmmd{""};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);  
    if(linestream >> cmmd) {
      return cmmd;
    }
  }
  return cmmd;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  //string ram{GetValueByKey<string>(kProcDirectory + to_string(pid) + kStatFilename, kVmSize)};
  string line, chunk;
  int ram;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatusFilename);
  if (filestream.is_open()) {
    while(filestream >> chunk) {
      if (chunk == kVmSize) {
        if (filestream >> chunk) {
          ram = stoi(chunk) / 1024;
          return to_string(ram);
        }
      }
    }
  }
  return "0";
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string chunk;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatusFilename);
  if (filestream.is_open()) {
    while (filestream >> chunk) {
      if (chunk == LinuxParser::kUid) {
        filestream >> chunk;
        return chunk;
      }
    }
  }
  return chunk;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  // get Uid
  string uid = Uid(pid);
  string line, chunk;
  string pattern{"x:"+uid};
  std::ifstream filestream(LinuxParser::kPasswordPath); 
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      auto mark = line.find(pattern);
      if (mark != string::npos)
        return line.substr(0, mark -1);
    }
  }
  return "0";
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long pidUpTime{0};
  string chunk, line;
  vector<string> values{};
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  if (filestream.is_open()) { 
    while(filestream >> chunk) {
      values.push_back(chunk);
      if (values.size() > 14)
        return stol(values[13]) / sysconf(_SC_CLK_TCK);
    }
  }
  return pidUpTime;
}

