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
 * 函数名称： CalcSPMenuLayout
 * 功能描述： 计算"设置页面"的功能菜单位置信息
 * 输入参数： ptPageLayout - 页面信息,含页面功能菜单信息
 * 输出参数： ptPageLayout - 页面信息,计算返回页面功能菜单位置信息
 * 返 回 值： 
 ***********************************************************************/
static int CalcSPMenuLayout(PT_PageLayout ptPageLayout)
{
	if (NULL == ptPageLayout || NULL == ptPageLayout->atIconLayout)
			return -1;
	
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout;
	
	int iXres, iYres, iBpp;  /*显示设备的参数*/
	int iIconWidth, iIconHeight; /*每个图标的宽、高都是一样的，所以计算第一个图标就可以了*/
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
		iIconWidth = atIconLayout[i].iRightBotX - atIconLayout[i].iLeftTopX +1; /*注意这里计算是要加1 的*/
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
 * 函数名称： SettingPageGetInputEvent
 * 功能描述： 为"设置页面"获得输入数据,判断输入事件位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 ***********************************************************************/
static int SettingPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * 函数名称： SettingPageRun
 * 功能描述： "设置页面"的运行函数:
 * 输入参数： ptParentPageParams - 页面信息
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static void SettingPageShow(PT_PageLayout ptPageLayout)
{
	int iError;
	PT_VideoMem ptVideoMem;
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout;
	
	/*1. 获得显存*/
	ptVideoMem = GetVideoMem(GetPageId("settingpage"), 1);
	if (NULL == ptVideoMem)
	{
		debug("error: setting page videomem error\n");
		return;
	}
	
	/*2. 计算坐标*/
	if (g_atSPIconsLayout[0].iLeftTopX == 0)
	{
		CalcSPMenuLayout(ptPageLayout);
	}
	/*3. 获得图标像素数据*/
	iError = GeneratePage(ptPageLayout, ptVideoMem);
	if (iError)
	{
		debug("error: generate setting page error\n");
		return ;
	}
	/*4. 刷到设备上进行显示*/
	FlushVideoMemToDev(ptVideoMem);
}



/**********************************************************************
 * 函数名称： SettingPageRun
 * 功能描述： "设置页面"的运行函数:
 * 输入参数： ptParentPageParams - 页面信息
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static void SettingPageRun(PT_PageParams ptParentPageParams)
{
	T_PageParams tPageParams;
	int iIndex;
	T_InputEvent tInputEvent;

	tPageParams.iPageID = GetPageId("settingpage");
	
	/*1. 页面显示*/
	SettingPageShow(&g_atSPMenuLayout);
	
	/* 2.获得输入事件*/
	while (1)
	{
		/*获得触摸屏触摸的图标下标*/
		iIndex = GenericGetInputEvent(&g_atSPMenuLayout, &tInputEvent);
		
		switch (iIndex)
		{
			case 0: 		/*选择目录*/
			{
				GetPage("browsepage")->Run(&tPageParams);		/*输入参数是为了判断由哪个页面进入的*/ 
				SettingPageShow(&g_atSPMenuLayout);
				break;
			}
			case 1: 		/*设置间隔*/
			{
				GetPage("autopage")->Run(&tPageParams); 	/*输入参数是为了判断由哪个页面进入的*/ 
				SettingPageShow(&g_atSPMenuLayout);

				break;
			}
			case 2: 		/*返回*/
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
 * 函数名称： SettingPageInit
 * 功能描述： 注册"设置页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int SettingPageInit(void)
{
	return PageActionRegister(&g_tSPAction);
}









