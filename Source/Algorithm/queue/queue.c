#include <string.h>
#include "queue.h"

void queue_init(queue_t* queue_ptr)
{
    memset(queue_ptr, 0, sizeof(queue_t));
    queue_ptr->isEmpty = true;
    queue_ptr->isFull = false;
}

void queue_enqueue(queue_t* queue_ptr, uint8_t* str_ptr, uint8_t numToEnqueue)
{
    for (uint8_t index = 0; index < numToEnqueue; index ++)
    {
        if (queue_ptr->isFull)
        {
            queue_ptr->overrun = true;
            queue_ptr->front = (queue_ptr->front + 1) % ARR_SIZE;
        }

        if (!queue_ptr->isEmpty)
        {
            queue_ptr->rear = (queue_ptr->rear + 1) % ARR_SIZE;
        }
        else
        {
            queue_ptr->isEmpty = false;
        }

        queue_ptr->buf[queue_ptr->rear] = str_ptr[index];

        if (((queue_ptr->rear + 1) % ARR_SIZE) == queue_ptr->front)
        {
            queue_ptr->isFull = true;
        }
    }
}

uint8_t queue_dequeue(queue_t* queue_ptr, uint8_t* str_ptr, uint8_t numToDequeue)
{
    if (queue_ptr->isEmpty || numToDequeue == 0)
    {
        return 0;
    }

    uint8_t index = 0;
    while(index < numToDequeue && !queue_ptr->isEmpty)
    {
        str_ptr[index] = queue_ptr->buf[queue_ptr->front];
        queue_ptr->isFull = false;

        if (queue_ptr->front == queue_ptr->rear)
        {
            queue_ptr->isEmpty = true;
            queue_ptr->front = 0;
            queue_ptr->rear  = 0;
        }
        else
        {
            queue_ptr->front = (queue_ptr->front + 1) % ARR_SIZE;
        }

        index++;
    }

    return index;
}

uint8_t queue_get_size(queue_t* queue_ptr)
{
    if (queue_ptr->isEmpty)
    {
        return 0;
    }

    if (queue_ptr->isFull)
    {
        return ARR_SIZE;
    }

    if (queue_ptr->rear > queue_ptr->front)
    {
        return queue_ptr->rear - queue_ptr->front + 1;
    }

    // Wraparound case
    return (ARR_SIZE - queue_ptr->front) + queue_ptr->rear + 1;
}

void queue_clear_overrun(queue_t* queue_ptr)
{
    queue_ptr->overrun = false;
}
