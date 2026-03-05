//============================================================================
// Name        	: FormatterBase.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 21, 2023 3:21:03 PM
//============================================================================

#ifndef SRC_JSON_FORMATTERBASE_HPP_
#define SRC_JSON_FORMATTERBASE_HPP_
#pragma once

#pragma once
#include <cstring>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

/**
 * @class FormatterBase
 * @brief Base class for JSON formatters.
 */
class FormatterBase {
public:
	/**
  * @brief Destructor.
  */
	virtual ~FormatterBase() {};

	/**
  * @brief Get the JSON representation of the given data.
  * @param data The data to be converted to JSON.
  * @return The JSON representation of the data.
  */
	virtual std::string GetJson(const void* data) const = 0;
};

#endif /* SRC_JSON_FORMATTERBASE_HPP_ */
