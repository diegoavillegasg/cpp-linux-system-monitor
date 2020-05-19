#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <experimental/filesystem>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  std::vector<int> currentPids, newPids{LinuxParser::Pids()};
  for (Process& p : processes_) {
    currentPids.push_back(p.Pid());
  }

  for (int pid: newPids) {
    if (!(std::find(currentPids.begin(), currentPids.end(), pid) != currentPids.end())) {
      // it's a new process
      
      Process newProcess(pid);
      processes_.emplace_back(newProcess);
    }
  }
  
  for (Process& p: processes_) {
       p.CpuUtilization(LinuxParser::ActiveJiffies(p.Pid()), 
                                LinuxParser::Jiffies());
  }

  std::sort(processes_.begin(), processes_.end(), std::greater<Process>());
  
  return processes_;
  
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }