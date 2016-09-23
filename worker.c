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
#include <errno.h>          // for perror()
#include <unistd.h>         // for getpid()
#include <mqueue.h>         // for mq-stuff
#include <time.h>           // for time()
#include <complex.h>

#include "settings.h"
#include "common.h"

static void rsleep (int t);

static double 
complex_dist (complex a)
{
    // distance of vector 'a'
    // (in fact the square of the distance is computed...)
    double re, im;
    
    re = __real__ a;
    im = __imag__ a;
    return (re * re + im * im);
}

static int 
mandelbrot_point (double x, double y)
{
    int     k;
    complex z;
	complex c;
    
	z = x + y * I;     // create a complex number 'z' from 'x' and 'y'
	c = z;

	for (k = 0; k < MAX_ITER && complex_dist(z) < INFINITY; k++)
	{
	    z = z * z + c;
    }
    
    //                                    2
    // k >= MAX_ITER or | z | >= INFINITY
    
    return (k);
}

//TODO: Delete. Ik wilde het nog niet verwijderen voordat de code 100% klaar is.
int mandelbrotToPixel(double mandelbrotX){
	return (mandelbrotX - X_LOWERLEFT)/STEP + 1;
}

//Convert a X- or Y- pixel-coordinate to a X- or Y- Mandelbrot coordinate
double pixelToMandelbrot(int Pixel, double lowerLeft){
	return (double)(lowerLeft+((Pixel)*STEP));
}

int main (int argc, char * argv[])
{
    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the two message queues (whose names are provided in the arguments)
    //  * repeatingly:
    //      - read from a message queue the new job to do
    //      - wait a random amount of time (e.g. rsleep(10000);)
    //      - do that job (use mandelbrot_point() if you like)
    //      - write the results to a message queue
    //    until there are no more jobs to do
    //  * close the message queues
    
	//Open both message queues
	sprintf(mq_orders, "/order_queue_%s_%d", TEAM_NAME, (int)getppid());
	sprintf(mq_response, "/response_queue_%s_%d", TEAM_NAME, (int)getppid());
	orderQueue = mq_open(mq_orders, O_RDONLY);
	responseQueue = mq_open(mq_response, O_WRONLY);
	//end mq

	MQ_FARMER_ORDER newOrder;
	MQ_WORKER_RESPONSE reply;
	while(1){
		//Check if an order has been received. If this failed, print an error and call the exit method.
		int received = mq_receive(orderQueue, (char*)&newOrder, sizeof(MQ_FARMER_ORDER), (unsigned int *)NULL);
		if(received == -1){
			perror("An error occurred receiving a message\n");
			exit(1);
		}

		//Wait for a random time between 0 and 10.000 microseconds)
		rsleep(10000);

		//Copy the Y-pixel-coordinate to the reply. Initialize the x-pixel-coordinate variable.
		reply.yReturn = newOrder.yCoord;
		int xLoop;

		//For all the pixels at pixel height yCoord, calculate the color using the mandelbrot_point() method.
		for(xLoop = 0; xLoop < X_PIXEL; xLoop++){
			reply.color[xLoop] = mandelbrot_point(pixelToMandelbrot(xLoop, X_LOWERLEFT), pixelToMandelbrot(newOrder.yCoord, Y_LOWERLEFT));
		}

		//Send the colors for the whole horizontal line at pixel yCoord to the Response queue.
		//If this failed, print an error and call the exit method.
		int justSent = mq_send(responseQueue, (char*)&reply, sizeof(MQ_WORKER_RESPONSE), (unsigned int)NULL);
		if(justSent == -1){
			perror("An error occurred responding");
			exit(1);
		}
	}
}

/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time
 * between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;
    
    if (first_call == true)
    {
        srandom (time (NULL) % getpid ());
        first_call = false;
    }
    usleep (random() % t);
}


