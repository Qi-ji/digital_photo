#include <stdio.h>

#include <disp_manager.h>
#include <page_manager.h>
#include <render.h>
#include <input_manager.h>


static T_IconLayout g_atMainPageIconLayout[] = {
	{0,0,0,0,"browse_mode.bmp"},
	{0,0,0,0,"continue_mode.bmp"},
	{0,0,0,0,"setting.bmp"},
	{0,0,0,0,NULL},
};

static T_PageLayout g_tPageLayout = {
	.iMaxTotalByte = 0,
	.atIconLayout = g_atMainPageIconLayout,
};


/**********************************************************************
 * 函数名称： CalcMainPageLayout
 * 功能描述： 计算首页面中各图标座标值
 * 输入参数： 无
 * 输出参数： ptPageLayout - 内含各图标的左上角/右下角座标值
 * 返 回 值： 0 - 成功  -1 - 失败
 ***********************************************************************/
static int  CalcMainPageLayout(PT_PageLayout ptPageLayout)
{
	if (NULL == ptPageLayout || NULL == ptPageLayout->atIconLayout)
		return -1;
	debug("***************************star CalcMainPage***************************\n");
	int iXres, iYres, iBpp;  /*显示设备的参数*/
	int iIconWidth, iIconHeight; /*每个图标的宽、高都是一样的，所以计算第一个图标就可以了*/
	int iIconTotalByte;			/*每个图标的总大小都是一样的*/
	PT_IconLayout atIconLayout;

	GetDefDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;
	atIconLayout = ptPageLayout->atIconLayout;

	/*browser_button.jpg 图标位置计算*/
	atIconLayout[0].iLeftTopX = iXres * 1/3;
	atIconLayout[0].iLeftTopY = iYres * 1/10;
	atIconLayout[0].iRightBotX = iXres * 2/3;
	atIconLayout[0].iRightBotY = iYres * 3/10;

		/* 计算该页面总Byte 数 */
	iIconWidth = atIconLayout[0].iRightBotX - atIconLayout[0].iLeftTopX +1; /*注意这里计算是要加1 的*/
	iIconHeight = atIconLayout[0].iRightBotY - atIconLayout[0].iLeftTopY +1;
	iIconTotalByte = iIconWidth * iIconHeight * iBpp / 8;

	if (ptPageLayout->iMaxTotalByte < iIconTotalByte)
	{
		ptPageLayout->iMaxTotalByte = iIconTotalByte;
	}
/*
	debug (" atIconLayout[0].iLeftTopX = %d \n.", atIconLayout[0].iLeftTopX);
	debug (" atIconLayout[0].iLeftTopY = %d \n.", atIconLayout[0].iLeftTopY);
	debug (" atIconLayout[0].iRightBotX = %d \n.", atIconLayout[0].iRightBotX);
	debug (" atIconLayout[0].iRightBotY = %d \n.", atIconLayout[0].iRightBotY);
	debug (" iIconTotalByte = %d \n.", iIconTotalByte);
*/
	/*continue_button.jpg 图标位置计算*/
	atIconLayout[1].iLeftTopX = iXres * 1/3;
	atIconLayout[1].iLeftTopY = iYres * 4/10;
	atIconLayout[1].iRightBotX = iXres * 2/3;
	atIconLayout[1].iRightBotY = iYres * 6/10;
	
	iIconWidth = atIconLayout[1].iRightBotX - atIconLayout[1].iLeftTopX +1; /*注意这里计算是要加1 的*/
	iIconHeight = atIconLayout[1].iRightBotY - atIconLayout[1].iLeftTopY +1;
	iIconTotalByte = iIconWidth * iIconHeight * iBpp / 8;
	if (ptPageLayout->iMaxTotalByte < iIconTotalByte)
	{
		ptPageLayout->iMaxTotalByte = iIconTotalByte;
	}
/*	
	debug (" atIconLayout[1].iLeftTopX = %d \n.", atIconLayout[1].iLeftTopX);
	debug (" atIconLayout[1].iLeftTopY = %d \n.", atIconLayout[1].iLeftTopY);
	debug (" atIconLayout[1].iRightBotX = %d \n.", atIconLayout[1].iRightBotX);
	debug (" atIconLayout[1].iRightBotY = %d \n.", atIconLayout[1].iRightBotY);
	debug (" iIconTotalByte = %d \n.", iIconTotalByte);
*/	
	/*setting_button.jpg 图标位置计算*/
	atIconLayout[2].iLeftTopX = iXres * 1/3;
	atIconLayout[2].iLeftTopY = iYres * 7/10;
	atIconLayout[2].iRightBotX = iXres * 2/3;
	atIconLayout[2].iRightBotY = iYres * 9/10;

	iIconWidth = atIconLayout[2].iRightBotX - atIconLayout[2].iLeftTopX +1; /*注意这里计算是要加1 的*/
	iIconHeight = atIconLayout[2].iRightBotY - atIconLayout[2].iLeftTopY +1;
	iIconTotalByte = iIconWidth * iIconHeight * iBpp / 8;
	if (ptPageLayout->iMaxTotalByte < iIconTotalByte)
	{
		ptPageLayout->iMaxTotalByte = iIconTotalByte;
	}
/*
	debug (" atIconLayout[2].iLeftTopX = %d \n.", atIconLayout[2].iLeftTopX);
	debug (" atIconLayout[2].iLeftTopY = %d \n.", atIconLayout[2].iLeftTopY);
	debug (" atIconLayout[2].iRightBotX = %d \n.", atIconLayout[2].iRightBotX);
	debug (" atIconLayout[2].iRightBotY = %d \n.", atIconLayout[2].iRightBotY);
	debug (" iIconTotalByte = %d \n.", iIconTotalByte);
*/
	debug("***************************emd CalcMainPage***************************\n");
	return 0;
}

