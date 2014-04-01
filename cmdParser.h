#ifdef _MSC_VER
#pragma once
#endif

#ifndef CMDPARSER_H
#define CMDPARSER_H

char* getCmdOption(char ** begin, char ** end, const std::string & option) {
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return nullptr;
}

char* getFileName(int argc, char** argv) {
    if (argc < 2) {
        return nullptr;
    }
    return argv[argc - 1];
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

#endif // CMDPARSER_H
