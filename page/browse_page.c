#include <stdio.h>

#include <disp_manager.h>
#include <page_manager.h>
#include <render.h>

static T_IconLayout g_atMenuIconLayout[] = {
	{0,0,0,0,"up_button.bmp"},
	{0,0,0,0,"select_button.bmp"},
	{0,0,0,0,"prepage_button.bmp"},
	{0,0,0,0,"nextpage_button.bmp"},
	{0,0,0,0,NULL},
};

static T_PageLayout g_tPageLayout = {
	.iTotalByte = 0,
	.atIconLayout = g_atMenuIconLayout,
};

static char g_strCurDir[256] = DEFAULT_DIR;
//static char g_strSelectedDir[256] = DEFAULT_DIR;

/* 用来描述某目录里的内容 */
static PT_DirContent *g_aptDirContents;  /* 数组:存有目录下"顶层子目录","文件"的名字 */
static int g_iDirContentsNumber;         /* g_aptDirContents数组有多少项 */
//static int g_iStartIndex = 0;            /* 在屏幕上显示的第1个"目录和文件"是g_aptDirContents数组里的哪一项 */

/**********************************************************************
 * 函数名称： CalcBPMenuLayout
 * 功能描述： 计算"浏览页面"的功能菜单位置信息
 * 输入参数： ptParentPageParams - 页面信息,含页面功能菜单信息
 * 输出参数： ptParentPageParams - 页面信息,计算返回页面功能菜单位置信息
 * 返 回 值： 
 ***********************************************************************/
static int CalcBPMenuLayout(PT_PageLayout ptPageLayout)
{
	
}

/**********************************************************************
 * 函数名称： CalcBPInterfaceLayout
 * 功能描述： 计算"浏览页面"的界面图标位置信息，即各个文件夹以及文件图标的信息
 *		 通过全局变量来传递函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 
 ***********************************************************************/

static int CalcBPInterfaceLayout(void)
{

}

/**********************************************************************
 * 函数名称： BrowsePageShow
 * 功能描述： "浏览页面"的显示函数
 * 输入参数： ptParentPageParams - 页面信息
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static void BrowsePageShow(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout;		/*指向4个菜单图标*/

/*获得显存*/
	ptVideoMem = GetVideoMem(GetPageId("browespage"), 1);
	if (ptVideoMem == NULL)
	{
		debug("can't get video mem for browse page!\n");
		return;
	}
/*计算菜单图标位置*/
	if (atIconLayout[0].iLeftTopX == 0)
	{
		CalcBPMenuLayout(ptPageLayout);
		CalcBPInterfaceLayout();
	}

}

/**********************************************************************
 * 函数名称： BrowsePageRun
 * 功能描述： "浏览页面"的运行函数: 显示路径中的目录及文件
 * 输入参数： ptParentPageParams - 页面信息
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static int BrowsePageRun(PT_PageParams ptParentPageParams)
{
	int iIndex;
	int iError;
	T_InputEvent tInputEvent;
	T_PageParams tPageParams;
	PT_DirContent aptDirContents;

	tPageParams.iPageID = GetPageId("browsepage");
	iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
	if (iError)
	{
		debug("GetDirContents error\n");
		return -1;
	}
	debug("g_iDirContentsNumber = %d.\n", g_iDirContentsNumber);

	BrowsePageShow(&g_tPageLayout);
	
}


T_PageAction g_tBPAction = {
	.name 			= "browsepage",
	.Run 			= BrowsePageRun,
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