/**********************************************************************
 * 函数名称： MainPageShow
 * 功能描述： "主页面"的显示函数，将主页面上的3个图标进行显示
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static void MainPageShow(PT_PageLayout ptMainPageLayout)
{
	/* 1.获得缓存来放置页面内容*/
	/* 2.若页面上图标未计算位置的话，计算图标位置*/
	/* 3.生成图标数据(获得图标像素、进行缩放、合并等操作)*/
	/* 4.刷到设备上进行显示*/
	/* 5.释放相关临时内存*/
	PT_VideoMem ptMainPageMem;
	PT_IconLayout atIconLayout;
	atIconLayout = ptMainPageLayout->atIconLayout;
/* 1.获得缓存来放置页面内容*/
	ptMainPageMem = GetVideoMem(GetPageId("main"), 1);   /* 1 表示是申请该内存是为当前线程使用*/
	if(NULL == ptMainPageMem)
	{
		debug("get VideoMem failed.\n");
	}


/* 2.若页面上图标未计算位置的话，计算图标位置*/
	if(atIconLayout[0].iLeftTopX == 0)
	{
		//CalcMainPageLayout(ptMainPageLayout);
		if(CalcMainPageLayout(ptMainPageLayout))
		{
			debug("CalcMainPageLayout error")
		}
	}

/* 3.生成图标数据(获得图标像素、进行缩放、合并等操作)*/
	//GeneratePage(ptMainPageLayout, ptMainPageMem);
	if(GeneratePage(ptMainPageLayout, ptMainPageMem))
	{
		debug("GeneratePage failed\n");
	}
/* 4.刷到设备上进行显示*/
	FlushVideoMemToDev(ptMainPageMem);

}

/**********************************************************************
 * 函数名称： MainPageGetInputEvent
 * 功能描述： 为"主页面"获得输入数据,判断输入事件位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 ***********************************************************************/
static int MainPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * 函数名称： MainPageRun
 * 功能描述： "主页面"的运行函数: 显示菜单图标,并根据用户对触摸屏输入作出反应
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
//void MainPageRun(PT_PageParams ptParentPageParams)
int MainPageRun(PT_PageParams ptParentPageParams)

{
	int iIndex;
	T_InputEvent tInputEvent;
	T_InputEvent tTempInputEvent;
	T_PageParams tPageParams;

	tPageParams.iPageID = GetPageId("mainpage");
	memset(tPageParams.strCurPictureFile, '\0', 256);

	
	/*显示页面*/
	MainPageShow(&g_tPageLayout);	
	/*准备子线程，预放下一页面*/

	/*获得触摸屏的输入事件*/
	while(1)
	{
		/*获得触摸屏触摸的图标下标*/
		iIndex = GenericGetInputEvent(&g_tPageLayout, &tInputEvent);

		switch (iIndex)
		{
			case 0:			/*浏览文件夹*/
			{
				GetPage("browsepage")->Run(&tPageParams);		/*输入参数是为了判断由哪个页面进入的*/	
				MainPageShow(&g_tPageLayout);
				break;
			}
			case 1:			/*图片连播*/
			{
				GetPage("autopage")->Run(&tPageParams);		/*输入参数是为了判断由哪个页面进入的*/	
				MainPageShow(&g_tPageLayout);
				/*
				picture_scan("./image");
				picture_display();
				*/
				break;
			}
			case 2:			/*设置*/
			{
				GetPage("settingpage")->Run(&tPageParams);		/*输入参数是为了判断由哪个页面进入的*/	
				MainPageShow(&g_tPageLayout);
				break;
			}
			default:
				break;
		}
	}
}

T_PageAction g_tMainPageAction = {
	.name 			= "mainpage",
	.Run 			= MainPageRun,
	.GetInputEvent 	= MainPageGetInputEvent,
};

/**********************************************************************
 * 函数名称： MainPageInit
 * 功能描述： 注册"主页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 ***********************************************************************/
int MainPageInit(void)
{
	return PageActionRegister(&g_tMainPageAction);
}




