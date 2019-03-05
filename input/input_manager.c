#include <stdio.h>
#include <pthread.h>

#include <input_manager.h>
#include <config.h>

static PT_InputOpr g_ptInputOprHead;
static T_InputEvent g_tInputEvent;

static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;  //������
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;	   //��������

/**********************************************************************
 * �������ƣ� InpuOprtRegister
 * ���������� ע���¼�����ģ��
 * ��������� ptInputOpr - �¼�����ģ��ṹ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int InputOprtRegister(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTempOpr = g_ptInputOprHead;
	
	if(NULL == g_ptInputOprHead)
	{
		g_ptInputOprHead = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	else
	{
		ptTempOpr = g_ptInputOprHead;
		while (ptTempOpr->ptNext)
		{
			ptTempOpr = ptTempOpr->ptNext;
			
		}
		ptTempOpr->ptNext = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	return 0;	
}

/**********************************************************************
 * �������ƣ� ShowInpuOprt
 * ���������� ע���¼�����ģ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void ShowInpuOprt(void)
{
	int iNumber = 0;
	PT_InputOpr ptTempOpr = g_ptInputOprHead;
	while (ptTempOpr->ptNext)
	{
		debug("InputOPr: %2d:%s.\n", iNumber++, ptTempOpr->name);
		ptTempOpr = ptTempOpr->ptNext;
	}
}
	
/**********************************************************************
 * �������ƣ� InputEventFunc  (�̺߳���)
 * ���������� "����ģ��"���̺߳���,
 *		  ÿ������ģ�鶼��ͨ���������߳�����ȡ��������, �������ݺ����ỽ�ѵȵ����ݵ������߳�
 * ��������� pVoid - ����ģ���"���������ݺ���"
 * ��������� ��
 * �� �� ֵ�� NULL - �����˳�
 ***********************************************************************/
static void *InputEventFunc(void *pVoid)
{
	T_InputEvent tInputEvent;
	
	/* ���庯��ָ�� */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	GetInputEvent = (int (*)(PT_InputEvent))pVoid;

	while (1)
	{
		if(0 == GetInputEvent(&tInputEvent))   /*GetInputEvent ������TSGetInputEvent()����*/
		{
			/* �������߳�, ��tInputEvent��ֵ����һ��ȫ�ֱ��� */
			/* �����ٽ���Դǰ���Ȼ�û����� */
			pthread_mutex_lock(&g_tMutex);
			g_tInputEvent = tInputEvent;			/*����õ������¼������Ϣ��ֵ��ȫ���¼�������*/

			/*	�������߳� */
			pthread_cond_signal(&g_tConVar);

			/* �ͷŻ����� */
			pthread_mutex_unlock(&g_tMutex);
		}
	}

	return NULL;
}

/**********************************************************************
 * �������ƣ� AllInputOprInit
 * ���������� ��������"����ģ��"���豸��صĳ�ʼ������
 *            ���������ڶ�ȡ�������ݵ����߳�,��main����ʱ���г�ʼ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int AllInputOprInit(void)
{
	PT_InputOpr ptTmp = g_ptInputOprHead;
	int iError = -1;

	while (ptTmp)
	{
		if (0 == ptTmp->DeviceInit())		//�������豸������/��������
		{
			/* �������߳� */
			pthread_create(&ptTmp->tTreadID, NULL, InputEventFunc, ptTmp->DevGetInputEvent);			
			iError = 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return iError;
}


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
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	/* ���� */
	pthread_mutex_lock(&g_tMutex);
	pthread_cond_wait(&g_tConVar, &g_tMutex);	

	/* �����Ѻ�,�������� */
	*ptInputEvent = g_tInputEvent;			/*g_tInputEvent�ṹ���е����ݸ�ֵ������ָ��ָ��Ŀռ���*/
	pthread_mutex_unlock(&g_tMutex);

	debug("12321iX = %d, iY = %d\n",ptInputEvent->iX, ptInputEvent->iY);
	return 0;	
}

/**********************************************************************
 * �������ƣ� AllInputRegister
 * ���������� ���ø�������ģ��ĳ�ʼ������,����ע���������ģ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int AllInputRegister(void)
{
	int iError = 0;
	// iError = StdinInit();
	iError |= TouchScreenInit();
	return iError;
}




