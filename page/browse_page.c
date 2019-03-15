#include <stdio.h>
#include <stdlib.h>

#include <disp_manager.h>
#include <page_manager.h>
#include <render.h>


/* ͼ����һ��������, "ͼ��+����"Ҳ��һ��������
 *   --------
 *   |  ͼ  |
 *   |  ��  |
 * ------------
 * |   ����   |
 * ------------
 */
#define DIR_FILE_ICON_WIDTH    80
#define DIR_FILE_ICON_HEIGHT   DIR_FILE_ICON_WIDTH
#define DIR_FILE_NAME_HEIGHT   40
#define DIR_FILE_NAME_WIDTH   (DIR_FILE_ICON_HEIGHT + DIR_FILE_NAME_HEIGHT)
#define DIR_FILE_ALL_WIDTH    DIR_FILE_NAME_WIDTH
#define DIR_FILE_ALL_HEIGHT   DIR_FILE_ALL_WIDTH

static T_IconLayout g_atMenuIconLayout[] = {
	{0,0,0,0,"up.bmp"},
	{0,0,0,0,"select.bmp"},
	{0,0,0,0,"pre_page.bmp"},
	{0,0,0,0,"next_page.bmp"},
	{0,0,0,0,NULL},
};

static T_PageLayout g_tBPMenuPageLayout = {		/*ҳ��˵�����ͼ�꼰λ��*/
	.iMaxTotalByte = 0,
	.atIconLayout = g_atMenuIconLayout,
};

static char g_strCurDir[256] = DEFAULT_DIR;
//static char g_strSelectedDir[256] = DEFAULT_DIR;

/* ��������ĳĿ¼������� */
static PT_DirContent *g_aptDirContents;  		/* ����:����Ŀ¼��"������Ŀ¼","�ļ�"������ */
static int g_iDirContentsNumber;         		/* g_aptDirContents�����ж����� */

static T_PixelDatas g_tDirClosedIconPixel;
static T_PixelDatas g_tDirOpenedIconPixel;
static T_PixelDatas g_tFileIconPixel;

static int g_iNumPerRow, g_iNumPerCol;
static int g_iStartIndex = 0;
static T_IconLayout *g_atDirAndFileLayout = NULL; /*�ļ��м��ļ�λ����Ϣ*/


static char *g_strDirClosedIcon  = "fold_closed.bmp";
static char *g_strDirOpenedIcon  = "fold_opened.bmp";
static char *g_strFileIcon = "file.bmp";
//static int g_iStartIndex = 0;            /* ����Ļ����ʾ�ĵ�1��"Ŀ¼���ļ�"��g_aptDirContents���������һ�� */

/**********************************************************************
 * �������ƣ� CalcBPMenuLayout
 * ���������� ����"���ҳ��"�Ĺ��ܲ˵�λ����Ϣ
 * ��������� ptParentPageParams - ҳ����Ϣ,��ҳ�湦�ܲ˵���Ϣ
 * ��������� ptParentPageParams - ҳ����Ϣ,���㷵��ҳ�湦�ܲ˵�λ����Ϣ
 * �� �� ֵ�� 
 ***********************************************************************/
