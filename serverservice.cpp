#include "serverservice.h"
#include <fstream>
#include <iterator>
#include <algorithm>
#include <string>
#include <thread>
#include "tasks_db.h"
#include "workers_queue_singelton.h"
#include <vector>
static map<int,thread> client_threads_map;
mutex mtx;

/*
 *   constractor , just to init sockets
 *   Input: none
 *   returns: none 
 */
server_service::server_service() :max_connections(1000) {
	p_socket_to_clients=0;
        p_socket_to_workers=0;        
}

/*
 *   distractor, closing all sockets
 *   Input: none
 *   returns: none 
 */
server_service::~server_service() {
	close(p_socket_to_clients);
        close(p_socket_to_workers);
}

/*
 *   setting network params
 *   Input: net_params
 *   returns: none 
 */
void server_service::set_network_params(I_network_params *net_params){
	p_network_params = dynamic_cast<server_network_params*>(net_params);
	if(!p_network_params){
		 throw std::invalid_argument( "cant do dynamic cast to server network params" );
        }
}

/*
 *   starting system, 3 threads are started.
 *   thr_worker is handling all workers
 *   thr_client is handling all clients
 *   thr_task_manager is sending tasks to each worker
 *   Input: none
 *   returns: none 
 */
void server_service::start_system(){
    cout<<"system started"<<endl;
    thread thr_workers(&server_service::listen_for_new_connections ,this, p_socket_to_workers, eWorker);
    thread thr_clients(&server_service::listen_for_new_connections ,this, p_socket_to_clients, eClient);
    thread thr_task_manager(&server_service::send_tasks_to_free_workers,this);
    thr_workers.join();
    thr_clients.join(); 
    thr_task_manager.join();
}

/*
 *   closing all sockets
 *   Input: none
 *   returns: none 
 */
void server_service::stop_system(){
    close_socket(p_socket_to_clients);
    close_socket(p_socket_to_workers);
}

/*
 *   closing socket
 *   Input: socket number
 *   returns: none 
 */
void server_service::close_socket(int m_socket){
	//close main socket
	close(m_socket);
	//close all connected sockets
	for(map<int,thread>::iterator it=client_threads_map.begin(); it!=client_threads_map.end(); ++it){
		close(it->first);
		client_threads_map.erase(it->first);
	}    
}

/*
 *   disconnect from a client
 *   Input: none
 *   returns: none 
 */
void disconnect_from_a_client(int sock){
	close(sock);
	client_threads_map.erase(sock);
	cout<<"A client is disconnected"<<endl;
}

/*
 *   receiving from a socket tasks, and saving them into the queue
 *   Input: socket number
 *   returns: none 
 */
void server_service::client_task_collector(int sock){
	int n;
	bool run=true;
	char buffer[256];
   
	bzero(buffer,256);	
        vector<pair<char* , int>*> *memo = new vector<pair<char* , int>*>();
	while(run){
		n = read(sock,buffer,255);
		if(n <= 0 ){ 
			if(n < 0){
				cout<<"Error reading from socket"<<endl;
			}
			disconnect_from_a_client(sock);
			run=false;
		} else {           
                    char *s1 = new char [n];
                    memcpy(s1, buffer,n);
                    memo->push_back(new pair<char* , int>(s1,n));
		}
	}
        DBtaskes.add_task(memo);
}

/*
 *   adding a worker to a waiting job queue
 *   Input: socket number
 *   returns: none 
 */
void server_service::add_free_worker_to_queue(int socket_num){
   (workers_queue_singelton().instance())->push(socket_num);
  
   	int n;
	bool run=true;
	char buffer[256];
	bzero(buffer,256);
         
	while(run){
		n = read(socket_num,buffer,255);
		if(n <= 0 ){ 
			if(n < 0){
				cout<<"Error reading from socket"<<endl;
			}
			disconnect_from_a_client(socket_num);
			run=false;
		} else {
                    int res = memcmp(buffer,"finished_task" , sizeof("finished_task"));
                    if(res == 0){
                        cout<<"adding again a worker to a queue"<<endl;
                        (workers_queue_singelton().instance())->push(socket_num);
                    }
                }
	}
}

