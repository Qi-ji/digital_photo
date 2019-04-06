#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <disp_manager.h>
#include <page_manager.h>
#include <render.h>
#include <file.h>
#include <test.h>

/* 图标是一个正方体, "图标+名字"也是一个正方体
 *   --------
 *   |  图  |
 *   |  标  |
 * ------------
 * |   名字   |
 * ------------
 */
#define DIR_FILE_ICON_WIDTH    80
#define DIR_FILE_ICON_HEIGHT   DIR_FILE_ICON_WIDTH
#define DIR_FILE_NAME_HEIGHT   40
#define DIR_FILE_NAME_WIDTH   (DIR_FILE_ICON_HEIGHT + DIR_FILE_NAME_HEIGHT)
#define DIR_FILE_ALL_WIDTH    DIR_FILE_NAME_WIDTH
#define DIR_FILE_ALL_HEIGHT   DIR_FILE_ALL_WIDTH

#define DIR_FILT_MIN_INDEX   1000 /*目录及文件的下标是由1000开始计算的*/
static T_IconLayout g_atMenuIconLayout[] = { /*菜单区域位置信息*/
	{0,0,0,0,"up.bmp"},
	{0,0,0,0,"select.bmp"},
	{0,0,0,0,"pre_page.bmp"},
	{0,0,0,0,"next_page.bmp"},
	{0,0,0,0,NULL},
};

static T_PageLayout g_tBPMenuPageLayout = {		/*页面菜单区域图标及位置*/
	.iMaxTotalByte = 0,
	.atIconLayout = g_atMenuIconLayout,
};

static char g_strCurDir[256] = DEFAULT_DIR;
//static char g_strSelectedDir[256] = DEFAULT_DIR;

/* 用来描述某目录里的内容 */
static PT_DirContent *g_aptDirContents;  		/* 数组:存有目录下"顶层子目录","文件"的名字 */
static int g_iDirContentsNumber;         		/* g_aptDirContents数组有多少项 */
T_PageLayout g_tInterfaceLayout = { 					/*非菜单区域位置信息*/
	.iMaxTotalByte = 0,
};	
static T_IconLayout *g_atDirAndFileLayout = NULL; /*文件夹及文件位置信息*/
static char *g_strDirClosedIcon  = "fold_closed.bmp";
static char *g_strDirOpenedIcon  = "fold_opened.bmp";
static char *g_strFileIcon = "file.bmp";
//static int g_iStartIndex = 0;            /* 在屏幕上显示的第1个"目录和文件"是g_aptDirContents数组里的哪一项 */


static T_PixelDatas g_tDirClosedIconPixel;
static T_PixelDatas g_tDirOpenedIconPixel;
static T_PixelDatas g_tFileIconPixel;

static int g_iNumPerRow, g_iNumPerCol;
static int g_iStartIndex = 0;			/*记录要打开的文件夹及文件的下标，为翻页情况准备*/




/**********************************************************************
 * 函数名称： CalcBPMenuLayout
 * 功能描述： 计算"浏览页面"的功能菜单位置信息
 * 输入参数： ptPageLayout - 页面信息,含页面功能菜单信息
 * 输出参数： ptPageLayout - 页面信息,计算返回页面功能菜单位置信息
 * 返 回 值： 
 ***********************************************************************/
