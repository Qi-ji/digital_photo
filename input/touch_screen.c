#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include <linux/input.h>
#include <input_manager.h>
#include <config.h>

static int g_TSfd;		/*�������豸fd*/

/**********************************************************************
 * �������ƣ� TSDevInit
 * ���������� ����������ģ����豸��ʼ������
 *            ע��: ����Ҫ�õ�LCD�ķֱ���, �˺���Ҫ��SelectAndInitDisplay֮����ܵ��� 
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int TSDevInit(void)
{
	/*1.���豸*/	
	g_TSfd = open(TOUCHSCREENDEV, O_RDONLY);
	if (g_TSfd < 0)
	{
		debug("TouchScreenDev open error.\n");
		return -1;
	}
	return 0;
}

/**********************************************************************
 * �������ƣ� TSDevExit
 * ���������� ����������ģ����豸�˳�����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void TSDevExit(void)
{
	close(g_TSfd);
}

/**********************************************************************
 * �������ƣ� TouchScreenGetInputEvent
 * ���������� ����������ģ��Ķ�ȡ���ݺ���,���ڴ���������ģ������߳��б�����
 * ��������� ��
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int TSGetInputEvent(PT_InputEvent ptInputEvent)
{
	int ret, i = 0;
	struct input_event t_TSInput;
	
	while(1)
	{
	/*������*/
		//memset(&t_TSInput, 0, sizeof(struct input_event));  /*�Ƚ��ṹ��ȫ������*/
		ret = read(g_TSfd, &t_TSInput, sizeof(struct input_event));
		if (ret == sizeof(struct input_event))
		{
			/*X����*/
			if ((t_TSInput.type == EV_ABS) && (t_TSInput.code == ABS_X))
				ptInputEvent->iX = t_TSInput.value;
			if ((t_TSInput.type == EV_ABS) && (t_TSInput.code == ABS_Y))
				ptInputEvent->iY = t_TSInput.value;
/*
			if ((t_TSInput.type == 0) && (t_TSInput.code == 0) && (t_TSInput.value == 0))
			{
				ptInputEvent->iPressure = 0; 
			}
*/		
		}
		else
		{
			debug("read TouchScreenDev error.\n");
			TSDevExit();
			return -1;
		}
		
		if( t_TSInput.type == EV_ABS )
		{
			ptInputEvent->iType = TOUCHSCREEN;
			ptInputEvent->iPressure = 1;
		}
		else 
		{
			ptInputEvent->iPressure = 0;
		}

		debug("X = %d, Y = %d, iType = %d, iPressur = %d.\n",ptInputEvent->iX, ptInputEvent->iY, ptInputEvent->iType, ptInputEvent->iPressure);
		//return 0;   /*��㲻���ΪʲôҪ����*/
		//if(i++ == 2)
			return 0;
	}
	return 0;   /*��㲻���ΪʲôҪ����*/
}

T_InputOpr g_tTSOpr = {
	.DeviceInit = TSDevInit,
	.DeviceExit = TSDevExit,
	.DevGetInputEvent = TSGetInputEvent,
};
/**********************************************************************
 * �������ƣ� TouchScreenInit
 * ���������� ע��"����������ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int TouchScreenInit(void)
{
	return InputOprtRegister(&g_tTSOpr);
}









