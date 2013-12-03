// ======================================================================== //
// Copyright 2009-2013 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "taskscheduler_mic.h"
#include "sys/sync/mutex.h"


namespace embree
{
  __align(64) AtomicMutex mtx;

  //#define DBG(x) { mtx.lock(); x ; mtx.unlock(); }
  #define DBG(x) 

  TaskSchedulerMIC::TaskSchedulerMIC()     
  {
    for (size_t i=0; i<NUM_TASKS; i++) tasks[i] = NULL;
    head_task_list = 0;
  }

  /*! waits for an event out of a task */
  void TaskSchedulerMIC::wait(size_t threadIndex, size_t threadCount, Event* event)
  {
    DBG(
	PING;
	DBG_PRINT(threadIndex);
	DBG_PRINT(threadCount);
	);

  }

    /*! processes next task */
  void TaskSchedulerMIC::work(size_t threadIndex, size_t threadCount, Task *task)
  { 
    DBG(std::cout << "START WORK task " << task << " threadIndex " << threadIndex << std::endl << std::flush);
    
    /* take next task from task list */
    TaskScheduler::Event* event = task->event;
    thread2event[threadIndex] = event; 

    DBG(
	std::cout << "GOT TASK " << (void*)task << " : threadIndex " << threadIndex << " threadCount " << threadCount << std::endl << std::flush;
	);

    while (true) 
      {
	if (unlikely((int)task->started < 0)) break;       
	int elt = --task->started;
	if (unlikely(elt < 0)) break;       

	if (task->run) task->run(task->runData,threadIndex,threadCount,elt,task->elts,task->event);
      }

    barrier.wait(threadIndex,threadCount);

    DBG(std::cout << "END WORK task " << task << " threadIndex " << threadIndex << std::endl << std::flush);
  }


  void TaskSchedulerMIC::add(ssize_t threadIndex, QUEUE queue, Task* task)
  {
    DBG( PING; std::cout << "add task " << task << " threadIndex " << threadIndex << std::endl << std::flush );

    if (task->elts == 1)
      {
	int elt = --task->started;
	if (task->run) task->run(task->runData,0,1,elt,task->elts,task->event);

	if (task->complete) 
	  task->complete(task->completeData,0,1,task->event);

      }
    else
      {

	__memory_barrier();
	unsigned int liveIndex = (head_task_list++)&(NUM_TASKS-1);
	__memory_barrier();
	if (tasks[liveIndex]) throw std::runtime_error("task list full");
	__memory_barrier();
	assert(tasks[liveIndex] == NULL); 
	tasks[liveIndex] = task;

	DBG( std::cout << "ADD TASK: head " << head_task_list << " liveIndex " << liveIndex << " threadIndex " << threadIndex << std::endl );

	DBG( std::cout << "WAIT APP THREAD..." << std::endl << std::flush);
	__memory_barrier();
	while (tasks[liveIndex] != NULL) { 
	  __pause(1023); 
	}
	DBG( std::cout << "WAIT APP THREAD...DONE" << std::endl << std::flush);
	

      }

  }

  void TaskSchedulerMIC::run(size_t threadIndex, size_t threadCount)
  {
    DBG( std::cout<< "run function for thread " << threadIndex << " " << std::endl << std::flush);

    unsigned int tail_task_list = 0;
    while(1) 
      {
	DBG( std::cout<< "ENTERING WAIT STATE " << threadIndex << " head_task_list " <<  head_task_list << " tail_task_list " << tail_task_list << std::endl << std::flush);

	/* wait for available task */
	while(head_task_list == tail_task_list && !terminateThreads)
	  {
	    __pause(1023); 
	  }

	DBG( std::cout<< "WOKE " << threadIndex << std::endl);

	unsigned int task_index = tail_task_list & (NUM_TASKS-1);
    	
	Task *task = tasks[task_index];
	if (task == NULL)
	  {
	    DBG(std::cout << "thread activated but no task found = threadIndex " << threadIndex << std::endl << std::flush);
	  }
	else
	  {
	    DBG(std::cout << "thread work = threadIndex " << threadIndex << std::endl << std::flush);

	    work(threadIndex,threadCount,task);

	    if (threadIndex == 0)
	      {
		DBG(
		    std::cout << "COMPLETE threadIndex " << threadIndex << std::endl << std::flush;
		    );


		DBG(
		    std::cout << "COMPLETE FCT START threadIndex " << threadIndex << std::endl << std::flush;
		    );

		if (task->complete) 
		  task->complete(task->completeData,threadIndex,threadCount,task->event);

		DBG(
		    std::cout << "COMPLETE FCT DONE threadIndex " << threadIndex << std::endl << std::flush;
		    );

		//if (task->event) task->event->dec();


		DBG(
		    std::cout << "EVENT DONE threadIndex " << threadIndex << std::endl << std::flush;
		    );

		__memory_barrier();
		tasks[task_index] = NULL;
		__memory_barrier();


	      }

	    __memory_barrier();	    
	    tail_task_list++;
	  }

	/* terminate thread */
	if (terminateThreads) 
	  {
	    return;
	  }

      } 

  }

  void TaskSchedulerMIC::terminate() {
    terminateThreads = true;
  }
}
