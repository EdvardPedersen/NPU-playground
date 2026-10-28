// Whole-APU package power from the amdgpu PPT sensor (shared by the bench tools).
#pragma once
#include <filesystem>
#include <fstream>
#include <string>

inline double apu_power_w() {
    static std::string path = [] {
        for (auto &e : std::filesystem::directory_iterator("/sys/class/hwmon")) {
            std::ifstream n(e.path() / "name");
            std::string name;
            if (n >> name && name == "amdgpu") return (e.path() / "power1_input").string();
        }
        return std::string();
    }();
    std::ifstream f(path);
    double uw = 0;
    f >> uw;
    return uw / 1e6;
}
