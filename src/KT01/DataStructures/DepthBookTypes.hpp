#ifndef KT01_DATASTRUCTURES_MARKETDEPTH_DEPTHBOOKTYPES_HPP
#define KT01_DATASTRUCTURES_MARKETDEPTH_DEPTHBOOKTYPES_HPP


#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdint>
#include <unordered_map>

#include <tbb/concurrent_vector.h>

#include <akl/Price.hpp>
using namespace akl;

#include <KT01/DataStructures/DepthBook.hpp>

using namespace std;

namespace KT01 {
namespace DataStructures {
namespace MarketDepth {

// Define a compile-time flag
//#define USE_TBB  // Comment this out to switch to std::vector

#ifdef USE_TBB
    using DepthBookContainer = tbb::concurrent_vector<DepthBook>;
#else
    using DepthBookContainer = std::vector<DepthBook>;
#endif

   

} // namespace MktData
} // namespace DataStructures
} // namespace KTN

#endif