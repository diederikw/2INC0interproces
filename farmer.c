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
#include <mqueue.h>         // for mqr

#include "settings.h"
#include "output.h"
#include "common.h"

//GLOBAL VARIABLES
int totalSent = 0;
int totalReceived = 0;

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

void printLine(MQ_WORKER_RESPONSE* response){
	int i;
	for(i = 0; i < X_PIXEL; i++){
		output_draw_pixel(i, response->yReturn, response->color[i]);
		//printf("%d ", response->color[i]);
	}
	//printf("\n");
}

void emptyResponseQueue(){
	int numberMessagesEmptied = 0;
	struct mq_attr attr;
	int attrRet = mq_getattr(responseQueue, &attr);
	if(attrRet == -1){
	 	perror("mq_getattr failed");
	   	exit(1);
	}
	if(attr.mq_curmsgs == 0){
		return;
	}
	printf("Starting to empty queue: %d\n", (int)attr.mq_curmsgs);
	while(attr.mq_curmsgs > 0){
		MQ_WORKER_RESPONSE justReceived;
		int received = mq_receive(responseQueue, (char*)&justReceived, sizeof(MQ_WORKER_RESPONSE), (unsigned int *)NULL);
		if(received == -1){
			perror("An error occurred receiving a message\n");
			exit(1);
		}
		printLine(&justReceived);
		totalReceived++;
		numberMessagesEmptied++;
		printf("I just printed to the window from response number %d\n", totalReceived);
		int attrRet = mq_getattr(responseQueue, &attr);
		if(attrRet == -1){
		 	perror("mq_getattr failed");
		   	exit(1);
		}
		printf("Emptying queue : %d", (int)attr.mq_curmsgs);
	}
	return;
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
    totalSent = 0;
    totalReceived = 0;
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
   	int numberOfSwitches = -1;
    struct mq_attr attr;
    while(totalSent < Y_PIXEL  && totalReceived < Y_PIXEL){
    	int attrRet = mq_getattr(orderQueue, &attr);
    	    if(attrRet == -1){
    	    	perror("mq_getattr failed");
    	    	exit(1);
    	    }
    	numberOfSwitches++;
    	//printf("Switched %d times between sending and receiving\n", numberOfSwitches);
    	while(attr.mq_curmsgs < MQ_MAX_MESSAGES && totalSent < Y_PIXEL){
    		sendOrder.yCoord = totalSent;
    		int justSent = mq_send(orderQueue, (char*)&sendOrder, sizeof(MQ_FARMER_ORDER), 0);
    		if(justSent == -1){
    			perror("Sending an order failed");
    			exit(1);
    		}
    		totalSent++;
    		printf("I just sent an order number %d\n", totalSent);
    		int attrRet = mq_getattr(orderQueue, &attr);
    		if(attrRet == -1){
    			perror("mq_getattr failed");
    			exit(1);
    		}
    	}
    	emptyResponseQueue();

    }
    //End Farming
    //END_DIY
    output_end ();
    
    return (0);
}
