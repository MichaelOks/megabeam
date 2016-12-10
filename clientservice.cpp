#include "clientservice.h"
#include <sys/sysinfo.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <fstream>

/*
 *   Init socket with port and ip address
 *   Input: none
 *   returns: none 
 */
void client_service::init_system(){
    
        struct hostent *server;
	this->p_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->p_socket < 0) {
		throw runtime_error("Error while opening the socket");
	}
	server = gethostbyname(p_network_params->ip);
	if (server == NULL) {
		throw runtime_error("Error no such host");
	}
	bzero((char *) &this->p_serv_addr, sizeof(this->p_serv_addr));
	this->p_serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&this->p_serv_addr.sin_addr.s_addr, server->h_length);
	this->p_serv_addr.sin_port = htons(p_network_params->port);
}


/*
 *   saving network params into this object
 *   Input: net_params - port and address
 *   returns: none
 */
void client_service::set_network_params(I_network_params *net_params){
	p_network_params = dynamic_cast<client_network_params*>(net_params);        
	if(!p_network_params){
            throw std::invalid_argument( "cant do dynamic cast to client network params" );
        }
}

/*
 *   sending a message to the server
 *   Input: data - a const char , size - size of the char
 *   returns: none
 */
void client_service::send_to_server_a_message(const char *data , int size){
		int n = write(this->p_socket, data, size);
		if(n <= 0 ){
			close(this->p_socket);
		}
}


/*
 *   saving received program program to a file 
 *   Input: program
 *   returns: none
 */
void save_program_as_a_file(vector<pair<char* , int>*> *program){
    ofstream OutFile;
    vector<pair<char* , int>*>::iterator vectorIter;
    OutFile.open("program_to_run", ios::out);
    
    for(vectorIter=program->begin(); vectorIter!=program->end();vectorIter++){
            pair<char* , int> *data = *vectorIter;
            OutFile.write(data->first , data->second );   
      }
    OutFile.close();
}
 
void clean_task(vector<pair<char* , int>*> *task){
      vector<pair<char* , int>*>::iterator vectorIter;
      for(vectorIter=task->begin(); vectorIter!=task->end();vectorIter++){
          delete (*vectorIter)->first;
          delete *vectorIter;
      }
     task->clear();
}

/*
 *   listening and executing tasks from a socket.
 *   Input: socket number
 *   returns: none
 */
void client_service::rx_listener(int socket){
	char buffer[256];
	int n;
	bool is_running=true;
        vector<pair<char* , int>*> task ;

	while(is_running){
		// read server response
		bzero(buffer,256);
		n = read(socket, &buffer, 255);
		if (n <= 0) {
			cout<<"Cannot connect to the server"<<endl;
			is_running=false;
		}
                char *s1 = new char [n];
                memcpy(s1, buffer,n);
                task.push_back(new pair<char* , int>(s1,n));
                if( n < 255){    
                    save_program_as_a_file(&task);
                    system("chmod +x ./program_to_run");
                    system("./program_to_run");
                    clean_task(&task);
                    string finished = "finished_task";
                    send_to_server_a_message(finished.c_str(),finished.size());
                }
	}
        cout<<"finished handling file"<<endl;
}


/*
 *   start system to run
 *   Input: none
 *   returns: none
 */
void client_service::start_system(){
	//Connect to the server by creating rx and tx threads.
	if (connect(this->p_socket, (struct sockaddr*)&this->p_serv_addr, sizeof(this->p_serv_addr)) < 0) {
		throw runtime_error("Cant connecting to the server");
	} else {
		thread rx_thread (&client_service::rx_listener,this,this->p_socket);
		rx_thread.join();
	}
}

/*
 *   saving network params into this object
 *   Input: net_params - port and address
 *   returns: none
 */
void client_service::stop_system(){
	close(this->p_socket);
}