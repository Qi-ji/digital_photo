#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <disp_manager.h>
#include <page_manager.h>
#include <render.h>
#include <file.h>
#include <test.h>

static T_IconLayout g_atMPIconLayou[] = {
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, "zoomout.bmp"},
	{0, 0, 0, 0, "zoomin.bmp"},
	{0, 0, 0, 0, "pre_pic.bmp"},
    {0, 0, 0, 0, "next_pic.bmp"},
    {0, 0, 0, 0, "continue_mod_small.bmp"},
	{0, 0, 0, 0, NULL},

};

static T_PageLayout g_tMPMenulayout = {
	.iMaxTotalByte = 0,
	.atIconLayout = g_atMPIconLayou,

};
	
static T_PixelDatas g_tOriginalPixelDatas;
static T_PixelDatas g_tZoomPixelDatas;


/**********************************************************************
 * �������ƣ� CalcMPPicLayout
 * ���������� ����"�ֶ�ҳ��"����Ƭ��ʾ�����λ����Ϣ
 * ��������� ptPageLayout - ҳ����Ϣ,��ҳ�湦�ܲ˵���Ϣ
 * ��������� ptPageLayout - ҳ����Ϣ,���㷵��ҳ�湦�ܲ˵�λ����Ϣ
 * �� �� ֵ�� 
 ***********************************************************************/
void CalcMPPicLayout (PT_IconLayout ptPicShowLayout)
{
	int iXres, iYres, iBpp;
	GetDefDispResolution(&iXres, &iYres, &iBpp);
	
	ptPicShowLayout->iLeftTopX = g_atMPIconLayou[0].iRightBotX + 1;/*100*/
	ptPicShowLayout->iLeftTopY = 0;
	ptPicShowLayout->iRightBotX = iXres - 1;
	ptPicShowLayout->iRightBotY = iYres - 1; 
}


/**********************************************************************
 * �������ƣ� CalcBPMenuLayout
 * ���������� ����"�ֶ�ҳ��"�Ĺ��ܲ˵�λ����Ϣ
 * ��������� ptPageLayout - ҳ����Ϣ,��ҳ�湦�ܲ˵���Ϣ
 * ��������� ptPageLayout - ҳ����Ϣ,���㷵��ҳ�湦�ܲ˵�λ����Ϣ
 * �� �� ֵ�� 
 ***********************************************************************/
static void CalcMPMenuLayout(PT_PageLayout ptPageLayout)
{
	if(NULL == ptPageLayout || NULL == ptPageLayout->atIconLayout)
		return;
	
	int iError;
	int i;
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout;
	int iXres, iYres, iBpp;
	int iIconWidth, iIcongHeight, iIconTotalByte;
	
	GetDefDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;
	//ptPageLayout->iWidth = iXres * 3\4;
	//ptPageLayout->iHeight = iYres;

	/*return.bmp*/
	atIconLayout[0].iLeftTopX = 0;
	atIconLayout[0].iLeftTopY = 0;
	atIconLayout[0].iRightBotX = 100;		/*99*/
	atIconLayout[0].iRightBotY = iYres * 1/6 -1;	/*99*/

	/*zoomout.bmp*/
	atIconLayout[1].iLeftTopX = 0;									/*0*/
	atIconLayout[1].iLeftTopY = atIconLayout[0].iRightBotY + 1;		/*100*/
	atIconLayout[1].iRightBotX = 100;		/*99*/
	atIconLayout[1].iRightBotY = iYres * 2/6 -1;					/*199*/

	/*zoomin.bmp*/
	atIconLayout[2].iLeftTopX = 0;									/*0*/
	atIconLayout[2].iLeftTopY = atIconLayout[1].iRightBotY + 1;		/*200*/
	atIconLayout[2].iRightBotX = 100;		/*99*/
	atIconLayout[2].iRightBotY = iYres * 3/6 -1;					/*299*/

	/*pre_pic.bmp*/
	atIconLayout[3].iLeftTopX = 0;
	atIconLayout[3].iLeftTopY = atIconLayout[2].iRightBotY + 1;		/*300*/
	atIconLayout[3].iRightBotX = 100;		/*99*/
	atIconLayout[3].iRightBotY = iYres * 4/6 -1;					/*399*/

	/*next_pic.bmp*/
	atIconLayout[4].iLeftTopX = 0;
	atIconLayout[4].iLeftTopY = atIconLayout[3].iRightBotY + 1;		/*400*/
	atIconLayout[4].iRightBotX = 100;		/*99*/
	atIconLayout[4].iRightBotY = iYres * 5/6 -1;					/*499*/

	/*continue_mod_small.bmp*/
	atIconLayout[5].iLeftTopX = 0;
	atIconLayout[5].iLeftTopY = atIconLayout[4].iRightBotY + 1;		/*500*/
	atIconLayout[5].iRightBotX = 100;		/*99*/
	atIconLayout[5].iRightBotY = iYres -1;					/*599*/

	for (i=0; i<6; i++)
	{
		iIconWidth     = atIconLayout[i].iRightBotX - atIconLayout[i].iLeftTopX + 1;
		iIcongHeight   = atIconLayout[i].iRightBotY - atIconLayout[i].iLeftTopY + 1;
		iIconTotalByte = iIconWidth * iIcongHeight * iBpp / 8;
		if (iIconTotalByte > ptPageLayout->iMaxTotalByte)
			ptPageLayout->iMaxTotalByte = iIconTotalByte;
	}

}

