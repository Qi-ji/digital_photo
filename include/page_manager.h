#ifndef __PAGE_MANAGER_H__
#define __PAGE_MANAGER_H__

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






#endif 

