#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float utilization{0};
  long activeJif = LinuxParser::ActiveJiffies();
  long idleJif = LinuxParser::IdleJiffies();
  
  prevActiveJiff_ = activeJif;
  prevIdleJiff_ = idleJif;
  // return ((activeJif + idleJif) / 4 ) / LinuxParser::UpTime();
}