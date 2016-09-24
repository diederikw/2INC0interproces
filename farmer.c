/* 
 * Operating Systems   (2INCO)   Practical Assignment
 * Interprocess Communication
 *
 * Diederik de Wit (0829667)
 * Michiel Favier (0951737)
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

//Open both the Orders and the Response Queue.
void openMessageQueue(){
	sprintf(mq_orders, "/order_queue_%s_%d", TEAM_NAME, (int)getpid());
	sprintf(mq_response, "/response_queue_%s_%d", TEAM_NAME, (int)getpid());

	//Save the desired attributes for the Order message queue in "attr".
	struct mq_attr attr;
	attr.mq_maxmsg = MQ_MAX_MESSAGES;
	attr.mq_msgsize = sizeof(MQ_FARMER_ORDER);

	//Open the Order message queue with the attributes "attr".
	//If the Order message queue could not be opened (a value of -1 was returned) an error will be printed and the exit method will be executed.
	orderQueue = mq_open(mq_orders, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);
	if(orderQueue == -1){
		perror("Opening the order queue failed");
	    exit(1);
	}

	//Save the desired attributes for the Response message queue in "attr".
	attr.mq_msgsize = sizeof(MQ_WORKER_RESPONSE);

	//Open the Response message queue with the attributes "attr".
	//If the Response message queue could not be opened (a value of -1 was returned) an error will be printed and the exit method will be executed.
	responseQueue = mq_open(mq_response, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);
	if(responseQueue == -1){
		perror("Opening the response queue failed");
		exit(1);
	}
}

//Print all the pixels in a single-pixel horizontal line
void printLine(MQ_WORKER_RESPONSE* response){
	int i;
	for(i = 0; i < X_PIXEL; i++){
		output_draw_pixel(i, response->yReturn, response->color[i]);
	}
}

//Empty the Response queue
void emptyResponseQueue(){

	struct mq_attr attr;
	int attrRet = mq_getattr(responseQueue, &attr);
	//If the attributes and status of the Response queue could not be retrieved, and error is shown and the exit method is called.
	if(attrRet == -1){
		perror("mq_getattr failed");
		exit(1);
	}

	//If there are currently no messages in the Response queue, return.
	if(attr.mq_curmsgs == 0){
		return;
	}

	//The Response queue will be emptied while the message queue is not empty and the total amount of messages received is smaller than the total amount of jobs.
	while(attr.mq_curmsgs > 0 && totalReceived < Y_PIXEL){
		//Retrieve a message from the Response queue
		MQ_WORKER_RESPONSE justReceived;
		int received = mq_receive(responseQueue, (char*)&justReceived, sizeof(MQ_WORKER_RESPONSE), (unsigned int *)NULL);

		//If no message could be retrieved from the Response queue, a error will be printed and the exit method called.
		if(received == -1){
			perror("An error occurred receiving a message\n");
			exit(1);
		}

		//The print method is called with the message which has just been retrieved from the Response queue.
		printLine(&justReceived);

		//Count the total amount of messages received and the amount of messages received during a single call of this method.
		totalReceived++;

		attrRet = mq_getattr(responseQueue, &attr);
		if(attrRet == -1){
		 	perror("mq_getattr failed");
		   	exit(1);
		}
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

    //  * create the message queues (see message_queue_test() in interprocess_basic.c)
    //  * create the child processes (see process_test() and message_queue_test())
    //  * do the farming (use output_draw_pixel() for the coloring)
    //  * wait until the children have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues contain your
    // student name and the process id (to ensure uniqueness during testing)
    

    //Set the counter for the total amount of messages received and sent to 0.
    totalSent = 0;
    totalReceived = 0;

    //MQ initialization
    openMessageQueue();
    //END MQ

    //Create workers, according to the maximum amount of workers allowed in settings.h
    pid_t worker_pids[NROF_WORKERS];
    int i;
    for(i = 0; i < NROF_WORKERS; i++){
    	worker_pids[i] = fork();

    	//Execute a worker in every fork
    	if (worker_pids[i] < 0){
    		perror("An error occurred trying to create a new process");
    		exit(1);
    	} else if (worker_pids[i] == 0){
    		execl("./worker","./worker",NULL);
    	}
    }

    //Farming
   	MQ_FARMER_ORDER sendOrder;

    struct mq_attr attr;

    //While the total amount of messages sent is less than the total amount of jobs, send more orders.
    while(totalSent < Y_PIXEL){
    	//Retrieve the attributes of the Order queue.
    	int attrRet = mq_getattr(orderQueue, &attr);
    	//If retrieving the attributes of the order queue failed, print a message and call the exit method.
    	if(attrRet == -1){
    	   	perror("mq_getattr failed");
    	   	exit(1);
    	}

    	//While the Order queue has not reached its limit and the total amount of messages sent is less than the total amount of jobs, send another order.
    	while(attr.mq_curmsgs < MQ_MAX_MESSAGES && totalSent < Y_PIXEL){
    		//Initialize the Y-value for the order.
    		sendOrder.yCoord = totalSent;

    		//Send the Order message to the Order queue.
    		//If this failed, print an error and call the exit method.
    		int justSent = mq_send(orderQueue, (char*)&sendOrder, sizeof(MQ_FARMER_ORDER), 0);
    		if(justSent == -1){
    			perror("Sending an order failed");
    			exit(1);
    		}

    		//Count the total amount of orders send
    		totalSent++;
    		//Get the attributes of the Order queue.
    		//If this failed, print an error and call the exit method.
    		int attrRet = mq_getattr(orderQueue, &attr);
    		if(attrRet == -1){
    			perror("mq_getattr failed");
    			exit(1);
    		}
    	}
    	//After the Order queue has been filled, empty the Response queue.
    	emptyResponseQueue();
    }

    /*After all the Orders have been sent and while the total amount of messages received from the Response queue is less than the total amount of jobs;
    empty the Response queue.*/
    while(totalReceived < Y_PIXEL){
    	emptyResponseQueue();
    }
    //stop all children and close message queues
    for(i = 0; i < NROF_WORKERS; i++){
    	sendOrder.yCoord = -1;
    	int justSent = mq_send(orderQueue, (char*)&sendOrder, sizeof(MQ_FARMER_ORDER), 0);
    	if(justSent == -1){
    		perror("Sending an order failed");
    		exit(1);
    	}
    }

    for(i = 0; i < NROF_WORKERS; i++){
    	waitpid(worker_pids[i], NULL, -1);
    }
    mq_close(orderQueue);
    mq_close(responseQueue);
    int unlinking = mq_unlink(mq_orders);
    if(unlinking == -1){
    	perror("Unlinking the order queue failed");
    }
    unlinking = mq_unlink(mq_response);
    if(unlinking == -1){
    	perror("Unlinking the response queue failed");
    }

    //End Farming
    output_end ();
    
    return (0);
}
