#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include <sys/time.h>
#include <pthread.h>

#define TOUCHSCREENDEV "/dev/input/event2"
#define TOUCHSCREEN 1

typedef struct InputEvent{
	int iX;			/*�������İ��µ�X/Y��ַ*/
	int iY;
	int iType;
	int ikey;
	int iPressure;  /*0 - �ɿ���1 - ����*/
}T_InputEvent, *PT_InputEvent;

typedef struct InputOpr{
	char *name;
	pthread_t tTreadID;	/*���߳�ID*/
	int (*DeviceInit)(void);
	int (*DeviceExit)(void);
	int (*DevGetInputEvent)(PT_InputEvent ptInputEvent);
	struct InputOpr *ptNext;
}T_InputOpr, *PT_InputOpr;

int TSDevInit(void);
int TSGetInputEvent(PT_InputEvent ptInputEvent);



#endif

