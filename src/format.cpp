#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::Format(int time) {
    string timeString = std::to_string(time);
    return string(2 - timeString.length(), '0') + timeString;
}

string Format::ElapsedTime(long seconds) {
    int hour, min, sec;
    hour = seconds / 3600;
    min = ( seconds % 3600 ) / 60;
    sec = seconds - hour * 3600 - min * 60 ;
    return Format(hour) + ":" + Format(min) + ":" + Format(sec); 
}