#ifndef IBINDER_PROXYSERVICE_H
#define IBINDER_PROXYSERVICE_H

#include <string>
#include <binder/IBinder.h>
#include <binder/IServiceConnection.h>


class ProxyService {
public:
    static ProxyService &instance() {
        static ProxyService pInstance;
        return pInstance;
    }
    ~ProxyService();
    void start();
    void stop();
    void bindService(std::string name, IServiceConnection *connection);
    void bindService(std::string name, service_connection_t *connection);
    IBinder *getService(std::string name);

private:
    ProxyService();
};

#endif //IBINDER_PROXYSERVICE_H
