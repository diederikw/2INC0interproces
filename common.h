/* 
 * Operating Systems   (2INCO)   Practical Assignment
 * Interprocess Communication
 *
 * Contains definitions which are commonly used by the farmer and the workers
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include "settings.h"


// TODO: put your definitions of the datastructures here
#define TEAM_NAME "MFAVIER_DDEWIT"


static char mq_orders[80];
static char mq_response[80];

mqd_t orderQueue;
mqd_t responseQueue;

typedef struct {
	int yCoord;
} MQ_FARMER_ORDER;

typedef struct {
	int yReturn;
	int color[X_PIXEL];
} MQ_WORKER_RESPONSE;


#endif

