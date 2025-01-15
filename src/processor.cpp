#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    long prev_active = active_;
    long prev_idle = idle_;
    long prev_total = total_;
    active_ = LinuxParser::ActiveJiffies();
    idle_ = LinuxParser::IdleJiffies();
    total_ = LinuxParser::Jiffies();

    long delta_active = active_ - prev_active;
    long delta_total = total_ - prev_total;
    return delta_total != 0 ? static_cast<float>(delta_active) / delta_total : 0.0;
}