/*
 * serverservice.h
 *
 *  Created on: Jul 12, 2016
 *      Author: michael
 */

#ifndef SERVERSERVICE_H_
#define SERVERSERVICE_H_

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <netinet/in.h>
#include <iostream>
#include "Imegabeamsystem.h"
#include <thread>
#include <map>
#include <mutex>
#include <exception>
#include "server_network_params.h"
#include "I_network_params.h"
#include <queue>
#include "tasks_db.h"


class server_service :public Imegabeam_system{
public:
	server_service();
	virtual ~server_service();
	void init_system();
	void set_network_params(I_network_params *net_params);
	void start_system();
	void stop_system();
	bool attach_client(int socket_num);

private:
	int p_socket_to_workers;
        int p_socket_to_clients;
        tasks_db DBtaskes;
         enum thread_type{
            eWorker,
            eClient
        };
	struct sockaddr_in cli_addr;
	const unsigned int max_connections;
        void client_task_collector(int sock);
	bool attach_worker(int socket_num);
        void listen_for_new_connections(int m_socket,thread_type eType);
        void init_socket(int *m_socket , int m_port);
        void close_socket(int m_socket);
        void add_free_worker_to_queue(int socket_num);
	server_network_params *p_network_params;
        void send_tasks_to_free_workers();
        void clean_task(vector<pair<char* , int>*> *task);
       
};
#endif /* SERVERSERVICE_H_ */