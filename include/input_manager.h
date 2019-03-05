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
/**********************************************************************
 * �������ƣ� InpuOprtRegister
 * ���������� ע���¼�����ģ��
 * ��������� ptInputOpr - �¼�����ģ��ṹ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int InputOprtRegister(PT_InputOpr ptInputOpr);

/**********************************************************************
 * �������ƣ� ShowInpuOprt
 * ���������� ע���¼�����ģ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void ShowInpuOprt(void);


/**********************************************************************
 * �������ƣ� InputDevicesInit
 * ���������� ��������"����ģ��"���豸��صĳ�ʼ������
 *            ���������ڶ�ȡ�������ݵ����߳�,��main����ʱ���г�ʼ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int AllInputOprInit(void);

/**********************************************************************
 * �������ƣ� AllInputInit
 * ���������� ���ø�������ģ��ĳ�ʼ������,����ע���������ģ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int AllInputRegister(void);

/**********************************************************************
 * �������ƣ� GetInputEvent (���̻߳�������¼��ĺ���)
 * ���������� �����������,����ʹ�õ�ǰ�߳�����,
 *            ��������ģ������̶߳������ݺ���������
 * ��������� ��
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� 0 - �ɹ�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int GetInputEvent(PT_InputEvent ptInputEvent);


#endif

