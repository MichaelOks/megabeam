

#ifndef WORKERS_QUEUE_SINGELTON_H
#define WORKERS_QUEUE_SINGELTON_H

#include <queue>
using namespace std;

class workers_queue_singelton
{
public:
   static queue<int>* instance()
   {
      static queue<int> workers_queue;
      return &workers_queue;
   }
};

#endif /* WORKERS_QUEUE_SINGELTON_H */

