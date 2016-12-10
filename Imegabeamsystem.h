
#ifndef IECHOSYSTEM_H
#define IECHOSYSTEM_H

#include <string>
#include "I_network_params.h"
using namespace std;

class Imegabeam_system {
public:
	virtual ~Imegabeam_system(){};
	virtual void init_system() =0;
	virtual void set_network_params(I_network_params *net_params) = 0;
	virtual void start_system() = 0;
	virtual void stop_system() = 0;
	Imegabeam_system(){};
};

#endif /* IECHOSYSTEM_H */

