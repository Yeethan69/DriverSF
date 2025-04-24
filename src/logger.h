//
// Created by ethan on 23/04/2025.
//
#pragma once
#ifndef LOGGER_H
#define LOGGER_H
#include <string>

namespace logger {
    // Vars
    inline bool log_dll = true;
    inline bool log_lua = true;

    // Functions
    void setup_console();
    void dll(const char* context, const std::string& message);
    void lua(const std::string& message);

} // logger

#endif //LOGGER_H
