
#ifndef TASKS_SINGELTON_H
#define TASKS_SINGELTON_H
#include <queue>
#include <mutex>
#include <vector>
using namespace std;

class tasks_db
{
private:
    mutex locker;
    queue<vector<pair<char* , int>*>*> tasks;
    
public:
   void add_task(vector<pair<char* , int>*> *task){
       lock_guard<mutex> lock(locker);
       tasks.push(task);
   }
   
   vector<pair<char* , int>*>* get_task(){
       lock_guard<mutex> lock(locker);
       vector<pair<char* , int>*> *task = tasks.front();
       tasks.pop();
       return task;
   }
   
   bool is_empty(){
       lock_guard<mutex> lock(locker);
       return tasks.empty();
   }
};

#endif /* TASKS_SINGELTON_H */
