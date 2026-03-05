//============================================================================
// Name        	: IL3MetaMap.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jul 4, 2023 11:27:49 AM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3METAMAP_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3METAMAP_HPP_
#include <map>
#include <string>
namespace KTN {
namespace CME {
namespace IL3 {
namespace META {

class IL3MetaMap {
public:
    /**
     * @brief Default constructor for IL3MetaMap class.
     */
    IL3MetaMap();

    /**
     * @brief Initializes the IL3MetaMap object.
     */
    void init();//(const std::string& description, const std::string& semanticType,
        // std::size_t id, std::size_t blockLength);
    // : description(description), semanticType(semanticType), id(id), blockLength(blockLength) {}

    /**
     * @brief Struct to hold metadata information.
     */
    struct MetaData
    {
        std::string description;     /**< Description of the metadata. */
        std::string semanticType;    /**< Semantic type of the metadata. */
        std::size_t id;              /**< ID of the metadata. */
        std::size_t blockLength;     /**< Block length of the metadata. */
    };

private:
    // Define the static metadata map
    std::map<std::size_t, MetaData> metadataMap;

};


} // namespace META
} // namespace IL3
} // namespace CME
} // namespace KTN
#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3METAMAP_HPP_ */
