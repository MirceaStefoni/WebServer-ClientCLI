#include <common/protocol.h>
#include <algorithm>
#include <sstream>

namespace Protocol {
    
    Method parseMethod(const std::string& methodStr) {
        std::string upperMethod = methodStr;
        std::transform(upperMethod.begin(), upperMethod.end(), upperMethod.begin(), ::toupper);
        
        if (upperMethod == "GET") {
            return Method::GET;
        } else if (upperMethod == "POST") {
            return Method::POST;
        } else {
            return Method::UNKNOWN;
        }
    }
    
    std::string methodToString(Method method) {
        switch (method) {
            case Method::GET:
                return "GET";
            case Method::POST:
                return "POST";
            default:
                return "UNKNOWN";
        }
    }
    
    std::string formatRequest(Method method, const std::string& path, const std::string& payload) {
        std::ostringstream request;
        request << methodToString(method) << " " << path;
        
        if (method == Method::POST && !payload.empty()) {
            request << " " << payload;
        }
        
        return request.str();
    }
    
} 