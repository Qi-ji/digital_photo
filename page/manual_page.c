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
static T_IconLayout g_tPicShowLayout; /*照片显示位置*/
	
static T_PixelDatas g_tOriginalPixelDatas;
static T_PixelDatas g_tZoomPixelDatas;


/**********************************************************************
 * 函数名称： CalcMPPicLayout
 * 功能描述： 计算"手动页面"的照片显示区域的位置信息
 * 输入参数： 
 * 输出参数： ptPicShowLayout - 页面信息,计算返回页面功能菜单位置信息
 * 返 回 值： 
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
 * 函数名称： CalcBPMenuLayout
 * 功能描述： 计算"手动页面"的功能菜单位置信息
 * 输入参数： ptPageLayout - 页面信息,含页面功能菜单信息
 * 输出参数： ptPageLayout - 页面信息,计算返回页面功能菜单位置信息
 * 返 回 值： 
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
 * 函数名称： GetOriginalPixel
 * 功能描述： 在"手动页面"上获得要显示照片的原始数据
 * 输入参数： strPicName - 要显示的照片的名字
 *			  
 * 输出参数： 
 * 返 回 值： g_tOriginalPixelDatas - 使用到全局变量来存放照片原始数据
 *            NULL - 失败
 ***********************************************************************/
static PT_PixelDatas GetOriginalPixel(char *strPicName)
{
	T_FileMap tFileMap;
	int iXres, iYres, iBpp;
	int iError;
	PT_PicFilePraser Praser;

	if (g_tOriginalPixelDatas.pucPixelDatas)	/*使用之前先对g_tOriginalPixelDatas.pucPixelDatas进行malloc的，在getpicel函数中会进行malloc*/
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
	
	UnMapFile(&tFileMap);/*注意在使用结束之后要Unmapfile*/
	return &g_tOriginalPixelDatas;
}

/**********************************************************************
 * 函数名称： GetZoomPixel
 * 功能描述： 在"手动页面"上获得要显示照片的缩放后的数据
 * 输入参数： ptOriginalPixelDatas - 要显示照片的原始数据
 *			  iZoomHeight - 缩放后的高度
 *			  iZoomWidth - 缩放后的宽度
 * 输出参数： 
 * 返 回 值： g_tZoomPixelDatas - 使用到全局变量来存放照片缩放后的数据
 *            NULL - 失败
 ***********************************************************************/
