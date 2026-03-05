#ifndef DYNAMICARRAY_HH_
#define DYNAMICARRAY_HH_

#include "limetypes.hh"

#include <map>
#include <cstdint>
#include <limits>



namespace LimeBrokerage {

    template <class T>
    class DynamicArray {
    public:

        DynamicArray(uint32_t, uint32_t){        
        }   
        
        T & operator[] (uint32_t index) {
            return map_[index];
        }

        std::uint32_t capacity() const{return std::numeric_limits<std::uint32_t>::max();}
    private:
        std::map<std::uint32_t, T> map_;
    };

    typedef DynamicArray<std::string> StringDynamicArray;
}
#endif /*DYNAMICARRAY_HH_*/