static int CalcBPMenuLayout(PT_PageLayout ptPageLayout)
{
	if (NULL == ptPageLayout || NULL == ptPageLayout->atIconLayout)
		return -1;
debug("-----------------1.start Calc Browse Page Menu Layout----------------\n");
	int iXres, iYres, iBpp;  /*��ʾ�豸�Ĳ���*/
	int iIconWidth, iIconHeight; /*ÿ��ͼ��Ŀ��߶���һ���ģ����Լ����һ��ͼ��Ϳ�����*/
	int iIconTotalByte;			/*ÿ��ͼ����ܴ�С����һ����*/
	PT_IconLayout atIconLayout;
	int i = 0;

	GetDefDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;
	atIconLayout = ptPageLayout->atIconLayout;

	/*up.bmp ͼ��λ�ü���*/
	atIconLayout[0].iLeftTopX = 0;
	atIconLayout[0].iLeftTopY = 0;
	atIconLayout[0].iRightBotX = iXres * 1/4;
	atIconLayout[0].iRightBotY = iYres * 1/4;

	/*select.bmp ͼ��λ�ü���*/
	atIconLayout[1].iLeftTopX = iXres * 0;
	atIconLayout[1].iLeftTopY = iYres * 1/4 + 1;
	atIconLayout[1].iRightBotX = iXres * 1/4;
	atIconLayout[1].iRightBotY = iYres * 2/4;
	
	/*pre_page.bmp ͼ��λ�ü���*/
	atIconLayout[2].iLeftTopX = iXres * 0;
	atIconLayout[2].iLeftTopY = iYres * 2/4 + 1;
	atIconLayout[2].iRightBotX = iXres * 1/4;
	atIconLayout[2].iRightBotY = iYres * 3/4;

	/*snext_page.bmp ͼ��λ�ü���*/
	atIconLayout[3].iLeftTopX = iXres * 0;
	atIconLayout[3].iLeftTopY = iYres * 3/4 + 1;
	atIconLayout[3].iRightBotX = iXres * 1/4;
	atIconLayout[3].iRightBotY = iYres * 1;

	for(i=0; i<4; i++)
	{
		iIconWidth = atIconLayout[i].iRightBotX - atIconLayout[i].iLeftTopX +1; /*ע�����������Ҫ��1 ��*/
		iIconHeight = atIconLayout[i].iRightBotY - atIconLayout[i].iLeftTopY +1;
		iIconTotalByte = iIconWidth * iIconHeight * iBpp / 8;
		if (ptPageLayout->iMaxTotalByte < iIconTotalByte)
		{
			ptPageLayout->iMaxTotalByte = iIconTotalByte;
		}
	}
debug("-----------------1.end Calc Browse Page Menu Layout----------------\n");

	return 0;
}

/**********************************************************************
 * �������ƣ� CalcBPInterfaceLayout
 * ���������� ����"���ҳ��"�Ľ���ͼ��λ����Ϣ���������ļ����Լ��ļ�ͼ�����Ϣ
 *		 ͨ��ȫ�ֱ��������ݺ���
 * ��������� ��
 * ��������� ptInterfaceLayout - ���зǲ˵������λ����Ϣ,��ָ���˸���ͼ�������λ�á�
 * �� �� ֵ�� PT_IconLayout aptIconLayout - �����к��и���ͼ�꼰���Ƶ�λ����Ϣ
 ***********************************************************************/
