/*
 * NoLock.hh
 *
 *  Created on: Jul 6, 2012
 *      Author: val
 */

#ifndef NOLOCK_HH_
#define NOLOCK_HH_

/**
 * Default no lock policy
 */
class NoLock {
public:
    NoLock() {}
    ~NoLock() {}
    inline int lock() {return 0;}
    inline int unlock() {return 0;}
    inline int tryLock() {return 0;}
};

#endif /* NOLOCK_HH_ */
