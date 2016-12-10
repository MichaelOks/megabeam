
#include <iostream>
#include "Imegabeamsystem.h"
#include "serverservice.h"
#include "clientservice.h"
#include "I_network_params.h"
#include "client_network_params.h"
#include "server_network_params.h"

using namespace std;

bool validate_user_input(int argc, char* argv[]){
	bool is_ok=true;
	if (argc < 2 || argc > 4) {
		is_ok=false;
	}else if(strcmp(argv[1], "server") == 0){
		is_ok=true;
	}else if (strcmp(argv[1], "worker") == 0 && (argc != 3)){
		is_ok=false;
	}else if(strcmp(argv[1], "worker") != 0 && strcmp(argv[1], "server") != 0){
		is_ok=false;
	}else if(atoi(argv[2]) > 65535 || atoi(argv[2]) < 1){
		is_ok=false;
	}
	return is_ok;
}

int main(int argc, char* argv[]) {
	Imegabeam_system *mysys;
        I_network_params *net_params;
        
	if(!validate_user_input(argc, argv)){
            cout<<"How to use:\n  megabeam_system server \n  megabeam_system worker [ip]"<<endl;
                return 0;
	}
	if(strcmp(argv[1], "server") == 0){
            cout<<"Starting server"<<endl;
                mysys = new server_service();
		net_params = new server_network_params();
		((server_network_params*)net_params)->workers_port = 6010;
		((server_network_params*)net_params)->clients_port = 6011;
	} else if (strcmp(argv[1], "worker") == 0){
            cout<<"Starting worker"<<endl;
                mysys = new client_service();
                net_params = new client_network_params();
                ((client_network_params*)net_params)->port = 6010;
                ((client_network_params*)net_params)->ip = argv[2];
	}

	if(!mysys){
		cout<<"Error in allocation"<<endl;
		return 0;
	}
        
	mysys->set_network_params(net_params);
	try{
            mysys->init_system();
            mysys->start_system();
	} catch(runtime_error &e){
		cout<<"[Error]:"<<e.what()<<endl;
	}
	mysys->stop_system();
	return 0;
}
