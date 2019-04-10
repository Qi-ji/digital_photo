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
int g_ibExitAutoPlay = 0;    /*Flag：当为1时，退出循环播放页面*/
int g_flag = 0;
static pthread_t g_AutoPageThreadID;
static pthread_mutex_t g_tAutoPlayThreadMutex  = PTHREAD_MUTEX_INITIALIZER; /* 互斥量 */

/**********************************************************************
 * 函数名称： AutoPageGetInputEvent
 * 功能描述： 为"连播页面"获得输入数据,判断输入事件位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 ***********************************************************************/
static int AutoPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * 函数名称： AutoPageFunction
 * 功能描述： "连播页面"的显示函数
 * 输入参数： ptPageLayout - 页面 菜单区域 图标及位置
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static void *AutoPageFunction(void *pVoid)
{
	picture_scan(g_tPageCfg.strSeletedDir);
	while(1)
	{
/*
		pthread_mutex_lock(&g_tAutoPlayThreadMutex);	
	    g_flag = g_ibExitAutoPlay;    AutoPageFunction线程检测到这个变量为1后会退出 			
	    pthread_mutex_unlock(&g_tAutoPlayThreadMutex);
*/		
		if(picture_display())			/*返回1的话，说明是触摸后退出的*/
		{
			return NULL;
		}

	}
}
/**********************************************************************
 * 函数名称： GetAutoPageCfg
 * 功能描述： "连播页面"配置数据的获得，包括要显示的文件夹、显示的间隔
 * 输入参数： 
 * 输出参数： ptPageCfg - 配置信息
 * 返 回 值： 无
 ***********************************************************************/
 static void GetAutoPageCfg(PT_PageCfg ptPageCfg)
{
	GetSlectDir(ptPageCfg->strSeletedDir);
	//GetIntervalTime(&ptPageCfg->iIntervalSecond);
}

/**********************************************************************
 * 函数名称： AutoPageRun
 * 功能描述： "连播页面"的运行函数: 显示路径中的目录及文件
 * 输入参数： ptParentPageParams - 页面信息
 * 输出参数： 无
 * 返 回 值： 无
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
		
	/*1. 获得显示的参数：显示的文件夹、显示的间隔*/
	GetAutoPageCfg(&g_tPageCfg);
	g_tPageCfg.iIntervalSecond = 1000;/*毫秒；增加设置页面后，这个要删掉*/


	/*有可能是在manual页面直接进行连播的，所以判断pageparam，*/
	/*若里面有文件夹路径的话，显示该路径下的照片*/
	if (ptParentPageParams->strCurPictureFile[0] != '\0')
	{
		strncpy(g_tPageCfg.strSeletedDir, ptParentPageParams->strCurPictureFile, 256);
		pTemp = strrchr(g_tPageCfg.strSeletedDir, '/');
		*pTemp = '\0';				/*将最后的文件名去掉，只获得目录路径*/ 
	}

	/*2. 开启一个线程来显示照片*/
	pthread_create(&g_AutoPageThreadID, NULL, AutoPageFunction,NULL);

	/*3. 获得触摸屏事件*/
	while(1)
	{
		iRet = GetInputEvent(&tInputEvent);
		if (0 == iRet)
		{
            pthread_mutex_lock(&g_tAutoPlayThreadMutex);
            g_ibExitAutoPlay = 1;   /* AutoPageFunction线程检测到这个变量为1后会退出 */
			g_flag = g_ibExitAutoPlay;	
            pthread_mutex_unlock(&g_tAutoPlayThreadMutex);
            pthread_join(g_AutoPageThreadID, NULL);  /* 等待子线程退出 */
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
 * 函数名称： MainPageInit
 * 功能描述： 注册"主页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 ***********************************************************************/
int AutoPageInit(void)
{
	return PageActionRegister(&g_tAPAction);
}
















