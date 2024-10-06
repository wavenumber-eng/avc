#include "e_queue.h"

#ifndef CONFIG__BQ_MAX_PRINTF_LEN
	#define CONFIG__BQ_MAX_PRINTF_LEN 128
#endif

void bq__clear(byte_queue_t *bq)
{
    bq->read_ptr=0;
    bq->write_ptr=0;
}

void bq__init(byte_queue_t *bq,uint32_t Size,uint8_t * Storage)
{
    uint16_t i;

    bq->size = Size;
    bq->read_ptr=0;
    bq->write_ptr=0;
    bq->storage = Storage;

    for (i=0;i<bq->size;i++)
    {
        bq->storage[i] = 0;
    }
}

uint32_t bq__bytes_available_to_write(byte_queue_t *bq)
{
	return ((bq->size) - bq__bytes_available_to_read(bq));
}

uint32_t bq__bytes_available_to_read(byte_queue_t *bq)
{
    if (bq->read_ptr > bq->write_ptr)
    {
        return (bq->size - bq->read_ptr + bq->write_ptr);
    }
    else if (bq->write_ptr > bq->read_ptr)
    {
        return     (bq->write_ptr - bq->read_ptr);
    }
    else
    {
        return 0;
    }
}


queue_result_e bq__enqueue(byte_queue_t *bq,uint8_t val)
{
    if (bq__bytes_available_to_write(bq) == 0)
    {
        return QUEUE_FULL;
    }
    else
    {
        bq->storage[bq->write_ptr] = val;
        bq->write_ptr++;

        if (bq->write_ptr >= bq->size) {
            bq->write_ptr = 0;
        }

        if(bq->enqueue_hook!=NULL)
        {
        	bq->enqueue_hook(bq->hook_arg);
        }
        return QUEUE_OK;
    }
}

int32_t bq__enqueue_array(byte_queue_t *bq,uint8_t *buf,uint32_t len)
{
    uint32_t i;
    for (i=0;i<len;i++)
    {
        bq__enqueue(bq,buf[i]);
    }
    return len;
}


queue_result_e bq__printf(byte_queue_t *bq, const char *format_string,...)
{

	 uint8_t bq_string_buffer[CONFIG__BQ_MAX_PRINTF_LEN+1];
     va_list argptr;
     va_start(argptr,format_string);
     vsnprintf((char *)bq_string_buffer,CONFIG__BQ_MAX_PRINTF_LEN,format_string,argptr);
     va_end(argptr);

     return bq__enqueue_array(bq,(uint8_t *)bq_string_buffer,strnlen((const char *)bq_string_buffer,CONFIG__BQ_MAX_PRINTF_LEN));
}


queue_result_e bq__dequeue(byte_queue_t *bq,uint8_t *val)
{
    if (bq__bytes_available_to_read(bq) == 0)
    {
        return QUEUE_EMPTY;
    }
    else
    {
        *val  = bq->storage[bq->read_ptr];

        bq->read_ptr++;

        if (bq->read_ptr >= bq->size) {
            bq->read_ptr = 0;
        }
        return QUEUE_OK;
    }
}


uint32_t bq__dequeue_array(byte_queue_t *bq,uint8_t *val,uint32_t len)
{
	uint32_t count;

	count = bq__bytes_available_to_read(bq);

	if(len<count)
	{
		count = len;
	}

    for(int i=0;i<count;i++)
    {
        val[i] = bq__dequeue_next(bq);
    }

	return count;
}

uint8_t bq__dequeue_next(byte_queue_t *bq)
{
    uint8_t retval;

    if (bq__bytes_available_to_read(bq) == 0)
    {
        return 0;
    }
    else
    {
        retval  = bq->storage[bq->read_ptr];

        bq->read_ptr++;

        if (bq->read_ptr >= bq->size)
        {
            bq->read_ptr = 0;
        }
        return retval;
    }
}
