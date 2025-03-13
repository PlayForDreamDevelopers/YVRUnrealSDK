#ifndef IBINDER_ISERVICECONNECTION_H
#define IBINDER_ISERVICECONNECTION_H

#include "IBinder.h"

class IServiceConnection {
public:
    virtual void onServiceConnected(IBinder *service) = 0;
    virtual void onServiceDisconnected() = 0;
};

typedef void (*OnServiceConnected)(IBinder *service);
typedef void (*OnServiceDisconnected)(void);

typedef struct {
    OnServiceConnected onServiceConnected;
    OnServiceDisconnected onServiceDisconnected;
} service_connection_t;


#endif //IBINDER_ISERVICECONNECTION_H
