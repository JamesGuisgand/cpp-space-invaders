#include "ScoreManager.hpp"
#include <cstdlib>
#include <fstream>
#include <utility>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
namespace inv {
namespace fs = std::filesystem;
fs::path ScoreManager::defaultPath() {
#ifdef _WIN32
    if (auto p = std::getenv("LOCALAPPDATA")) return fs::path(p)/"cpp-space-invaders/best.txt";
#else
    if (auto p = std::getenv("XDG_DATA_HOME"); p && *p) return fs::path(p)/"cpp-space-invaders/best.txt";
    if (auto p = std::getenv("HOME")) return fs::path(p)/".local/share/cpp-space-invaders/best.txt";
#endif
    return "best.txt";
}
ScoreManager::ScoreManager(fs::path path) : file_(std::move(path)) {
    std::ifstream in(file_);
    int value;
    char extra;
    if ((in >> value) && value >= 0 && !(in >> extra)) best_ = value;
}
bool ScoreManager::record(int score) {
    if (score <= best_) return true;
    std::error_code error;
    if (!file_.parent_path().empty()) fs::create_directories(file_.parent_path(),error);
    if (error) return false;
    auto tmp = file_;
    tmp += ".tmp";
    std::ofstream out(tmp);
    out << score << '\n';
    out.close();
    if (!out) return false;
#ifdef _WIN32
    if (!MoveFileExW(tmp.c_str(),file_.c_str(),MOVEFILE_REPLACE_EXISTING)) return false;
#else
    fs::rename(tmp,file_,error);
    if (error) return false;
#endif
    best_ = score;
    return true;
}
}
