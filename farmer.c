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
    	sprintf(mq_orders, "/order_queue_MFAVIER_%d", (int)getpid());
    	sprintf(mq_response, "/response_queue_MFAVIER_%d", (int)getpid());
    	struct mq_attr attr;
    	attr.mq_maxmsg = MQ_MAX_MESSAGES;
    	attr.mq_msgsize = sizeof(MQ_FARMER_ORDER);
    	mq_open(mq_orders, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);
    	attr.mq_msgsize = sizeof(MQ_WORKER_RESPONSE);
    	mq_open(mq_response, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);
    //END MQ
    pid_t worker_pids[NROF_WORKERS];
    int i;
    for(i = 0; i < NROF_WORKERS; i++){
    	worker_pids[i] = fork();
    	if (worker_pids[i] < 0){
    		printf("An error occurred trying to create a new process\n");
    	} else if (worker_pids[i] == 0){
    		execl("./worker","./worker",NULL);
    	} else {
    		MQ_FARMER_ORDER sendOrder;
    		sendOrder.xCoord = i;
    		sendOrder.yCoord = i;
    		mq_send(mq_orders, (char*)&sendOrder, sizeof(MQ_FARMER_ORDER), 0);
    		waitpid(-1,NULL,0);
    	}
    }
    //END_DIY
    output_end ();
    
    return (0);
}

