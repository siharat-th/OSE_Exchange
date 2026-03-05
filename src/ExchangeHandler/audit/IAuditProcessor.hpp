//============================================================================
// Name        	: IAuditProcessor.hpp
// Author      	: sgaer
// Version     	:
// Copyright   	: Copyright (C) 2025 Katana Financial
// Date			: Feb 3, 2023 1:50:50 PM
//============================================================================

#ifndef SRC_LOGGERS_IAUDITPROCESSOR_HPP_
#define SRC_LOGGERS_IAUDITPROCESSOR_HPP_

#include <string>
#include <cstdint>

namespace KTN{

class IAuditProcessor {
    public:
        virtual ~IAuditProcessor() {}
        virtual std::string process(char* buf, int templateId, uint64_t customerOrderTime = 0) = 0;
};

} // namespace KTN



    
#endif /* SRC_LOGGERS_IAUDITPROCESSOR_HPP_ */