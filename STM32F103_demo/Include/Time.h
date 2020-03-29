#ifndef  __TIME_H__
#define  __TIME_H__

#ifdef __cplusplus
extern "C" {
#endif
	
extern void Delay1Ms(U32 ms);
extern void Delay10Ms(U32 m10s);
extern void Delay1Us(U32 Us);
extern void Timer10MsCallBack(void);

#define TIMER_START 0x01
#define TIMER_USED   0x02
#define TIMER_OUT   0x04
#define TIMER_REPEAT 0x08
#define TIMER_POLLING 0x10

#define TIMER_MAX 25

typedef void (*CallBackPtr)(void);

typedef struct tagTimerQueue{
	U8 Flags;
	U32 Interval;
	U32 TimerCnt;
    CallBackPtr TimerCallBack;
}TimerQueue;

void QueueTimerInit(void);
U8 QueueTimerCreate(U32 Interval,BOOLEAN bStart,BOOLEAN bRepeat,BOOLEAN bPolling,CallBackPtr TimerCallBack);
U8 QueueTimerDelete(U8 Index);
U8 QueueTimerStop(U8 Index);
U8 QueueTimerStart(U8 Index);
U8 QueueTimerFlag(U8 Index,BOOLEAN bRepeat,BOOLEAN bPolling);
U8 QueueTimerInterval(U8 Index,U32 Interval);
U8 QueueTimerRestore(U8 Index);
void QueueTimerTick(U8 TickMs);
void QueueTimerPolling(void);

#ifdef __cplusplus
};
#endif

#endif
