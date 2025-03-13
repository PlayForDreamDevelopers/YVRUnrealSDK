#ifndef IBINDER_IBINDER_H
#define IBINDER_IBINDER_H

#define LOG_TAG "W-BINDER"

#include <string>


class IBinder {
public:
    virtual void start();
    virtual void stop();
    virtual std::string getName();
};


#endif //IBINDER_IBINDER_H
