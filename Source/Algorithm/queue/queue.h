#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

#define ARR_SIZE 64U // user define

typedef struct queue_st
{
	volatile uint8_t buf[ARR_SIZE];
	uint8_t front;
	uint8_t rear;
	bool overrun;
	bool isFull;
	bool isEmpty;
} queue_t;

void queue_init(queue_t* queue_ptr);
void queue_enqueue(queue_t* queue_ptr, uint8_t* str_ptr, uint8_t numToEnqueue);
uint8_t queue_dequeue(queue_t* queue_ptr, uint8_t* str_ptr, uint8_t numToDequeue);
uint8_t queue_get_size(queue_t* queue_ptr);
void queue_clear_overrun(queue_t* queue_ptr);

#endif
