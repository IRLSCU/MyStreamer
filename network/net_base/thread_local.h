// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef BASE_THREADLOCAL_H
#define BASE_THREADLOCAL_H

#include "base_mutex.h"
#include "uncopyable.h"

#include <pthread.h>

NAMESPACE_START

template <typename T>
class ThreadLocal : noncopyable
{
public:
    ThreadLocal()
    {
        MCHECK(pthread_key_create(&pkey_, &ThreadLocal::destructor));
    }

    ~ThreadLocal()
    {
        MCHECK(pthread_key_delete(pkey_));
    }

    T &value()
    {
        T *perThreadValue = static_cast<T *>(pthread_getspecific(pkey_));
        if (!perThreadValue)
        {
            T *newObj = new T();
            MCHECK(pthread_setspecific(pkey_, newObj));
            perThreadValue = newObj;
        }
        return *perThreadValue;
    }

private:
    static void destructor(void *x)
    {
        T *obj = static_cast<T *>(x);
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
        T_must_be_complete_type dummy;
        (void)dummy;
        delete obj;
    }

private:
    pthread_key_t pkey_;
};

NAMESPACE_END

#endif // MUDUO_BASE_THREADLOCAL_H
