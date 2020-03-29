
#include "Include.h"
/* ------------------------------------------------------------------------- */

#define RING_BUFFER_SIZE  rb->size


void QueueClear(RingBuffer_t *rb)
{
	if(rb==NULL)
		return ;
	rb->readIndex=0;
	rb->writeIndex=0;
}

U32 QueueDataCount(RingBuffer_t *rb)
{
	U32 Cnt;
	
    if(rb->writeIndex >= rb->readIndex)
		Cnt=rb->writeIndex - rb->readIndex;
	else
		Cnt=RING_BUFFER_SIZE+rb->writeIndex - rb->readIndex;

    return Cnt;
}

U32 QueueFreeCount(RingBuffer_t *rb)
{
	U32   FreeCnt ;
	if(rb->writeIndex >= rb->readIndex)
        FreeCnt=RING_BUFFER_SIZE-rb->writeIndex + rb->readIndex;
	else
        FreeCnt=rb->readIndex-rb->writeIndex;
	return FreeCnt;
}

BOOLEAN  QueueWrite(RingBuffer_t *rb,U8 *buffer,U32 nBytes)
{
	U32   newWriteIndex,i,FreeCnt;
	//-----------------------------

	if(rb==NULL || buffer==NULL || nBytes ==0)
		return FALSE;

	if(rb->Qinit)
	{
		rb->readIndex=0;
		rb->writeIndex=0;
		rb->Qinit=FALSE;
	}
	
    if(rb->writeIndex >= rb->readIndex)
        FreeCnt=RING_BUFFER_SIZE-rb->writeIndex + rb->readIndex;
	else
        FreeCnt=rb->readIndex-rb->writeIndex;
    
	if(FreeCnt<nBytes)
        return FALSE;
     
	//-----------------------------
    for(i=0;i<nBytes;i++)
	{
		newWriteIndex = rb->writeIndex + 1;
		if(newWriteIndex >= RING_BUFFER_SIZE)
			newWriteIndex = 0;
		rb->buffer[rb->writeIndex] = buffer[i];
		rb->writeIndex = newWriteIndex;
	}
	return TRUE;
}

BOOLEAN QueueRead(RingBuffer_t *rb,U8 *buffer,U32 nBytes)
{
	U32 i,Cnt;
	//-----------------------------------------

	if(rb==NULL || buffer==NULL || nBytes ==0)
		return FALSE;

	if(rb->Qinit)
	{
		rb->readIndex=0;
		rb->writeIndex=0;
		rb->Qinit=FALSE;
		return FALSE;
	}

    if(rb->writeIndex < rb->readIndex)
        Cnt=RING_BUFFER_SIZE+rb->writeIndex - rb->readIndex;
		else
		Cnt = rb->writeIndex - rb->readIndex;

	if(Cnt<nBytes)
		return FALSE;
	//-----------------------------------------
	for(i=0;i<nBytes;i++)
	{
		buffer[i] = rb->buffer[rb->readIndex];
		rb->readIndex++;
		if(rb->readIndex >= RING_BUFFER_SIZE)
			rb->readIndex = 0;
	}
    return TRUE;
}

BOOLEAN QueueLook(RingBuffer_t *rb,U8 *buffer,U32 nBytes)
{
	U32 i,Cnt,readIndex;
	//-----------------------------------------

	if(rb==NULL || buffer==NULL || nBytes ==0)
		return FALSE;

	if(rb->Qinit)
	{
		rb->readIndex=0;
		rb->writeIndex=0;
		rb->Qinit=FALSE;
		return FALSE;
	}

    if(rb->writeIndex < rb->readIndex)
        Cnt=RING_BUFFER_SIZE+rb->writeIndex - rb->readIndex;
		else
		Cnt = rb->writeIndex - rb->readIndex;

	if(Cnt<nBytes)
		return FALSE;
	//-----------------------------------------
	readIndex=rb->readIndex;
	for(i=0;i<nBytes;i++)
	{
		buffer[i] = rb->buffer[readIndex];
		readIndex++;
		if(readIndex >= RING_BUFFER_SIZE)
			readIndex = 0;
	}
    return TRUE;
}
