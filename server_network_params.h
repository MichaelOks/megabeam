
#ifndef SERVER_NETWORK_PARAMS_H
#define SERVER_NETWORK_PARAMS_H


#include "I_network_params.h"
#include "serverservice.h"

class server_network_params :public I_network_params {
public:
	int clients_port;
	int workers_port;
        server_network_params(){};
        ~server_network_params(){};
};

#endif /* SERVER_NETWORK_PARAMS_H */