static int CalcBPMenuLayout(PT_PageLayout ptPageLayout)
{
	if (NULL == ptPageLayout || NULL == ptPageLayout->atIconLayout)
		return -1;
debug("-----------------1.start Calc Browse Page Menu Layout----------------\n");
	int iXres, iYres, iBpp;  /*显示设备的参数*/
	int iIconWidth, iIconHeight; /*每个图标的宽、高都是一样的，所以计算第一个图标就可以了*/
	int iIconTotalByte;			/*每个图标的总大小都是一样的*/
	PT_IconLayout atIconLayout;
	int i = 0;

	GetDefDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;
	atIconLayout = ptPageLayout->atIconLayout;

	/*up.bmp 图标位置计算*/
	atIconLayout[0].iLeftTopX = 0;
	atIconLayout[0].iLeftTopY = 0;
	atIconLayout[0].iRightBotX = iXres * 1/4;
	atIconLayout[0].iRightBotY = iYres * 1/4;

	/*select.bmp 图标位置计算*/
	atIconLayout[1].iLeftTopX = iXres * 0;
	atIconLayout[1].iLeftTopY = iYres * 1/4 + 1;
	atIconLayout[1].iRightBotX = iXres * 1/4;
	atIconLayout[1].iRightBotY = iYres * 2/4;
	
	/*pre_page.bmp 图标位置计算*/
	atIconLayout[2].iLeftTopX = iXres * 0;
	atIconLayout[2].iLeftTopY = iYres * 2/4 + 1;
	atIconLayout[2].iRightBotX = iXres * 1/4;
	atIconLayout[2].iRightBotY = iYres * 3/4;

	/*snext_page.bmp 图标位置计算*/
	atIconLayout[3].iLeftTopX = iXres * 0;
	atIconLayout[3].iLeftTopY = iYres * 3/4 + 1;
	atIconLayout[3].iRightBotX = iXres * 1/4;
	atIconLayout[3].iRightBotY = iYres * 1;

	for(i=0; i<4; i++)
	{
		iIconWidth = atIconLayout[i].iRightBotX - atIconLayout[i].iLeftTopX +1; /*注意这里计算是要加1 的*/
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
 * 函数名称： CalcBPInterfaceLayout
 * 功能描述： 计算"浏览页面"的界面图标位置信息，即各个文件夹以及文件图标的信息
 *		 通过全局变量来传递函数
 * 输入参数： 无
 * 输出参数： ptInterfaceLayout - 含有非菜单区域的位置信息,并指向了各个图标的坐标位置。
 * 返 回 值： PT_IconLayout aptIconLayout - 数组中含有各个图标及名称的位置信息
 ***********************************************************************/
static int CalcBPInterfaceLayout(PT_PageLayout ptInterfaceLayout)
{
	int iXres, iYres, iBpp;
	int iIconWidth, iIconHeight;			/*每个图标的长宽*/
	int iNumPerRow, iNumPerCol;			/* 每行每列图标的个数*/
	int iInterFaceWidth, iInterFaceHeight;/*非菜单界面的宽高*/
	int iDeltaX, iDeltaY;				/*每个图标之间的间距*/
	int iIconLeftTopX, iIconLeftTopY; /*文件夹图标显示的开始位置坐标*/
	int i ,j, k = 0;
	//PT_PageLayout ptInterfaceLayout; /*含有非菜单区域的显示信息*/
debug("-----------------2.start Calc Browse Page Interface Layout----------------\n");

/* 1.获得非菜单显示区域的位置信息，即‘文件夹及文件的显示区域’*/
	GetDefDispResolution(&iXres, &iYres,&iBpp);
	ptInterfaceLayout->iLeftTopX 	 = g_atMenuIconLayout[0].iRightBotX + 1;
	ptInterfaceLayout->iLeftTopY 	 = 0;
	ptInterfaceLayout->iRightBotX	 = iXres - 1;
	ptInterfaceLayout->iRightBotY 	 = iYres - 1;
	ptInterfaceLayout->iBpp 		 = iBpp;
	ptInterfaceLayout->iMaxTotalByte = DIR_FILE_ALL_HEIGHT * DIR_FILE_ALL_WIDTH * iBpp / 8;
/*	
debug("非菜单显示区域坐标ptInterfaceLayout->iLeftTopX = %d\n", ptInterfaceLayout->iLeftTopX);
debug("ptInterfaceLayout->iLeftTopY = %d\n", ptInterfaceLayout->iLeftTopY);
debug("ptInterfaceLayout->iRightBotX = %d\n", ptInterfaceLayout->iRightBotX);
debug("ptInterfaceLayout->iRightBotY = %d\n", ptInterfaceLayout->iRightBotY);
*/
/* 2.获得各文件图标的显示坐标 */
    iIconWidth  = DIR_FILE_ALL_WIDTH;		
    iIconHeight = iIconWidth;	
	/* 2.1确定每行每列可以显示多少个图标*/
	iInterFaceWidth = ptInterfaceLayout->iRightBotX - ptInterfaceLayout->iLeftTopX + 1;
	iNumPerRow = iInterFaceWidth / iIconWidth;
	 /*确保每个图标之间的间隔大于10个像素*/
	while (1)
	{
		iDeltaX = iInterFaceWidth - iNumPerRow * iIconWidth;/*总的空隙长度*/
		if (iDeltaX / (iNumPerRow + 1) < 10)
		{
			iNumPerRow--;
		}
		else 
			break;
	}
	g_iNumPerRow = iNumPerRow;
	iDeltaX = iDeltaX / (iNumPerRow + 1);/*每行每个图标之间的间距*/

	iInterFaceHeight = ptInterfaceLayout->iRightBotY- ptInterfaceLayout->iLeftTopY+ 1;
	iNumPerCol= iInterFaceHeight / iIconHeight;
	 /*确保每个图标之间的间隔大于10个像素*/
	while (1)
	{
		iDeltaY = iInterFaceHeight - iNumPerCol* iIconHeight;/*总的空隙长度*/
		if (iDeltaY / (iNumPerCol+ 1) < 10)
		{
			iNumPerCol--;
		}
		else 
			break;
	}
	g_iNumPerCol = iNumPerCol;
	iDeltaY = iDeltaY / (iNumPerCol+ 1);/*每列每个图标之间的间距*/
   /* 可以显示 iNumPerRow * iNumPerCol个"目录或文件"
     * 分配"两倍+1"的T_Layout结构体: 一个用来表示图标,另一个用来表示名字
     * 最后一个用来存NULL,借以判断结构体数组的末尾
     */
	g_atDirAndFileLayout = malloc(sizeof(T_IconLayout) * (2 * iNumPerRow *iNumPerCol + 1));
   if (NULL == g_atDirAndFileLayout)
   {
	   debug("malloc error!\n");
	   return -1;
   }
   ptInterfaceLayout->atIconLayout  = g_atDirAndFileLayout;
    /* 确定图标和名字的位置 
     *
     * 图标是一个正方体, "图标+名字"也是一个正方体
     *   --------
     *   |  图  |
     *   |  标  |
     * ------------
     * |   名字   |
     * ------------
     */
	iIconLeftTopX = ptInterfaceLayout->iLeftTopX + iDeltaX;
	iIconLeftTopY = ptInterfaceLayout->iLeftTopY + iDeltaY;

	/* k 表示是图标的坐标地址， k + 1 表示是名字的坐标地址*/
	for (i=0; i<iNumPerCol; i++)
	{
		for(j=0; j<iNumPerRow; j++)
		{
			/* 图标 */
			g_atDirAndFileLayout[k].iLeftTopX  = iIconLeftTopX + (DIR_FILE_NAME_WIDTH - DIR_FILE_ICON_WIDTH) / 2;
			g_atDirAndFileLayout[k].iLeftTopY  = iIconLeftTopY;
			g_atDirAndFileLayout[k].iRightBotX = g_atDirAndFileLayout[k].iLeftTopX + DIR_FILE_ICON_WIDTH - 1;
			g_atDirAndFileLayout[k].iRightBotY = g_atDirAndFileLayout[k].iLeftTopY + DIR_FILE_ICON_HEIGHT - 1;
//debug("g_atDirAndFileLayout[%d].iLeftTopX,iLeftTopY = (%d,%d)\n", k,g_atDirAndFileLayout[k].iLeftTopX, g_atDirAndFileLayout[k].iLeftTopY);

			g_atDirAndFileLayout[k+1].iLeftTopX  = iIconLeftTopX;
			g_atDirAndFileLayout[k+1].iLeftTopY  = iIconLeftTopY + DIR_FILE_ICON_HEIGHT + 1;
			g_atDirAndFileLayout[k+1].iRightBotX = iIconLeftTopX + DIR_FILE_NAME_WIDTH - 1;
			g_atDirAndFileLayout[k+1].iRightBotY = iIconLeftTopY + DIR_FILE_ALL_HEIGHT - 1;
//debug("g_atDirAndFileLayout[%d].iLeftTopX,iLeftTopY = (%d,%d)\n", k+1,g_atDirAndFileLayout[k+1].iLeftTopX, g_atDirAndFileLayout[k+1].iLeftTopY);

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
 * 函数名称： GetInterfaceIconsPixel
 * 功能描述： 为"浏览页面"生成菜单区域中的图标
 * 输入参数： ptPageLayout - 内含非菜单区域多个图标的文件名和显示区域
 * 输出参数： 无
 * 返 回 值：0 - 成功
 *           其他值 - 失败
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
 * 函数名称： GeneBPDirAndFile
 * 功能描述： 为"浏览页面"生成"目录或文件"区域中的图标和文字,就是显示目录内容
 * 输入参数： iStartIndex        - 在屏幕上显示的第1个"目录和文件"是aptDirContents数组里的哪一项
 *            iDirContentsNumber - aptDirContents数组有多少项
 *            aptDirContents     - 数组:存有目录下"顶层子目录","文件"的名字 
 *            ptVideoMem         - 在这个VideoMem中构造页面
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
static int GeneBPDirAndFile(PT_PageLayout ptPageLayout, int iStartIndex, int iDirContentsNumber,
							PT_DirContent *aptDirContents, PT_VideoMem ptVideoMem)
{
	int i, j, k = 0;
	int iDirContentIndex = iStartIndex;
	PT_IconLayout atLayout = ptPageLayout->atIconLayout;
	/*将显示区域进行清屏*/
	ClearRegionVideoMem(ptPageLayout->iLeftTopX, ptPageLayout->iLeftTopY, 
	 				     ptPageLayout->iRightBotX, ptPageLayout->iRightBotY,
	 					 ptVideoMem, COLOR_BACKGROUND);
	 //SetFontSize(atLayout[1].iBotRightY - atLayout[1].iTopLeftY - 5);
//debug("g_iNumPerCol = %d \n", g_iNumPerCol);	 
//debug("g_iNumPerRow = %d \n", g_iNumPerRow);
	for (i = 0; i < g_iNumPerCol; i++)
    {
        for (j = 0; j < g_iNumPerRow; j++)
        {
            if (iDirContentIndex < iDirContentsNumber)
            {
                /* 显示目录或文件的图标 */
                if (aptDirContents[iDirContentIndex]->eFileType == FILETYPE_DIR)
                {
					//debug("atLayout[%d].iLeftTopX,iLeftTopY =(%d, %d) \n", k, atLayout[k].iLeftTopX, atLayout[k].iLeftTopY);
                    PicMerge(atLayout[k].iLeftTopX, atLayout[k].iLeftTopY, &g_tDirClosedIconPixel, &ptVideoMem->tPixelDatas);
					
                }
                else
                {
					//debug("atLayout[%d].iLeftTopX,iLeftTopY =(%d, %d) \n", k, atLayout[k].iLeftTopX, atLayout[k].iLeftTopY);
                    PicMerge(atLayout[k].iLeftTopX, atLayout[k].iLeftTopY, &g_tFileIconPixel, &ptVideoMem->tPixelDatas);
                }

                k++;
                /* 显示目录或文件的名字 */
               // iError = MergerStringToCenterOfRectangleInVideoMem(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, atLayout[k].iBotRightX, atLayout[k].iBotRightY, (unsigned char *)aptDirContents[iDirContentIndex]->strName, ptVideoMem);
                k++;

                iDirContentIndex++;
            }
            else
            {
                break;
            }
        }
//debug("(iDirContentIndex, iDirContentsNumber) = (%d,%d)\n", iDirContentIndex, iDirContentsNumber);
        if (iDirContentIndex >= iDirContentsNumber)
        {
            break;
        }
    }
	return 0;
}

/**********************************************************************
 * 函数名称： BrowsePageShow
 * 功能描述： "浏览页面"的显示函数
 * 输入参数： ptPageLayout - 页面 菜单区域 图标及位置
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static void BrowsePageShow(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout;	/*指向4个菜单图标*/
							
	int iError;
/*获得显存*/
	ptVideoMem = GetVideoMem(GetPageId("browespage"), 1);
	if (ptVideoMem == NULL)
	{
		debug("can't get video mem for browse page!\n");
		return;
	}
/*计算菜单图标与界面文件与文件夹图标位置*/
	if (atIconLayout[0].iLeftTopX == 0)
	{
		CalcBPMenuLayout(ptPageLayout);
		CalcBPInterfaceLayout(&g_tInterfaceLayout);
	}
/*获得界面文件与文件夹图标像素数据*/    
	if (!g_tDirClosedIconPixel.pucPixelDatas)
    {
        GetInterfaceIconsPixel(&g_tInterfaceLayout);
    }
/* 3.生成图标数据(获得图标像素、进行缩放、合并等操作)*/
	iError = GeneratePage(ptPageLayout, ptVideoMem);
	iError |= GeneBPDirAndFile(&g_tInterfaceLayout, g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptVideoMem);
	if(iError)
	{
		debug("GeneratePage or DirAndFile failed\n");
		return ;
	}
/* 4.刷到设备上进行显示*/
	FlushVideoMemToDev(ptVideoMem);

}

/**********************************************************************
 * 函数名称： BrowsePageGetInputEvent
 * 功能描述： 为"浏览页面"获得输入数据,判断输入事件位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 ***********************************************************************/
static int BrowsePageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * 函数名称： FindFileIndex
 * 功能描述： 为"浏览页面"获得输入数据,判断输入事件位于哪一个"目录或文件"上
 * 输入参数： ptInterfaceLayout - 内含"目录或文件"的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 ***********************************************************************/
static int FindFileIndex(PT_PageLayout ptInterfaceLayout, PT_InputEvent ptInputEvent)
{
	int i = 0;
	PT_IconLayout atIconLayout = ptInterfaceLayout->atIconLayout;
	
	/* 处理数据 */
	/* 确定触点位于哪一个按钮上 */
	//while (atIconLayout[i].pcName) /*并没有定义名字*/
	while (atIconLayout[i].iLeftTopX)
	{
		if ((ptInputEvent->iX >= atIconLayout[i].iLeftTopX) && (ptInputEvent->iX <= atIconLayout[i].iRightBotX) && \
			 (ptInputEvent->iY >= atIconLayout[i].iLeftTopY) && (ptInputEvent->iY <= atIconLayout[i].iRightBotY))
		{
			debug("the index is %d\n", i);
			/* 找到了被点中的按钮 */
			return i;  
		}
		else
		{
			i++;
		}			
	}

	/* 触点没有落在按钮上 */
	return -1;
}

/**********************************************************************
 * 函数名称： BrowsePageRun
 * 功能描述： "浏览页面"的运行函数: 显示路径中的目录及文件
 * 输入参数： ptParentPageParams - 页面信息
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static void BrowsePageRun(PT_PageParams ptParentPageParams)
{
	T_InputEvent tInputEvent;
	int iIndex,iPressIndex;
	int bPressed = 0;/* =0 为之前无按键被按下*/
	
	int iFileIndex;/*目录或者文件下表 0 1 2 3   ,目录中的第几个文件*/
	char strTmp[256]; /*临时放置点击的文件夹的路径名*/
	char *pcTmp;
	PT_DirContent aptDirContents;
	
	PT_VideoMem ptVideoMem;
	T_PageParams tPageParams;
	int iError;

	ptVideoMem = GetDevVideoMem();/*直接在显存上显示*/
	tPageParams.iPageID = GetPageId("browsepage");
	T_InputEvent tTempInputEvent;
	
	/*1.获得文件夹内容存储到 g_aptDirContents 结构体中*/
	iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
	if (iError)
	{
		debug("GetDirContents error\n");
		return ;
	}

	/*2.显示浏览页面。2.1菜单图标坐标获取，2.2文件图标坐标获取，2.3生成图标数据*/
	BrowsePageShow(&g_tBPMenuPageLayout);/*目录及文件图标通过全局变量的方式进行传递*/

	/* 3.获得输入事件*/
	while (1)
	{
		/*获得触摸屏触摸的Menu区域图标下标，tInputEvent 中通过函数获得触摸屏坐标*/
		iIndex = GenericGetInputEvent(&g_tBPMenuPageLayout, &tInputEvent);
		if (iIndex == -1)	/*说明不在Menu上面*/
		{
			iIndex = FindFileIndex(&g_tInterfaceLayout, &tInputEvent);
			if (iIndex != -1)
			{
				if (g_iStartIndex + iIndex /2 < g_iDirContentsNumber)/*点击的图片或者文字，都表示一个文件夹*/
				{
					iIndex += DIR_FILT_MIN_INDEX; /*目录及文件返回下标为1001,1002 ···*/
				}
				else 
					iIndex = -1;
			}

		}

		if (tInputEvent.iPressure == 0)/*按键是松开状态*/
		{
			
			if (iIndex < DIR_FILT_MIN_INDEX)/*点击的为menu区域*/
			{

				switch (iIndex)
				{	
					case 0: 		/*向上*/
					{
						if (0 == strcmp(g_strCurDir, DEFAULT_DIR))  /* 已经是顶层目录 */
						{
							FreeDirContents(g_aptDirContents, g_iDirContentsNumber);
							return 0;
						}
							 
						pcTmp = strrchr(g_strCurDir, '/'); /* 找到最后一个'/', 把它去掉 */
						*pcTmp = '\0';
				
						FreeDirContents(g_aptDirContents, g_iDirContentsNumber);
						iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
						if (iError)
						{
							debug("GetDirContents error!\n");
							return 0;
						}
						g_iStartIndex = 0;
						iError = GeneBPDirAndFile(&g_tInterfaceLayout, g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptVideoMem);
						break;
					}
					case 1: 		/*选择目录*/
					{
						debug("you press the second button\n");
						break;
					}
					case 2: 		/*上一页*/
					{
						debug("you press the third button\n");
						break;
					}
					case 3: 		/*下一页*/
					{
						debug("you press the forth button\n");
						break;
					}
				}
			}
			
			else 					/*点击的为Interface 目录与文件区域*/
			{
				/*单击目录则进入*/
				iFileIndex = g_iStartIndex + (iIndex - DIR_FILT_MIN_INDEX)/2;	/*除以2是因为下标包括图标跟名字，所以需要除以2才是真正要打开的文件*/
				if (g_aptDirContents[iFileIndex]->eFileType == FILETYPE_DIR)		/*是文件夹*/
				{
					 snprintf(strTmp, 256, "%s/%s", g_strCurDir, g_aptDirContents[iFileIndex]->strName);
					 strTmp[255] = '\0';
					 strcpy(g_strCurDir, strTmp);
					 FreeDirContents(g_aptDirContents, g_iDirContentsNumber);
					 iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
					 if (iError)
					 {
						 debug("GetDirContents error!\n");
						 return ;
					 }
					 g_iStartIndex = 0;
					GeneBPDirAndFile(&g_tInterfaceLayout, g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptVideoMem);
				}
				else if (g_aptDirContents[iFileIndex]->eFileType == FILETYPE_FILE)	/*是文件*/
				{
					snprintf(tPageParams.strCurPictureFile, 256, "%s/%s", g_strCurDir, g_aptDirContents[iFileIndex]->strName);
					tPageParams.strCurPictureFile[255] = '\0';

					//BPJpgTest(strTmp);/*测试是否可以显示*/
                    if (isPictureFileSupported(tPageParams.strCurPictureFile))
                    {
                        tPageParams.iPageID = GetPageId("browse");
                        GetPage("manual")->Run(&tPageParams);
                        BrowsePageShow(&g_tBPMenuPageLayout);
                    }					
					
				}
				
			}
				
		}
			
		else /*有按键按下*/
		{
			
			if (!bPressed)/*之前无按键被按下*/
			{
				bPressed = 1;
				iPressIndex = iIndex;
				if(iPressIndex < DIR_FILT_MIN_INDEX)		/*menu 区域*/
				{
					PressIcon(&g_atMenuIconLayout[iPressIndex]);
				}
			}
		}
		
	}
}


T_PageAction g_tBPAction = {
	.name 			= "browsepage",
	.Run 			= BrowsePageRun,
	.GetInputEvent  = BrowsePageGetInputEvent,
};

/**********************************************************************
 * 函数名称： MainPageInit
 * 功能描述： 注册"主页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 ***********************************************************************/
int BrowsePageInit(void)
{
	return PageActionRegister(&g_tBPAction);
}






