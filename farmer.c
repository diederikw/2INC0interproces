/* 
 * Operating Systems   (2INCO)   Practical Assignment
 * Interprocess Communication
 *
 * STUDENT_NAME_1 (STUDENT_NR_1)
 * STUDENT_NAME_2 (STUDENT_NR_2)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8. 
 * ”Extra” steps can lead to higher marks because we want students to take the initiative. 
 * Extra steps can be, for example, in the form of measurements added to your code, a formal 
 * analysis of deadlock freeness etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>    
#include <unistd.h>         // for execlp
#include <mqueue.h>         // for mq

#include "settings.h"
#include "output.h"
#include "common.h"


void openMessageQueue(){
			sprintf(mq_orders, "/order_queue_%s_%d", TEAM_NAME, (int)getpid());
			sprintf(mq_response, "/response_queue_%s_%d", TEAM_NAME, (int)getpid());
			struct mq_attr attr;
	    	attr.mq_maxmsg = MQ_MAX_MESSAGES;
	    	attr.mq_msgsize = sizeof(MQ_FARMER_ORDER);
	    	orderQueue = mq_open(mq_orders, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);
	    	if(orderQueue == -1){
	    		perror("Opening the order queue failed");
	    		exit(1);
	    	}
	    	attr.mq_maxmsg = MQ_MAX_MESSAGES;
	    	attr.mq_msgsize = sizeof(MQ_WORKER_RESPONSE);
	    	responseQueue = mq_open(mq_response, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);
	    	if(responseQueue == -1){
	    		perror("Opening the response queue failed");
	    		exit(1);
	    	}
}


int main (int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }
        
    output_init();

    // TODO:
    //  * create the message queues (see message_queue_test() in interprocess_basic.c)
    //  * create the child processes (see process_test() and message_queue_test())
    //  * do the farming (use output_draw_pixel() for the coloring)
    //  * wait until the children have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues contain your
    // student name and the process id (to ensure uniqueness during testing)
    
    //DIY
    //MQ initialization
    	openMessageQueue();
    //END MQ
    pid_t worker_pids[NROF_WORKERS];
    int i;
    for(i = 0; i < NROF_WORKERS; i++){
    	worker_pids[i] = fork();
    	if (worker_pids[i] < 0){
    		perror("An error occurred trying to create a new process");
    		exit(1);
    	} else if (worker_pids[i] == 0){
    		execl("./worker","./worker",NULL);
    	} else {

    	}
    }
    //Farming
    MQ_FARMER_ORDER sendOrder;
    sendOrder.xCoord = i;
    sendOrder.yCoord = i;
    int justSent = mq_send(orderQueue, (char*)&sendOrder, sizeof(MQ_FARMER_ORDER), 0);
    if(justSent == -1){
    	perror("Sending an order failed");
   	}
    waitpid(-1,NULL,0);
    //End Farming
    //END_DIY
    output_end ();
    
    return (0);
}