/**********************************************************************
 * �������ƣ� GetOriginalPixel
 * ���������� ��"�ֶ�ҳ��"�ϻ��Ҫ��ʾ��Ƭ��ԭʼ����
 * ��������� strPicName - Ҫ��ʾ����Ƭ������
 *			  
 * ��������� 
 * �� �� ֵ�� g_tOriginalPixelDatas - ʹ�õ�ȫ�ֱ����������Ƭԭʼ����
 *            NULL - ʧ��
 ***********************************************************************/
static PT_PixelDatas GetOriginalPixel(char *strPicName)
{
	T_FileMap tFileMap;
	int iXres, iYres, iBpp;
	int iError;
	PT_PicFilePraser Praser;

	if (g_tOriginalPixelDatas.pucPixelDatas)	/*ʹ��֮ǰ�ȶ�g_tOriginalPixelDatas.pucPixelDatas����malloc�ģ���getpicel�����л����malloc*/
	{
		free (g_tOriginalPixelDatas.pucPixelDatas);
		g_tOriginalPixelDatas.pucPixelDatas = NULL;
	}
	
	strncpy(tFileMap.strFileName, strPicName,256);
	tFileMap.strFileName[255] = '\0'; 
	iError = MapFile(&tFileMap);
	if (iError)
	{
		debug("MapFile [%s] error.\n", strPicName);
		return NULL;
	}

	GetDefDispResolution(&iXres, &iYres, &iBpp);
	g_tOriginalPixelDatas.ibpp = iBpp;
	
	Praser = GetPicPraser(&tFileMap);
	if (NULL == Praser)
	{
		debug("Do not find the praser for [%s].\n", strPicName);
		UnMapFile(&tFileMap);
		return NULL;
	}
		
	iError = Praser->GetPixelDatas(&tFileMap,&g_tOriginalPixelDatas);
	if (iError)
	{
		debug("Can not get [%s] pixel datas\n", strPicName);
		UnMapFile(&tFileMap);
		return NULL;
	}
	
	UnMapFile(&tFileMap);/*ע����ʹ�ý���֮��ҪUnmapfile*/
	return &g_tOriginalPixelDatas;
}

/**********************************************************************
 * �������ƣ� GetZoomPixel
 * ���������� ��"�ֶ�ҳ��"�ϻ��Ҫ��ʾ��Ƭ�����ź������
 * ��������� ptOriginalPixelDatas - Ҫ��ʾ��Ƭ��ԭʼ����
 *			  iZoomHeight - ���ź�ĸ߶�
 *			  iZoomWidth - ���ź�Ŀ��
 * ��������� 
 * �� �� ֵ�� g_tZoomPixelDatas - ʹ�õ�ȫ�ֱ����������Ƭ���ź������
 *            NULL - ʧ��
 ***********************************************************************/
static PT_PixelDatas GetZoomPixel(PT_PixelDatas ptOriginalPixelDatas, int iZoomHeight,int iZoomWidth)
{
	return NULL;
}

