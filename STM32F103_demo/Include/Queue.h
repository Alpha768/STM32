
#ifndef  __QUEUE_H__
#define  __QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- */
/* ------------------- simple ring buffer implementation ------------------- */
/* ------------------------------------------------------------------------- */


typedef struct tagRingBuffer{
	  U32   size;
    U32   readIndex;
    U32   writeIndex;
    U8    *buffer;
	  BOOLEAN Qinit;
}RingBuffer_t;


void QueueClear(RingBuffer_t *rb);
U32 QueueDataCount(RingBuffer_t *rb);
U32 QueueFreeCount(RingBuffer_t *rb);
BOOLEAN  QueueWrite(RingBuffer_t *rb,U8 *buffer,U32 nBytes);
BOOLEAN  QueueRead(RingBuffer_t *rb,U8 *buffer,U32 nBytes);
BOOLEAN QueueLook(RingBuffer_t *rb,U8 *buffer,U32 nBytes);

#ifdef __cplusplus
};
#endif

#endif
