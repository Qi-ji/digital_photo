#ifndef __PAGE_MANAGER_H__
#define __PAGE_MANAGER_H__

#include <disp_manager.h>
#include <page_manager.h>
#include <input_manager.h>




/*页面上图标区域位置结构体*/
typedef struct IconLayout
{
	int iLeftTopX;
	int iLeftTopY;
	int iRightBotX;
	int iRightBotY;
	char *pcName; 
}T_IconLayout, *PT_IconLayout;

/*整个页面区域位置结构体*/
typedef struct PageLayout
{
	int iLeftTopX;
	int iLeftTopY;
	int iRightBotX;
	int iRightBotY;
	int iWidth;
	int iHeight;
	int iBpp;
	int iMaxTotalByte;	/*在描绘时是每个图片以此进行描画并合并到内存上，所以这个变量记录该页面上最大的那个图标的总大小*/
	PT_IconLayout atIconLayout; 
}T_PageLayout, *PT_PageLayout;


typedef struct PageParams {
    int  iPageID;                  /* 页面的ID */
    char strCurPictureFile[256];  /* 要处理的第1个图片文件 */
}T_PageParams, *PT_PageParams;


/*页面操作结构体*/
typedef struct PageAction {
	char 	*name;            							/* 页面名字 */
	int	(*Run)(PT_PageParams ptParentPageParams);  	/* 页面的运行函数 */ 
	int 	(*GetInputEvent)(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent);  /* 获得输入数据的函数 */
	int 	(*Prepare)(void);        						 /* (未实现)后台准备函数: 为加快程序运行而同时处理某些事情 */
	struct PageAction *ptNext;    						 /* 链表 */
}T_PageAction, *PT_PageAction;


/**********************************************************************
 * 函数名称： RegisterPageAction
 * 功能描述： 注册"页面模块", "页面模块"含有页面显示的函数
 * 输入参数： ptPageAction - 一个结构体,内含"页面模块"的操作函数
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 ***********************************************************************/
int PageActionRegister(PT_PageAction ptPageAction);

/**********************************************************************
 * 函数名称： GetPageId
 * 功能描述： 获得页面专属的ID
 * 输入参数： pcPageName - 页面名字 例如main brose setting 等
 * 输出参数： 无
 * 返 回 值： 页面专属ID
 ***********************************************************************/
int GetPageId(char *pcPageName);

/**********************************************************************
 * 函数名称： GeneratePage
 * 功能描述： 从图标文件中解析出图像数据并放在指定区域,从而生成页面数据
 * 输入参数： ptPageLayout - 内含多个图标的文件名和显示区域
 *            ptVideoMem   - 在这个VideoMem里构造页面数据
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem);

/**********************************************************************
 * 函数名称： MainPageRun
 * 功能描述： "主页面"的运行函数: 显示菜单图标,并根据用户对触摸屏输入作出反应
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
int MainPageRun(PT_PageParams ptParentPageParams);


/**********************************************************************
 * 函数名称： GenericGetInputEvent
 * 功能描述： 读取输入数据,并判断它位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 ***********************************************************************/
int GenericGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent);

/**********************************************************************
 * 函数名称： GetPage
 * 功能描述： 根据名字取出指定的"页面模块"
 * 输入参数： pcName - 名字
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - "页面模块"的PT_PageAction结构体指针
 ***********************************************************************/
PT_PageAction GetPage(char *pcName);

/**********************************************************************
 * 函数名称： MainPageInit
 * 功能描述： 注册"主页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int MainPageInit(void);

/**********************************************************************
 * 函数名称： MainPageInit
 * 功能描述： 注册"主页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 ***********************************************************************/
int BrowsePageInit(void);

/**********************************************************************
 * 函数名称： PagesInit
 * 功能描述： 调用各个"页面模块"的初始化函数,就是注册它们
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int PagesInit(void);

/**********************************************************************
 * 函数名称： ManualPageInit
 * 功能描述： 注册"手动页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期 	   版本号	 修改人		  修改内容
 ***********************************************************************/
int ManualPageInit(void);

#endif 

