#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include "Imegabeamsystem.h"
#include "client_network_params.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <exception>
#include "client_network_params.h"

class client_service : public Imegabeam_system{
public:
	client_service(){p_socket=0;};
	virtual ~client_service(){};
	void init_system();
	void set_network_params(I_network_params *net_params);
	void start_system();
	void stop_system();
private:
	int p_max_threads;
	int p_socket;
	struct sockaddr_in p_serv_addr;
        client_network_params *p_network_params;
        void send_to_server_a_message(const char *data, int size);
        void client_engine();
        void rx_listener(int socket);
};


#endif /* CLIENTSERVICE_H */