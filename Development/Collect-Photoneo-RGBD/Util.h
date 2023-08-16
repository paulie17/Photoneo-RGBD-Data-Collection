#pragma once
#include <algorithm>
#include <iostream>
#include <string>

namespace externalCamera {

namespace path {

inline std::string delimiter()
{
#ifdef __linux__
    return "/";
#endif
#ifdef _WIN32
    return "\\";
#endif
#ifdef _WIN64
    return "\\";
#endif
    return "/";
}

inline std::string join(const std::string& p1, const std::string& p2)
{
    return p1 + delimiter() + p2;
}

inline std::string dataFolder(const std::string& projectFolder)
{
    return join(projectFolder, "Data");
}

inline std::string settingsFolder(const std::string& projectFolder)
{
    return join(projectFolder, "Settings");
}

inline std::string calibrationFile(const std::string& projectFolder)
{
    return join(projectFolder, "calibration.txt");
}

std::string getProjectFolder(const std::string& executablePath);

bool readable(const std::string& path);

} // namespace path


template<typename Value>
struct AskEntry {
    Value value;
    std::string description;
};

/**
 * Print out multiple options and let the user select one of them.
 */
template <typename Value>
Value ask(
        const std::string& message,
        std::initializer_list<AskEntry<Value>> entries) {
    std::cout << message << std::endl;
    for (const auto& entry : entries) {
        std::cout << entry.value << "  " << entry.description << std::endl;
    }
    while (true) {
        Value value;
        if (std::cin >> value) {
            if (entries.end() != std::find_if(entries.begin(), entries.end(),
                    [&](const AskEntry<Value>& entry) {
                        return entry.value == value;
                    }))
            return value;
        }
    }
}

} // namespace externalCamera
