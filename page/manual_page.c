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
static T_IconLayout g_tPicShowLayout; /*��Ƭ��ʾλ��*/
	
static T_PixelDatas g_tOriginalPixelDatas;
static T_PixelDatas g_tZoomPixelDatas;


/**********************************************************************
 * �������ƣ� CalcMPPicLayout
 * ���������� ����"�ֶ�ҳ��"����Ƭ��ʾ�����λ����Ϣ
 * ��������� 
 * ��������� ptPicShowLayout - ҳ����Ϣ,���㷵��ҳ�湦�ܲ˵�λ����Ϣ
 * �� �� ֵ�� 
 ***********************************************************************/
void CalcMPPicLayout (void)
{
	int iXres, iYres, iBpp;
	GetDefDispResolution(&iXres, &iYres, &iBpp);
	
	g_tPicShowLayout.iLeftTopX = g_atMPIconLayou[0].iRightBotX + 1;/*100*/
	g_tPicShowLayout.iLeftTopY = 0;
	g_tPicShowLayout.iRightBotX = iXres - 1;
	g_tPicShowLayout.iRightBotY = iYres - 1; 
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

	int iXres, iYres, iBpp;
	float k;
	int iError;
	
/*1. �����С����Ƭ�ĳ������Ϣ*/	
	/*��֤��Ƭ�ĳ���Ȳ���*/
	k = (float)ptOriginalPixelDatas->iWidth / ptOriginalPixelDatas->iHeight;
	
	GetDefDispResolution(&iXres, &iYres, &iBpp);
	g_tZoomPixelDatas.ibpp = iBpp;
	g_tZoomPixelDatas.iHeight = iZoomHeight;
	g_tZoomPixelDatas.iWidth = iZoomHeight * k;
	
	/*���������ĸ���С���Ը�С���Ǹ�Ϊ��׼*/
	if (g_tZoomPixelDatas.iWidth > iZoomWidth)
	{
		g_tZoomPixelDatas.iWidth = iZoomWidth;
		g_tZoomPixelDatas.iHeight = iZoomWidth / k;
	}

	g_tZoomPixelDatas.iLineByte = g_tZoomPixelDatas.iWidth * iBpp / 8;
	g_tZoomPixelDatas.iTotalByte = g_tZoomPixelDatas.iLineByte * g_tZoomPixelDatas.iHeight;
	g_tZoomPixelDatas.pucPixelDatas = malloc(g_tZoomPixelDatas.iTotalByte);
	if (NULL == g_tZoomPixelDatas.pucPixelDatas)
	{
		return NULL;
	}
	debug("k = %f\n",k);
	debug("g_tZoomPixelDatas.iWidth = %d\n", g_tZoomPixelDatas.iWidth);
	debug("g_tZoomPixelDatas.iHeight = %d\n", g_tZoomPixelDatas.iHeight);
	debug("g_tZoomPixelDatas.iLineByte = %d\n", g_tZoomPixelDatas.iLineByte);
	debug("g_tZoomPixelDatas.iTotalByte = %d\n", g_tZoomPixelDatas.iTotalByte);

	iError = PicZoom(ptOriginalPixelDatas, &g_tZoomPixelDatas);
	if (iError)
		return NULL;
	
	return &g_tZoomPixelDatas;
}

