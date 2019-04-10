#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <disp_manager.h>
#include <page_manager.h>
#include <render.h>
#include <file.h>
#include <test.h>

T_PageCfg g_tPageCfg;
int g_ibExitAutoPlay = 0;    /*Flag����Ϊ1ʱ���˳�ѭ������ҳ��*/
int g_flag = 0;
static pthread_t g_AutoPageThreadID;
static pthread_mutex_t g_tAutoPlayThreadMutex  = PTHREAD_MUTEX_INITIALIZER; /* ������ */

/**********************************************************************
 * �������ƣ� AutoPageGetInputEvent
 * ���������� Ϊ"����ҳ��"�����������,�ж������¼�λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(PageLayout->atLayout�������һ��)
 ***********************************************************************/
static int AutoPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * �������ƣ� AutoPageFunction
 * ���������� "����ҳ��"����ʾ����
 * ��������� ptPageLayout - ҳ�� �˵����� ͼ�꼰λ��
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
static void *AutoPageFunction(void *pVoid)
{
	picture_scan(g_tPageCfg.strSeletedDir);
	while(1)
	{
/*
		pthread_mutex_lock(&g_tAutoPlayThreadMutex);	
	    g_flag = g_ibExitAutoPlay;    AutoPageFunction�̼߳�⵽�������Ϊ1����˳� 			
	    pthread_mutex_unlock(&g_tAutoPlayThreadMutex);
*/		
		if(picture_display())			/*����1�Ļ���˵���Ǵ������˳���*/
		{
			return NULL;
		}

	}
}
/**********************************************************************
 * �������ƣ� GetAutoPageCfg
 * ���������� "����ҳ��"�������ݵĻ�ã�����Ҫ��ʾ���ļ��С���ʾ�ļ��
 * ��������� 
 * ��������� ptPageCfg - ������Ϣ
 * �� �� ֵ�� ��
 ***********************************************************************/
 static void GetAutoPageCfg(PT_PageCfg ptPageCfg)
{
	GetSlectDir(ptPageCfg->strSeletedDir);
	//GetIntervalTime(&ptPageCfg->iIntervalSecond);
}

/**********************************************************************
 * �������ƣ� AutoPageRun
 * ���������� "����ҳ��"�����к���: ��ʾ·���е�Ŀ¼���ļ�
 * ��������� ptParentPageParams - ҳ����Ϣ
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
static void AutoPageRun(PT_PageParams ptParentPageParams)
{
	char *pTemp;
	int iRet;
	T_InputEvent tInputEvent;
	T_PageParams tPageParams;
	tPageParams.iPageID = GetPageId("autopage");
	g_ibExitAutoPlay = 0;
	g_flag = 0;
		
	/*1. �����ʾ�Ĳ�������ʾ���ļ��С���ʾ�ļ��*/
	GetAutoPageCfg(&g_tPageCfg);
	g_tPageCfg.iIntervalSecond = 1000;/*���룻��������ҳ������Ҫɾ��*/


	/*�п�������manualҳ��ֱ�ӽ��������ģ������ж�pageparam��*/
	/*���������ļ���·���Ļ�����ʾ��·���µ���Ƭ*/
	if (ptParentPageParams->strCurPictureFile[0] != '\0')
	{
		strncpy(g_tPageCfg.strSeletedDir, ptParentPageParams->strCurPictureFile, 256);
		pTemp = strrchr(g_tPageCfg.strSeletedDir, '/');
		*pTemp = '\0';				/*�������ļ���ȥ����ֻ���Ŀ¼·��*/ 
	}

	/*2. ����һ���߳�����ʾ��Ƭ*/
	pthread_create(&g_AutoPageThreadID, NULL, AutoPageFunction,NULL);

	/*3. ��ô������¼�*/
	while(1)
	{
		iRet = GetInputEvent(&tInputEvent);
		if (0 == iRet)
		{
            pthread_mutex_lock(&g_tAutoPlayThreadMutex);
            g_ibExitAutoPlay = 1;   /* AutoPageFunction�̼߳�⵽�������Ϊ1����˳� */
			g_flag = g_ibExitAutoPlay;	
            pthread_mutex_unlock(&g_tAutoPlayThreadMutex);
            pthread_join(g_AutoPageThreadID, NULL);  /* �ȴ����߳��˳� */
            return;			
		}

	}

}

T_PageAction g_tAPAction = {
	.name 			= "autopage",
	.Run			= AutoPageRun,
	.GetInputEvent  = AutoPageGetInputEvent,
};

/**********************************************************************
 * �������ƣ� MainPageInit
 * ���������� ע��"��ҳ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 ***********************************************************************/
int AutoPageInit(void)
{
	return PageActionRegister(&g_tAPAction);
}
