static int CalcBPInterfaceLayout(PT_PageLayout ptInterfaceLayout)
{
	int iXres, iYres, iBpp;
	int iIconWidth, iIconHeight;			/*ÿ��ͼ��ĳ���*/
	int iNumPerRow, iNumPerCol;			/* ÿ��ÿ��ͼ��ĸ���*/
	int iInterFaceWidth, iInterFaceHeight;/*�ǲ˵�����Ŀ��*/
	int iDeltaX, iDeltaY;				/*ÿ��ͼ��֮��ļ��*/
	int iIconLeftTopX, iIconLeftTopY; /*�ļ���ͼ����ʾ�Ŀ�ʼλ������*/
	int i ,j, k = 0;
	int iTotalByte;
	//PT_PageLayout ptInterfaceLayout; /*���зǲ˵��������ʾ��Ϣ*/
debug("-----------------2.start Calc Browse Page Interface Layout----------------\n");

/* 1.��÷ǲ˵���ʾ�����λ����Ϣ�������ļ��м��ļ�����ʾ����*/
	GetDefDispResolution(&iXres, &iYres,&iBpp);
	ptInterfaceLayout->iLeftTopX 	 = g_atMenuIconLayout[0].iRightBotX + 1;
	ptInterfaceLayout->iLeftTopY 	 = 0;
	ptInterfaceLayout->iRightBotX	 = iXres - 1;
	ptInterfaceLayout->iRightBotY 	 = iYres - 1;
	ptInterfaceLayout->iBpp 		 = iBpp;
	ptInterfaceLayout->iMaxTotalByte = DIR_FILE_ALL_HEIGHT * DIR_FILE_ALL_WIDTH * iBpp / 8;
	
debug("�ǲ˵���ʾ��������ptInterfaceLayout->iLeftTopX = %d\n", ptInterfaceLayout->iLeftTopX);
debug("ptInterfaceLayout->iLeftTopY = %d\n", ptInterfaceLayout->iLeftTopY);
debug("ptInterfaceLayout->iRightBotX = %d\n", ptInterfaceLayout->iRightBotX);
debug("ptInterfaceLayout->iRightBotY = %d\n", ptInterfaceLayout->iRightBotY);

/* 2.��ø��ļ�ͼ�����ʾ���� */
    iIconWidth  = DIR_FILE_ALL_WIDTH;		
    iIconHeight = iIconWidth;	
	/* 2.1ȷ��ÿ��ÿ�п�����ʾ���ٸ�ͼ��*/
	iInterFaceWidth = ptInterfaceLayout->iRightBotX - ptInterfaceLayout->iLeftTopX + 1;
	iNumPerRow = iInterFaceWidth / iIconWidth;
	 /*ȷ��ÿ��ͼ��֮��ļ������10������*/
	while (1)
	{
		iDeltaX = iInterFaceWidth - iNumPerRow * iIconWidth;/*�ܵĿ�϶����*/
		if (iDeltaX / (iNumPerRow + 1) < 10)
		{
			iNumPerRow--;
		}
		else 
			break;
	}
	g_iNumPerRow = iNumPerRow;
	iDeltaX = iDeltaX / (iNumPerRow + 1);/*ÿ��ÿ��ͼ��֮��ļ��*/

	iInterFaceHeight = ptInterfaceLayout->iRightBotY- ptInterfaceLayout->iLeftTopY+ 1;
	debug("11111111111111111 iInterFaceHeight= %d\n",iInterFaceHeight);
	iNumPerCol= iInterFaceHeight / iIconHeight;
	 /*ȷ��ÿ��ͼ��֮��ļ������10������*/
	while (1)
	{
		iDeltaY = iInterFaceHeight - iNumPerCol* iIconHeight;/*�ܵĿ�϶����*/
		if (iDeltaY / (iNumPerCol+ 1) < 10)
		{
			iNumPerCol--;
		}
		else 
			break;
	}
	g_iNumPerCol = iNumPerCol;
	iDeltaY = iDeltaY / (iNumPerCol+ 1);/*ÿ��ÿ��ͼ��֮��ļ��*/
   /* ������ʾ iNumPerRow * iNumPerCol��"Ŀ¼���ļ�"
     * ����"����+1"��T_Layout�ṹ��: һ��������ʾͼ��,��һ��������ʾ����
     * ���һ��������NULL,�����жϽṹ�������ĩβ
     */
	g_atDirAndFileLayout = malloc(sizeof(T_IconLayout) * (2 * iNumPerRow *iNumPerCol + 1));
   if (NULL == g_atDirAndFileLayout)
   {
	   debug("malloc error!\n");
	   return -1;
   }
   ptInterfaceLayout->atIconLayout  = g_atDirAndFileLayout;
    /* ȷ��ͼ������ֵ�λ�� 
     *
     * ͼ����һ��������, "ͼ��+����"Ҳ��һ��������
     *   --------
     *   |  ͼ  |
     *   |  ��  |
     * ------------
     * |   ����   |
     * ------------
     */
	iIconLeftTopX = ptInterfaceLayout->iLeftTopX + iDeltaX;
	iIconLeftTopY = ptInterfaceLayout->iLeftTopY + iDeltaY;

	/* k ��ʾ��ͼ��������ַ�� k + 1 ��ʾ�����ֵ������ַ*/
	for (i=0; i<iNumPerCol; i++)
	{
		for(j=0; j<iNumPerRow; j++)
		{
			/* ͼ�� */
			g_atDirAndFileLayout[k].iLeftTopX  = iIconLeftTopX + (DIR_FILE_NAME_WIDTH - DIR_FILE_ICON_WIDTH) / 2;
			g_atDirAndFileLayout[k].iLeftTopY  = iIconLeftTopY;
			g_atDirAndFileLayout[k].iRightBotX = g_atDirAndFileLayout[k].iLeftTopX + DIR_FILE_ICON_WIDTH - 1;
			g_atDirAndFileLayout[k].iRightBotY = g_atDirAndFileLayout[k].iLeftTopY + DIR_FILE_ICON_HEIGHT - 1;
debug("g_atDirAndFileLayout[%d].iLeftTopX,iLeftTopY = (%d,%d)\n", k,g_atDirAndFileLayout[k].iLeftTopX, g_atDirAndFileLayout[k].iLeftTopY);

			g_atDirAndFileLayout[k+1].iLeftTopX  = iIconLeftTopX;
			g_atDirAndFileLayout[k+1].iLeftTopY  = iIconLeftTopY + DIR_FILE_ICON_HEIGHT + 1;
			g_atDirAndFileLayout[k+1].iRightBotX = iIconLeftTopX + DIR_FILE_NAME_WIDTH - 1;
			g_atDirAndFileLayout[k+1].iRightBotY = iIconLeftTopY + DIR_FILE_ALL_HEIGHT - 1;
debug("g_atDirAndFileLayout[%d].iLeftTopX,iLeftTopY = (%d,%d)\n", k+1,g_atDirAndFileLayout[k+1].iLeftTopX, g_atDirAndFileLayout[k+1].iLeftTopY);

			iIconLeftTopX += DIR_FILE_ALL_WIDTH + iDeltaX;
			k += 2;
//debug("ptInterfaceLayout->atIconLayout[%d].iLeftTopX = %d, g_atDirAndFileLayout[k].iLeftTopY = %d\n", k,ptInterfaceLayout->atIconLayout[k].iLeftTopX,g_atDirAndFileLayout[k].iLeftTopY);
		}
		
		iIconLeftTopX = ptInterfaceLayout->iLeftTopX + iDeltaX;
		iIconLeftTopY += DIR_FILE_ALL_HEIGHT + iDeltaY;		
	}

	g_atDirAndFileLayout[k].iLeftTopX  = 0;
	g_atDirAndFileLayout[k].iLeftTopY  = 0;
	g_atDirAndFileLayout[k].iRightBotX = 0;
	g_atDirAndFileLayout[k].iRightBotY = 0;
	g_atDirAndFileLayout[k].pcName = NULL;

debug("-----------------2.end Calc Browse Page Interface Layout----------------\n");   
	return 0;
}