/**********************************************************************
 * �������ƣ� ShowPicture
 * ���������� ��"�ֶ�ҳ��"����ʾѡ�����Ƭ
 * ��������� g_tPicShowLayout - �ں�ͼƬ����ʾ����
 *			  ptVideoMem - �Դ�
 *			  strPicName - Ҫ��ʾ����Ƭ������
 * ��������� 
 * �� �� ֵ�� 0 - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
static int ShowPicture(PT_VideoMem ptVideoMem, char *strPicName)
{
	int iPictureWidth, iPictureHeight;		/*��Ƭ��ʾ����Ĵ�С��ʾ*/
	int iPicLeftTopX, iPicLeftTopY;			/*��Ƭ�ϲ�ʱ�ĺϲ�λ��*/
	PT_PixelDatas ptOriginalPixel, ptZoomPixel;
	
	/*1. �����Ƭ��ԭʼ����*/
	ptOriginalPixel = GetOriginalPixel(strPicName);		/*g_tOriginalPixelDatas - ʹ�õ�ȫ�ֱ����������Ƭԭʼ����*/
	debug("Have got [%s] pixel datas.\n", strPicName);
	
	/*2. �����Ƭ���ź�Ĵ�С���Լ����ź������*/
	iPictureWidth = g_tPicShowLayout.iRightBotX - g_tPicShowLayout.iLeftTopX + 1;
	iPictureHeight = g_tPicShowLayout.iRightBotY - g_tPicShowLayout.iLeftTopY + 1;

	ptZoomPixel = GetZoomPixel(&g_tOriginalPixelDatas, iPictureHeight, iPictureWidth);
	if (NULL == ptZoomPixel)
	{
		debug("fale: can not get [%s] zoom pixel\n", strPicName);
		return -1;
	}
	debug("Have got Zoomed pixel datas.\n");
	/*3. �����Ƭ���ź�Ĵ�С���ϲ����ڴ���ʱ��λ��*/
	debug("g_tPicShowLayout.iLeftTopX = %d\n",g_tPicShowLayout.iLeftTopX);
	debug("g_tPicShowLayout.iLeftTopY = %d\n",g_tPicShowLayout.iLeftTopY);

	debug("iPictureWidth = %d\n",iPictureWidth);
	debug("iPictureHeight = %d\n",iPictureHeight);
	debug("ptZoomPixel->iWidth = %d\n",ptZoomPixel->iWidth);
	debug("ptZoomPixel->iHeight = %d\n",ptZoomPixel->iHeight);

	iPicLeftTopX = g_tPicShowLayout.iLeftTopX + ((iPictureWidth - ptZoomPixel->iWidth) / 2);
	iPicLeftTopY = g_tPicShowLayout.iLeftTopY + ((iPictureHeight - ptZoomPixel->iHeight) / 2);

	debug("iPicLeftTopX = %d\n",iPicLeftTopX);
	debug("iPicLeftTopY = %d\n",iPicLeftTopY);

	ClearRegionVideoMem(g_tPicShowLayout.iLeftTopX, g_tPicShowLayout.iLeftTopY, g_tPicShowLayout.iRightBotX, g_tPicShowLayout.iRightBotY, ptVideoMem, BLACK);
	PicMerge(iPicLeftTopX, iPicLeftTopY, ptZoomPixel, &ptVideoMem->tPixelDatas);
	return 0;
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
		CalcMPPicLayout();					/*��ʾ��Ƭ�����λ��*/
	}
	/*3. ���Menuͼ������Ƭ����*/
	iError  = GeneratePage(ptPageLayout, ptVideoMem);
	iError |= ShowPicture(ptVideoMem, ptParentPageParams->strCurPictureFile);
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
	T_InputEvent tInputEvent;
	int iIndex;
	int iError;
	char strDirName[256];
	char strFileName[256];
	char strNewFileName[256];
	PT_DirContent *aptDirContents;
    int iDirContentsNumber;
	int iPicFileIndex;
	char *pcTmp;				
	PT_VideoMem ptDevVideoMem;

	T_PageParams tPageParams;
	tPageParams.iPageID = GetPageId("manual");
	ptDevVideoMem = GetDevVideoMem();

	/*1. ��ʾҳ��*/
	ManualPageShow(&g_tMPMenulayout, ptParentPageParams);
	/*2. ����Ƭ��š��ļ�Ŀ¼���д������㷭ҳ*/

	/* ȡ��Ŀ¼�� */
    strcpy(strDirName, ptParentPageParams->strCurPictureFile);
    pcTmp = strrchr(strDirName, '/');
    *pcTmp = '\0';	
	    /* ȡ���ļ��� */
    strcpy(strFileName, pcTmp+1);

    /* ��õ�ǰĿ¼������Ŀ¼���ļ������� */
    iError = GetDirContents(strDirName, &aptDirContents, &iDirContentsNumber);

    /* ȷ����ǰ��ʾ������һ���ļ� */
    for (iPicFileIndex = 0; iPicFileIndex < iDirContentsNumber; iPicFileIndex++)
    {
        if (0 == strcmp(strFileName, aptDirContents[iPicFileIndex]->strName))
        {
            break;
        }
    }
	
	/*3. ��������¼�*/
	while(1)
	{
		/*��ô�����������Menu����ͼ���±꣬tInputEvent ��ͨ��������ô���������*/
		iIndex = GenericGetInputEvent(&g_tMPMenulayout, &tInputEvent);
		if (tInputEvent.iPressure == 0)/*�������ɿ�״̬*/
		{
			switch (iIndex)
			{	
				case 0: 		/*����*/
				{
					return;
					break;
				}
				case 1: 		/*��С*/
				{
					
					break;
				}
				case 2: 		/*�Ŵ�*/
				{
					
					break;
				}
				case 3: 		/*��һ��*/
				{
					if (iPicFileIndex < 1)
						iPicFileIndex = iDirContentsNumber;
					iPicFileIndex--;
					sprintf(strNewFileName, "%s/%s", strDirName, aptDirContents[iPicFileIndex]->strName);
					if (isPictureFileSupported(strNewFileName))
					{
						ShowPicture(ptDevVideoMem, strNewFileName);
						break;
					}
				}
				case 4: 		/*��һ��*/
				{
					if (iPicFileIndex > iDirContentsNumber - 2)
						iPicFileIndex = -1;
					iPicFileIndex++;
					sprintf(strNewFileName, "%s/%s", strDirName, aptDirContents[iPicFileIndex]->strName);
					if (isPictureFileSupported(strNewFileName))
					{
						ShowPicture(ptDevVideoMem, strNewFileName);
						break;
					}

				}		
				case 5: 		/*���� */
				{
					
					break;
				}				
			}
			
			}
	}

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


