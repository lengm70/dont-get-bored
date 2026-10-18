#pragma once
#include <filesystem>
#include <map>
#include <string>
namespace app {
inline constexpr const char* progressRecordsPath = "config/progress.ini";
class ProgressRecords {
public:
    bool Set(const std::string& key,int value);
    int Get(const std::string& key) const;
    bool Best(const std::string& key,int value,bool lowerIsBetter=false);
    bool Load(const std::filesystem::path& path=progressRecordsPath);
    bool Save(const std::filesystem::path& path=progressRecordsPath);
    bool Dirty() const { return dirty_; }
private:
    std::map<std::string,int> values_;
    bool dirty_=false;
};
}
