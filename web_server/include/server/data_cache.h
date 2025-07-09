#ifndef DATA_CACHE_H
#define DATA_CACHE_H

#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>

class DataCache {
public:
    DataCache();
    ~DataCache();
    
    // Add data to cache (thread-safe)
    void addData(const std::string& data);
    
    // Get all cached data (thread-safe)
    std::vector<std::string> getData() const;
    
    // Get cache size (thread-safe)
    size_t size() const;
    
    // Clear all cached data (thread-safe)
    void clear();
    
private:
    std::vector<std::string> data_;
    mutable std::shared_mutex mutex_;
};

#endif // DATA_CACHE_H 