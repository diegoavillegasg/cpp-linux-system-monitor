#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float utilization{0};
  long activeJif = LinuxParser::ActiveJiffies();
  long idleJif = LinuxParser::IdleJiffies();
  utilization = static_cast<float>((activeJif - prevActiveJiff_)) / ((activeJif - prevActiveJiff_) + (idleJif - prevIdleJiff_));
  prevActiveJiff_ = activeJif;
  prevIdleJiff_ = idleJif;
  return utilization;
}