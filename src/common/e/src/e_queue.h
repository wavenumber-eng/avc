#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "string.h"

#ifndef _E_QUEUE_H_
#define _E_QUEUE_H_

typedef void (*bq_hook_t)(void * arg);

typedef struct {

    uint32_t read_ptr;
    uint32_t write_ptr;
    uint32_t size;
    uint8_t *storage;
    void *hook_arg;
    bq_hook_t enqueue_hook;

} byte_queue_t;

typedef enum
{
	 QUEUE_FULL    =   -1,
	 QUEUE_EMPTY   =   -2,
	 QUEUE_OK      =    0,
} queue_result_e;

void 	           bq__init(byte_queue_t *bq,uint32_t size,uint8_t * storage);
uint32_t		   bq__bytes_available_to_write(byte_queue_t *bq);
uint32_t 		   bq__bytes_available_to_read(byte_queue_t *bq);
void	           bq__clear(byte_queue_t *bq);
queue_result_e 	   bq__enqueue(byte_queue_t *bq,uint8_t Val);
int32_t		 	   bq__enqueue_array(byte_queue_t *bq,uint8_t *Buf,uint32_t len);
queue_result_e 	   bq__dequeue(byte_queue_t *bq,uint8_t *Val);
uint8_t 		   bq__dequeue_next(byte_queue_t *bq);

/*
 *  Returns the actual amount removed if there was less than len available.
 */
uint32_t		   bq__dequeue_array(byte_queue_t *bq,uint8_t *val,uint32_t len);

queue_result_e     bq__printf(byte_queue_t *bq, const char *format_string,...);



#define BYTE_QUEUE__MAKE(name,length) uint8_t bq__##name##__storage[length];\
									  byte_queue_t name = { \
									 .read_ptr=0,\
									 .write_ptr=0,\
								     .size = length,	\
								     .storage = bq__##name##__storage\
								      }

#endif /* QUEUE_H_ */
