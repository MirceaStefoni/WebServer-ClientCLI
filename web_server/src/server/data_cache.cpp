#include <server/data_cache.h>
#include <common/logger.h>

DataCache::DataCache() {
    Logger::logMessage("DataCache initialized");
}

DataCache::~DataCache() {
    Logger::logMessage("DataCache destroyed");
}

void DataCache::addData(const std::string& data) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    data_.push_back(data);
    Logger::logMessage("Data added to cache: " + data + " (total entries: " + std::to_string(data_.size()) + ")");
}

std::vector<std::string> DataCache::getData() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return data_;
}

size_t DataCache::size() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return data_.size();
}

void DataCache::clear() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    size_t prev_size = data_.size();
    data_.clear();
    Logger::logMessage("Cache cleared, removed " + std::to_string(prev_size) + " entries");
} 