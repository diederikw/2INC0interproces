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
static char mq_orders[80];
static char mq_response[80];

typedef struct {

} MQ_FARMER_ORDER;

typedef struct {

} MQ_WORKER_RESPONSE;

#endif