/**********************************************************************
 * �������ƣ� GetInterfaceIconsPixel
 * ���������� Ϊ"���ҳ��"���ɲ˵������е�ͼ��
 * ��������� ptPageLayout - �ں��ǲ˵�������ͼ����ļ�������ʾ����
 * ��������� ��
 * �� �� ֵ��0 - �ɹ�
 *           ����ֵ - ʧ��
 ***********************************************************************/
static int GetInterfaceIconsPixel(PT_PageLayout ptPageLayout)
{
	int iXres, iYres, iBpp;
	T_PixelDatas tOriginalIconPixel;

	GetDefDispResolution(&iXres, &iYres,&iBpp);

	tOriginalIconPixel.ibpp = iBpp;
	
	g_tDirClosedIconPixel.ibpp 	= iBpp;
	g_tDirOpenedIconPixel.ibpp 	= iBpp;
	g_tFileIconPixel.ibpp		= iBpp;

	g_tDirClosedIconPixel.iHeight	= DIR_FILE_ICON_HEIGHT;
	g_tDirOpenedIconPixel.iHeight 	= DIR_FILE_ICON_HEIGHT;
	g_tFileIconPixel.iHeight		= DIR_FILE_ICON_HEIGHT;

	g_tDirClosedIconPixel.iWidth	= DIR_FILE_ICON_WIDTH;
	g_tDirOpenedIconPixel.iWidth	= DIR_FILE_ICON_WIDTH;
	g_tFileIconPixel.iWidth			= DIR_FILE_ICON_WIDTH;

	g_tDirClosedIconPixel.iLineByte = g_tDirClosedIconPixel.iWidth * iBpp / 8;
	g_tDirOpenedIconPixel.iLineByte = g_tDirOpenedIconPixel.iWidth *  iBpp / 8;;
	g_tFileIconPixel.iLineByte		= g_tFileIconPixel.iWidth *  iBpp / 8;

	g_tDirClosedIconPixel.iTotalByte = g_tDirClosedIconPixel.iLineByte * g_tDirClosedIconPixel.iHeight;
	g_tDirOpenedIconPixel.iTotalByte = g_tDirOpenedIconPixel.iLineByte * g_tDirOpenedIconPixel.iHeight;
	g_tFileIconPixel.iTotalByte 	 = g_tFileIconPixel.iLineByte * g_tFileIconPixel.iHeight;

	g_tDirClosedIconPixel.pucPixelDatas = malloc(ptPageLayout->iMaxTotalByte);
	g_tDirOpenedIconPixel.pucPixelDatas = malloc(ptPageLayout->iMaxTotalByte);
	g_tFileIconPixel.pucPixelDatas	    = malloc(ptPageLayout->iMaxTotalByte);
	
	if (NULL == g_tDirClosedIconPixel.pucPixelDatas)
	{
		debug ("malloc error");
		return -1;
	}
	if (NULL == g_tDirOpenedIconPixel.pucPixelDatas)
	{
		debug ("malloc error");
		return -1;
	}
	if (NULL == g_tFileIconPixel.pucPixelDatas)
	{
		debug ("malloc error");
		return -1;
	}

debug("-------------------------------GetPixelFrm ClosedIcon\n");
	GetPixelFrmIcon(g_strDirClosedIcon, &tOriginalIconPixel);
debug("---------------PicZoom ClosedIcon\n");
	PicZoom(&tOriginalIconPixel, &g_tDirClosedIconPixel);
    //FreePixelFrmIcon(&tOriginalIconPixel);
debug("-------------------------------GetPixelFrm OpenedIcon\n");
	GetPixelFrmIcon(g_strDirOpenedIcon, &tOriginalIconPixel);
debug("---------------PicZoom OpenedIcon\n");
	PicZoom(&tOriginalIconPixel, &g_tDirOpenedIconPixel);
    //FreePixelFrmIcon(&tOriginalIconPixel);
debug("-------------------------------GetPixelFrm FileIcon\n");
	GetPixelFrmIcon(g_strFileIcon, &tOriginalIconPixel);
debug("--------------PicZoom FileIcon\n");
	PicZoom(&tOriginalIconPixel, &g_tFileIconPixel);
    FreePixelFrmIcon(&tOriginalIconPixel);
		
	return 0;
}

