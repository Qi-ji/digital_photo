#ifndef __PAGE_MANAGER_H__
#define __PAGE_MANAGER_H__

#include <disp_manager.h>
#include <page_manager.h>


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
	int iTotalByte;	/*在描绘时是每个图片以此进行描画并合并到内存上，所以这个变量记录该页面上最大的那个图标的总大小*/
	
	PT_IconLayout atIconLayout; 
}T_PageLayout, *PT_PageLayout;


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
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void MainPageRun(void);


#endif 

