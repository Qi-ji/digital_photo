#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config.h>
#include <page_manager.h>
#include <disp_manager.h>
#include <render.h>
#include <file.h>
#include <test.h>


static T_IconLayout g_atSPIconsLayout[] = {
	{0, 0, 0, 0, "select_fold.bmp"},
	{0, 0, 0, 0, "interval.bmp"},
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, NULL},
};

static T_PageLayout g_atSPMenuLayout = {
	.iMaxTotalByte = 0,
	.atIconLayout  = g_atSPIconsLayout,
};


/**********************************************************************
 * �������ƣ� CalcSPMenuLayout
 * ���������� ����"����ҳ��"�Ĺ��ܲ˵�λ����Ϣ
 * ��������� ptPageLayout - ҳ����Ϣ,��ҳ�湦�ܲ˵���Ϣ
 * ��������� ptPageLayout - ҳ����Ϣ,���㷵��ҳ�湦�ܲ˵�λ����Ϣ
 * �� �� ֵ�� 
 ***********************************************************************/
static int CalcSPMenuLayout(PT_PageLayout ptPageLayout)
{
	if (NULL == ptPageLayout || NULL == ptPageLayout->atIconLayout)
			return -1;
	
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout;
	
	int iXres, iYres, iBpp;  /*��ʾ�豸�Ĳ���*/
	int iIconWidth, iIconHeight; /*ÿ��ͼ��Ŀ��߶���һ���ģ����Լ����һ��ͼ��Ϳ�����*/
	int iIconTotalByte;			
	int i = 0;

	GetDefDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;

	/*select_fold.bmp*/
	atIconLayout[0].iLeftTopX = iXres * 2/5;		
	atIconLayout[0].iLeftTopY = iYres * 1/10;		/*102*/
	atIconLayout[0].iRightBotX = iXres * 3/5;		
	atIconLayout[0].iRightBotY = iYres * 3/10;		/*306*/

	/*interval.bmp*/
	atIconLayout[1].iLeftTopX = iXres * 2/5;		
	atIconLayout[1].iLeftTopY = iYres * 4/10;		/*408*/
	atIconLayout[1].iRightBotX = iXres * 3/5;		
	atIconLayout[1].iRightBotY = iYres * 6/10;		
	/*return.bmp*/
	atIconLayout[2].iLeftTopX = iXres * 2/5;		
	atIconLayout[2].iLeftTopY = iYres * 7/10;		/*408*/
	atIconLayout[2].iRightBotX = iXres * 3/5;		
	atIconLayout[2].iRightBotY = iYres * 9/10;

	for(i=0; i<3; i++)
	{
		iIconWidth = atIconLayout[i].iRightBotX - atIconLayout[i].iLeftTopX +1; /*ע�����������Ҫ��1 ��*/
		iIconHeight = atIconLayout[i].iRightBotY - atIconLayout[i].iLeftTopY +1;
		iIconTotalByte = iIconWidth * iIconHeight * iBpp / 8;
		if (ptPageLayout->iMaxTotalByte < iIconTotalByte)
		{
			ptPageLayout->iMaxTotalByte = iIconTotalByte;
		}
	}
	return 0;
}

/**********************************************************************
 * �������ƣ� SettingPageGetInputEvent
 * ���������� Ϊ"����ҳ��"�����������,�ж������¼�λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(PageLayout->atLayout�������һ��)
 ***********************************************************************/
static int SettingPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * �������ƣ� SettingPageRun
 * ���������� "����ҳ��"�����к���:
 * ��������� ptParentPageParams - ҳ����Ϣ
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
static void SettingPageShow(PT_PageLayout ptPageLayout)
{
	int iError;
	PT_VideoMem ptVideoMem;
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout;
	
	/*1. ����Դ�*/
	ptVideoMem = GetVideoMem(GetPageId("settingpage"), 1);
	if (NULL == ptVideoMem)
	{
		debug("error: setting page videomem error\n");
		return;
	}
	
	/*2. ��������*/
	if (g_atSPIconsLayout[0].iLeftTopX == 0)
	{
		CalcSPMenuLayout(ptPageLayout);
	}
	/*3. ���ͼ����������*/
	iError = GeneratePage(ptPageLayout, ptVideoMem);
	if (iError)
	{
		debug("error: generate setting page error\n");
		return ;
	}
	/*4. ˢ���豸�Ͻ�����ʾ*/
	FlushVideoMemToDev(ptVideoMem);
}



/**********************************************************************
 * �������ƣ� SettingPageRun
 * ���������� "����ҳ��"�����к���:
 * ��������� ptParentPageParams - ҳ����Ϣ
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
static void SettingPageRun(PT_PageParams ptParentPageParams)
{
	T_PageParams tPageParams;
	int iIndex;
	T_InputEvent tInputEvent;

	tPageParams.iPageID = GetPageId("settingpage");
	
	/*1. ҳ����ʾ*/
	SettingPageShow(&g_atSPMenuLayout);
	
	/* 2.��������¼�*/
	while (1)
	{
		/*��ô�����������ͼ���±�*/
		iIndex = GenericGetInputEvent(&g_atSPMenuLayout, &tInputEvent);
		
		switch (iIndex)
		{
			case 0: 		/*ѡ��Ŀ¼*/
			{
				GetPage("browsepage")->Run(&tPageParams);		/*���������Ϊ���ж����ĸ�ҳ������*/ 
				SettingPageShow(&g_atSPMenuLayout);
				break;
			}
			case 1: 		/*���ü��*/
			{
				GetPage("autopage")->Run(&tPageParams); 	/*���������Ϊ���ж����ĸ�ҳ������*/ 
				SettingPageShow(&g_atSPMenuLayout);

				break;
			}
			case 2: 		/*����*/
			{
				return;
				break;
			}
			default:
				break;
		}

	}

}


T_PageAction g_tSPAction = {
	.name 			= "settingpage",
	.Run 			= SettingPageRun,
	.GetInputEvent  = SettingPageGetInputEvent,
};

/**********************************************************************
 * �������ƣ� SettingPageInit
 * ���������� ע��"����ҳ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int SettingPageInit(void)
{
	return PageActionRegister(&g_tSPAction);
}









