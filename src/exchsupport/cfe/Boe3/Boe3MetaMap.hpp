#ifndef BOE3METAMAP_HPP_
#define BOE3METAMAP_HPP_

#include <string>
#include <map>
#include <cstddef>

namespace KTN {
namespace CFE {
namespace Boe3 {
namespace Meta {

class BOE3MetaMap {
public:
    /**
     * @brief Default constructor for BOE3MetaMap class.
     */
    BOE3MetaMap();

    /**
     * @brief Initializes the BOE3MetaMap object with CFE message metadata.
     */
    void init();

    /**
     * @brief Struct to hold metadata information.
     */
    struct MetaData {
        std::string description;     /**< Description of the metadata. */
        std::string semanticType;    /**< Semantic type of the metadata. */
        std::size_t id;              /**< ID of the metadata. */
        std::size_t blockLength;     /**< Block length of the metadata (always 0 for BOE3). */
    };

private:
    // Define the static metadata map
    std::map<std::size_t, MetaData> metadataMap;
};

} // namespace Meta
} // namespace Boe3
} // namespace CFE
} // namespace KTN

#endif /* BOE3METAMAP_HPP_ */