/**********************************************************************
 * �������ƣ� GeneBPDirAndFile
 * ���������� Ϊ"���ҳ��"����"Ŀ¼���ļ�"�����е�ͼ�������,������ʾĿ¼����
 * ��������� iStartIndex        - ����Ļ����ʾ�ĵ�1��"Ŀ¼���ļ�"��aptDirContents���������һ��
 *            iDirContentsNumber - aptDirContents�����ж�����
 *            aptDirContents     - ����:����Ŀ¼��"������Ŀ¼","�ļ�"������ 
 *            ptVideoMem         - �����VideoMem�й���ҳ��
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
static int GeneBPDirAndFile(PT_PageLayout ptPageLayout, int iStartIndex, int iDirContentsNumber,
							PT_DirContent *aptDirContents, PT_VideoMem ptVideoMem)
{
	int i, j, k = 0;
	int iDirContentIndex = iStartIndex;
	PT_IconLayout atLayout = ptPageLayout->atIconLayout;
	/*����ʾ�����������*/
	ClearRegionVideoMem(ptPageLayout->iLeftTopX, ptPageLayout->iLeftTopY, 
	 				     ptPageLayout->iRightBotX, ptPageLayout->iRightBotY,
	 					 ptVideoMem, COLOR_BACKGROUND);
	 //SetFontSize(atLayout[1].iBotRightY - atLayout[1].iTopLeftY - 5);
debug("g_iNumPerCol = %d \n", g_iNumPerCol);	 
debug("g_iNumPerRow = %d \n", g_iNumPerRow);
	for (i = 0; i < g_iNumPerCol; i++)
    {
        for (j = 0; j < g_iNumPerRow; j++)
        {
            if (iDirContentIndex < iDirContentsNumber)
            {
                /* ��ʾĿ¼���ļ���ͼ�� */
                if (aptDirContents[iDirContentIndex]->eFileType == FILETYPE_DIR)
                {
					debug("atLayout[%d].iLeftTopX,iLeftTopY =(%d, %d) \n", k, atLayout[k].iLeftTopX, atLayout[k].iLeftTopY);
                    PicMerge(atLayout[k].iLeftTopX, atLayout[k].iLeftTopY, &g_tDirClosedIconPixel, &ptVideoMem->tPixelDatas);
					
                }
                else
                {
					debug("atLayout[%d].iLeftTopX,iLeftTopY =(%d, %d) \n", k, atLayout[k].iLeftTopX, atLayout[k].iLeftTopY);
                    PicMerge(atLayout[k].iLeftTopX, atLayout[k].iLeftTopY, &g_tFileIconPixel, &ptVideoMem->tPixelDatas);
                }

                k++;
                /* ��ʾĿ¼���ļ������� */
               // iError = MergerStringToCenterOfRectangleInVideoMem(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, atLayout[k].iBotRightX, atLayout[k].iBotRightY, (unsigned char *)aptDirContents[iDirContentIndex]->strName, ptVideoMem);
                k++;

                iDirContentIndex++;
            }
            else
            {
                break;
            }
        }
debug("(iDirContentIndex, iDirContentsNumber) = (%d,%d)\n", iDirContentIndex, iDirContentsNumber);
        if (iDirContentIndex >= iDirContentsNumber)
        {
            break;
        }
    }
	return 0;
}

