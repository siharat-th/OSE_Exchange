/*
 * Singleton.hpp
 *
 *  Created on: Feb 5, 2017
 *      Author: sgaer
 */

#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#pragma once

#include <KT01/Core/Macro.hpp>

//namespace LWT { namespace core {

/// Singleton class template.
/// Relies on C++11 thread-safety of static initialization.
template <typename T>
class Singleton {
protected:
    Singleton() {}
    Singleton(const Singleton&) = delete;

public:
    static T& instance() {
        static T instance_;
        return instance_;
    }
};

//} }



#endif /* SINGLETON_HPP_ */
