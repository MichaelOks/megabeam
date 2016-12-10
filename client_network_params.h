

#ifndef CLIENT_NETWORK_PARAMS_H
#define CLIENT_NETWORK_PARAMS_H

#include "I_network_params.h"

class client_network_params: public I_network_params {
public:
	int port;
};

#endif /* CLIENT_NETWORK_PARAMS_H */