/**********************************************************************
 * �������ƣ� BrowsePageShow
 * ���������� "���ҳ��"����ʾ����
 * ��������� ptPageLayout - ҳ�� �˵����� ͼ�꼰λ��
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
static void BrowsePageShow(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout;	/*ָ��4���˵�ͼ��*/
	T_PageLayout tInterfaceLayout = {						/*�ǲ˵�����λ����Ϣ*/
		.iMaxTotalByte = 0,
	};								
	int iError;
/*����Դ�*/
	ptVideoMem = GetVideoMem(GetPageId("browespage"), 1);
	if (ptVideoMem == NULL)
	{
		debug("can't get video mem for browse page!\n");
		return;
	}
/*����˵�ͼ��������ļ����ļ���ͼ��λ��*/
	if (atIconLayout[0].iLeftTopX == 0)
	{
		CalcBPMenuLayout(ptPageLayout);
		CalcBPInterfaceLayout(&tInterfaceLayout);
	}
/*��ý����ļ����ļ���ͼ����������*/    
	if (!g_tDirClosedIconPixel.pucPixelDatas)
    {
        GetInterfaceIconsPixel(&tInterfaceLayout);
    }
/* 3.����ͼ������(���ͼ�����ء��������š��ϲ��Ȳ���)*/
	iError = GeneratePage(ptPageLayout, ptVideoMem);
	iError |= GeneBPDirAndFile(&tInterfaceLayout, g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptVideoMem);
	if(iError)
	{
		debug("GeneratePage or DirAndFile failed\n");
		return ;
	}
/* 4.ˢ���豸�Ͻ�����ʾ*/
	FlushVideoMemToDev(ptVideoMem);

}

/**********************************************************************
 * �������ƣ� BrowsePageGetInputEvent
 * ���������� Ϊ"���ҳ��"�����������,�ж������¼�λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(PageLayout->atLayout�������һ��)
 ***********************************************************************/
static int BrowsePageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * �������ƣ� BrowsePageRun
 * ���������� "���ҳ��"�����к���: ��ʾ·���е�Ŀ¼���ļ�
 * ��������� ptParentPageParams - ҳ����Ϣ
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
static int BrowsePageRun(PT_PageParams ptParentPageParams)
{
	int iIndex;
	int iError;
	T_InputEvent tInputEvent;
	T_PageParams tPageParams;
	PT_DirContent aptDirContents;

	tPageParams.iPageID = GetPageId("browsepage");
	/*1.����ļ������ݴ洢�� g_aptDirContents �ṹ����*/
	iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
	if (iError)
	{
		debug("GetDirContents error\n");
		return -1;
	}

	/*2.��ʾ���ҳ�档2.1�˵�ͼ�������ȡ��2.2�ļ�ͼ�������ȡ��2.3����ͼ������*/
	BrowsePageShow(&g_tBPMenuPageLayout);

	/* 3.��������¼�*/
	while (1)
	{
		/*��ô�����������ͼ���±�*/
		iIndex = GenericGetInputEvent(&g_tBPMenuPageLayout, &tInputEvent);
		switch (iIndex)
		{
			case 0:			/*����*/
			{
				debug("you press the first button\n");
				 if (0 == strcmp(g_strCurDir, "/"))  /* �Ѿ��Ƕ���Ŀ¼ */
                     {
                         FreeDirContents(g_aptDirContents, g_iDirContentsNumber);
                         return ;
                     }
			}
			case 1:			/*ѡ��Ŀ¼*/
			{
				debug("you press the second button\n");
				return 0;
			}
			case 2:			/*��һҳ*/
			{
				debug("you press the third button\n");
				return 0;
			}
			case 3:			/*��һҳ*/
			{
				debug("you press the forth button\n");
				return 0;
			}
		}
	}
}


T_PageAction g_tBPAction = {
	.name 			= "browsepage",
	.Run 			= BrowsePageRun,
};

/**********************************************************************
 * �������ƣ� MainPageInit
 * ���������� ע��"��ҳ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 ***********************************************************************/
int BrowsePageInit(void)
{
	return PageActionRegister(&g_tBPAction);
}