static PT_PixelDatas GetZoomPixel(PT_PixelDatas ptOriginalPixelDatas, int iZoomHeight,int iZoomWidth)
{

	int iXres, iYres, iBpp;
	float k;
	int iError;
	
/*1. 获得缩小后照片的长宽等信息*/	
	/*保证照片的长宽比不变*/
	k = (float)ptOriginalPixelDatas->iWidth / ptOriginalPixelDatas->iHeight;
	
	GetDefDispResolution(&iXres, &iYres, &iBpp);
	g_tZoomPixelDatas.ibpp = iBpp;
	g_tZoomPixelDatas.iHeight = iZoomHeight;
	g_tZoomPixelDatas.iWidth = iZoomHeight * k;
	
	/*看长跟宽哪个更小，以更小的那个为标准*/
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
 * 函数名称： ShowPicture
 * 功能描述： 在"手动页面"上显示选择的照片
 * 输入参数： g_tPicShowLayout - 内含图片的显示区域
 *			  ptVideoMem - 显存
 *			  strPicName - 要显示的照片的名字
 * 输出参数： 
 * 返 回 值： 0 - 成功
 *            其他值 - 失败
 ***********************************************************************/
static int ShowPicture(PT_VideoMem ptVideoMem, char *strPicName)
{
	int iPictureWidth, iPictureHeight;		/*照片显示区域的大小表示*/
	int iPicLeftTopX, iPicLeftTopY;			/*照片合并时的合并位置*/
	PT_PixelDatas ptOriginalPixel, ptZoomPixel;
	
	/*1. 获得照片的原始数据*/
	ptOriginalPixel = GetOriginalPixel(strPicName);		/*g_tOriginalPixelDatas - 使用到全局变量来存放照片原始数据*/
	debug("Have got [%s] pixel datas.\n", strPicName);
	
	/*2. 获得照片缩放后的大小，以及缩放后的数据*/
	iPictureWidth = g_tPicShowLayout.iRightBotX - g_tPicShowLayout.iLeftTopX + 1;
	iPictureHeight = g_tPicShowLayout.iRightBotY - g_tPicShowLayout.iLeftTopY + 1;

	ptZoomPixel = GetZoomPixel(&g_tOriginalPixelDatas, iPictureHeight, iPictureWidth);
	if (NULL == ptZoomPixel)
	{
		debug("fale: can not get [%s] zoom pixel\n", strPicName);
		return -1;
	}
	debug("Have got Zoomed pixel datas.\n");
	/*3. 获得照片缩放后的大小及合并到内存上时的位置*/
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
 * 函数名称： ManualPageGetInputEvent
 * 功能描述： 为"手动页面"获得输入数据,判断输入事件位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 ***********************************************************************/
static int ManualPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * 函数名称： ManualPageShow
 * 功能描述： "手动页面"的显示函数，显示菜单栏，以及要显示的照片
 * 输入参数： ptPageLayout - 页面 菜单区域 图标及位置
 *			  ptParentPageParams - 上一级页面的ID以及要显示的照片的路径信息
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static void ManualPageShow(PT_PageLayout ptPageLayout, PT_PageParams ptParentPageParams)
{
	PT_VideoMem ptVideoMem; 
	PT_IconLayout ptIconLayout = ptPageLayout->atIconLayout; /*Menu图标显示位置*/
	int iError;
	
	/*1.获得显存*/
	ptVideoMem = GetVideoMem(GetPageId("manual"), 1);
	if (NULL == ptVideoMem)
		return;
	
	/*2. 获得Menu图标坐标位置以及照片显示区域位置*/
	if (g_atMPIconLayou[0].iLeftTopX == 0)
	{
	    CalcMPMenuLayout(ptPageLayout);
		CalcMPPicLayout();					/*显示照片区域的位置*/
	}
	/*3. 获得Menu图标与照片数据*/
	iError  = GeneratePage(ptPageLayout, ptVideoMem);
	iError |= ShowPicture(ptVideoMem, ptParentPageParams->strCurPictureFile);
	if (iError)
		return;

	/*4. 刷到设备上*/
	FlushVideoMemToDev(ptVideoMem);

}

/**********************************************************************
 * 函数名称： ManualPageRun
 * 功能描述： "手动页面"的运行函数: 
 * 输入参数： ptParentPageParams - 跳转过来页面的信息
 * 输出参数： 无
 * 返 回 值： 无
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

	/*1. 显示页面*/
	ManualPageShow(&g_tMPMenulayout, ptParentPageParams);
	/*2. 对照片序号、文件目录进行处理，方便翻页*/

	/* 取出目录名 */
    strcpy(strDirName, ptParentPageParams->strCurPictureFile);
    pcTmp = strrchr(strDirName, '/');
    *pcTmp = '\0';	
	    /* 取出文件名 */
    strcpy(strFileName, pcTmp+1);

    /* 获得当前目录下所有目录和文件的名字 */
    iError = GetDirContents(strDirName, &aptDirContents, &iDirContentsNumber);

    /* 确定当前显示的是哪一个文件 */
    for (iPicFileIndex = 0; iPicFileIndex < iDirContentsNumber; iPicFileIndex++)
    {
        if (0 == strcmp(strFileName, aptDirContents[iPicFileIndex]->strName))
        {
            break;
        }
    }
	
	/*3. 获得输入事件*/
	while(1)
	{
		/*获得触摸屏触摸的Menu区域图标下标，tInputEvent 中通过函数获得触摸屏坐标*/
		iIndex = GenericGetInputEvent(&g_tMPMenulayout, &tInputEvent);
		if (tInputEvent.iPressure == 0)/*按键是松开状态*/
		{
			switch (iIndex)
			{	
				case 0: 		/*返回*/
				{
					return;
					break;
				}
				case 1: 		/*缩小*/
				{
					
					break;
				}
				case 2: 		/*放大*/
				{
					
					break;
				}
				case 3: 		/*上一张*/
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
				case 4: 		/*下一张*/
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
				case 5: 		/*连播 */
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
 * 函数名称： ManualPageInit
 * 功能描述： 注册"手动页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期 	   版本号	 修改人		  修改内容
 ***********************************************************************/
int ManualPageInit(void)
{
	return PageActionRegister(&g_tMPAction);
}