/*
 *   attach worker
 *   Input: socket number
 *   returns: none 
 */
bool server_service::attach_worker(int socket_num){
    cout<<"attached worker"<<endl;
	if(client_threads_map.size() < max_connections){
		client_threads_map[socket_num] = thread(&server_service::add_free_worker_to_queue,this,socket_num);
		client_threads_map[socket_num].detach();
		cout<<"A new worker is connected"<<" socket:"<<socket_num<<endl;
		return true;
	}
        else{
            cout<<"too much workers connections "<<endl;
            return false;
        }
}

/*
 *   attach client
 *   Input: socket number
 *   returns: none 
 */
bool server_service::attach_client(int socket_num){
        cout<<"attached client"<<endl;
        if(client_threads_map.size() < max_connections){
		client_threads_map[socket_num] = thread(&server_service::client_task_collector,this,socket_num);
		client_threads_map[socket_num].detach();
		cout<<"A new client is connected"<<" socket:"<<socket_num<<endl;
		return true;
	} else{
            cout<<"too much clients connections"<<endl;
            return false;
        }
}

/*
 *   listen for new connections
 *   Input: socket number , type - client/worker
 *   returns: none 
 */
void server_service::listen_for_new_connections(int m_socket , thread_type eType){
	int new_socket;
	socklen_t socket_len;
        
	listen(m_socket,max_connections);
	socket_len = sizeof(cli_addr);
        while (1) {
		new_socket = accept(m_socket, (struct sockaddr *) &cli_addr, &socket_len);
                if (new_socket >= 0) {
                    if(eType == eClient){
			if(!attach_client(new_socket)){
				cout<<"To much open connections, only "<< max_connections <<" allowed."<<endl;
				close(new_socket);
			}
                    } else {
                        //This is worker type
                        if(!attach_worker(new_socket)){
				cout<<"To much open connections, only "<< max_connections <<" allowed."<<endl;
				close(new_socket);
			}
                    }
		} else {
			close(new_socket);
		}
	}
}

/*
 *   init workers and clients sockets
 *   Input: none
 *   returns: none 
 */
void server_service::init_system(){
    init_socket(&p_socket_to_workers,p_network_params->workers_port);
    init_socket(&p_socket_to_clients,p_network_params->clients_port);
}

/*
 *   bind socket with port and ip
 *   Input: socket and port number
 *   returns: none 
 */
void server_service::init_socket(int *m_socket , int m_port){
    struct sockaddr_in serv_addr;

	*m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (*m_socket < 0) {
		throw runtime_error("Error opening socket");
	}
	// Initialize socket
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(m_port);
	// Bind the host address
	if (bind(*m_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		throw runtime_error("There is an error in binding, try other port");
	}        
}

/*
 *   free space by deleting task
 *   Input: task
 *   returns: none 
 */
void server_service::clean_task(vector<pair<char* , int>*> *task){
      vector<pair<char* , int>*>::iterator vectorIter;
      for(vectorIter=task->begin(); vectorIter!=task->end();vectorIter++){
          delete (*vectorIter)->first;
          delete *vectorIter;
      }
     task->clear();
     delete task;
}

/*
 *   It is sending each time that there is are tasks and a free workers
 *   Input: none
 *   returns: none 
 */
void server_service::send_tasks_to_free_workers(){
    int n=0,worker=0;
    vector<pair<char* , int>*> *task;
    vector<pair<char* , int>*>::iterator vectorIter;
    
    while(1){
        while(!(workers_queue_singelton().instance()->empty() || DBtaskes.is_empty())){
            worker = workers_queue_singelton().instance()->front();
            workers_queue_singelton().instance()->pop();
            task = DBtaskes.get_task();
            cout<<"size of task:" << task->size()<<endl;
            cout<<"sending to socket"<<worker<<endl;
            
            for(vectorIter=task->begin(); vectorIter!=task->end();vectorIter++){
                pair<char* , int> *data = *vectorIter;
                n = write(worker,(void *)data->first,data->second);
                if (n < 0) {
                    cout<<"there is an error with this client!!"<<endl;
                }
            }
            clean_task(task);
        }
        this_thread::yield();
    }
  }

