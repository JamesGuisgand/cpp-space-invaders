#pragma once
#include <filesystem>
namespace inv {
class ScoreManager {
    std::filesystem::path file_;
    int best_{};
public:
    explicit ScoreManager(std::filesystem::path path);
    int best() const { return best_; }
    bool record(int score);
    static std::filesystem::path defaultPath();
};
}