/**********************************************************************
 * �������ƣ� ShowPicure
 * ���������� ��"�ֶ�ҳ��"����ʾѡ�����Ƭ
 * ��������� ptPicShowLayout - �ں�ͼƬ����ʾ����
 *			  ptVideoMem - �Դ�
 *			  strPicName - Ҫ��ʾ����Ƭ������
 * ��������� 
 * �� �� ֵ�� 0 - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
static int ShowPicure(PT_IconLayout ptPicShowLayout, PT_VideoMem ptVideoMem, char *strPicName)
{
    int iError;
	int iPictureWidth, iPictureHeight;
	PT_PixelDatas ptOriginalPixel, ptZoomPixel;
	
	/*1. �����Ƭ��ԭʼ����*/
	ptOriginalPixel = GetOriginalPixel(strPicName);		/*g_tOriginalPixelDatas - ʹ�õ�ȫ�ֱ����������Ƭԭʼ����*/

	/*2. �����Ƭ���ź�Ĵ�С���Լ����ź������*/
	iPictureWidth = ptPicShowLayout->iRightBotX - ptPicShowLayout->iLeftTopX + 1;
	iPictureHeight = ptPicShowLayout->iRightBotY - ptPicShowLayout->iLeftTopY + 1;

	ptZoomPixel = GetZoomPixel(&g_tOriginalPixelDatas, iPictureHeight, iPictureWidth);

	
	/*3. �����Ƭ���ź�Ĵ�С���ϲ����ڴ���ʱ��λ��*/
	
}


/**********************************************************************
 * �������ƣ� ManualPageGetInputEvent
 * ���������� Ϊ"�ֶ�ҳ��"�����������,�ж������¼�λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(PageLayout->atLayout�������һ��)
 ***********************************************************************/
static int ManualPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * �������ƣ� ManualPageShow
 * ���������� "�ֶ�ҳ��"����ʾ��������ʾ�˵������Լ�Ҫ��ʾ����Ƭ
 * ��������� ptPageLayout - ҳ�� �˵����� ͼ�꼰λ��
 *			  ptParentPageParams - ��һ��ҳ���ID�Լ�Ҫ��ʾ����Ƭ��·����Ϣ
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
static void ManualPageShow(PT_PageLayout ptPageLayout, PT_PageParams ptParentPageParams)
{
	PT_VideoMem ptVideoMem; 
	T_IconLayout tShowPicLayout; /*��Ƭ��ʾλ��*/
	PT_IconLayout ptIconLayout = ptPageLayout->atIconLayout; /*Menuͼ����ʾλ��*/
	int iError;
	
	/*1.����Դ�*/
	ptVideoMem = GetVideoMem(GetPageId("manual"), 1);
	if (NULL == ptVideoMem)
		return;
	
	/*2. ���Menuͼ������λ���Լ���Ƭ��ʾ����λ��*/
	if (g_atMPIconLayou[0].iLeftTopX == 0)
	{
	    CalcMPMenuLayout(ptPageLayout);
		CalcMPPicLayout(&tShowPicLayout);					/*��ʾ��Ƭ�����λ��*/
	}
	/*3. ���Menuͼ������Ƭ����*/
	iError  = GeneratePage(ptPageLayout, ptVideoMem);
	iError |= ShowPicure(&tShowPicLayout, ptVideoMem, ptParentPageParams->strCurPictureFile);
	if (iError)
		return;

	/*4. ˢ���豸��*/
	FlushVideoMemToDev(ptVideoMem);

}

/**********************************************************************
 * �������ƣ� ManualPageRun
 * ���������� "�ֶ�ҳ��"�����к���: 
 * ��������� ptParentPageParams - ��ת����ҳ�����Ϣ
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
static void ManualPageRun(PT_PageParams ptParentPageParams)
{
	T_PageParams tPageParams;
	tPageParams.iPageID = GetPageId("manual");
	
		//{
	ManualPageShow(&g_tMPMenulayout, ptParentPageParams);
		//}
	//while(1);

}


T_PageAction g_tMPAction = {
	.name 			= "manual",
	.Run 			= ManualPageRun,
	.GetInputEvent  = ManualPageGetInputEvent,
};
/**********************************************************************
 * �������ƣ� ManualPageInit
 * ���������� ע��"�ֶ�ҳ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸����� 	   �汾��	 �޸���		  �޸�����
 ***********************************************************************/
int ManualPageInit(void)
{
	return PageActionRegister(&g_tMPAction);
}


