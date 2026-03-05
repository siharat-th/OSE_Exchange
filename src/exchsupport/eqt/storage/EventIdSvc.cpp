#include <exchsupport/eqt/storage/EventIdSvc.hpp>
#include <filesystem>
#include <fcntl.h>

EventIdSvc::EventIdSvc(string dir, string filename) 
	: _DIR(dir), _FILENAME(filename)
{
	CreateAndRun("EventIdSvc", 2);
}

EventIdSvc::~EventIdSvc() {
	_ACTIVE.store(false, std::memory_order_relaxed);
}

void EventIdSvc::Persist(uint64_t eventId) {
	_MSGS.push(eventId);
}

void EventIdSvc::Run() {
    std::string filepath = _DIR + "/" + _FILENAME;
    // create directory if not exists
    std::filesystem::create_directories(std::filesystem::path(filepath).parent_path());

    // Open without truncating so we preserve the file content
    int fd = open(filepath.c_str(), O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return;
    }    
    
    _ACTIVE.store(true, std::memory_order_relaxed);    
    while (_ACTIVE.load(std::memory_order_relaxed)) {
        uint64_t eventId;
        uint64_t latestEventId = 0;
        bool hasEvent = false;
        
        // Extract the latest eventId from the queue
        while (_MSGS.try_pop(eventId)) {
            latestEventId = eventId;
            hasEvent = true;
        }
          // Write only the latest value
        if (hasEvent) {
            // Go back to the beginning of the file to overwrite existing data
            lseek(fd, 0, SEEK_SET);
            std::string data = std::to_string(latestEventId);
            ssize_t bytesWritten = write(fd, data.c_str(), data.size());
            
            if (bytesWritten > 0) {
                // Truncate the file to match the new data size only if write was successful
                ftruncate(fd, data.size());
                fsync(fd);
            }
        }
        
        // Small delay to reduce CPU usage
        usleep(10000); // 10ms
    }

    close(fd);
}