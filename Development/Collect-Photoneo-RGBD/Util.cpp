#include "Util.h"

#include <fstream>
#if defined(_WIN32)
#include <windows.h>
#elif defined (__linux__)
#include <unistd.h>
#endif

namespace externalCamera {

namespace path {

std::string getProjectFolder(const std::string& executablePath) {
    std::string projectFolder;
    projectFolder =
            executablePath.substr(0, executablePath.find_last_of(delimiter()));
    projectFolder =
            projectFolder.substr(0, projectFolder.find_last_of(delimiter()));
#ifdef __linux__
    if (projectFolder.size() <= 3) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            projectFolder = cwd;
    }
#endif
    std::cout << "Project directory identified as: " << projectFolder
            << std::endl;
    return projectFolder;
}

bool readable(const std::string& path) {
    std::ifstream stream(path);
    return stream.good();
}

} // namespace path

} // namespace externalCamera
