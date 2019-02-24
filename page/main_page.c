#include <stdio.h>

#include <disp_manager.h>
#include <page_manager.h>

static PT_IconLayout g_atMainPageIconLayout[] = {
	{0,0,0,0,"browse_button.jpg"},
	{0,0,0,0,"continue_button.jpg"},
	{0,0,0,0,"setting_button.jpg"},
	{0,0,0,0,NULL},
};

static T_PageLayout g_tPageLayout = {
	.iTotalByte = 0,
	.atIconLayout = g_atMainPageIconLayout,
};


/**********************************************************************
 * 函数名称： CalcMainPageLayout
 * 功能描述： 计算首页面中各图标座标值
 * 输入参数： 无
 * 输出参数： ptPageLayout - 内含各图标的左上角/右下角座标值
 * 返 回 值： 无
 ***********************************************************************/
static void  CalcMainPageLayout(PT_PageLayout ptPageLayout)
{
	if (NULL == ptPageLayout || NULL == ptPageLayout->atIconLayout)
		{return -1;}
		
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

	if (ptPageLayout->iTotalByte < iIconTotalByte)
	{
		ptPageLayout->iTotalByte = iIconTotalByte;
	}
	
	/*continue_button.jpg 图标位置计算*/
	atIconLayout[1].iLeftTopX = iXres * 1/3;
	atIconLayout[1].iLeftTopY = iYres * 4/10;
	atIconLayout[1].iRightBotX = iXres * 2/3;
	atIconLayout[1].iRightBotY = iYres * 6/10;
	
	/*setting_button.jpg 图标位置计算*/
	atIconLayout[2].iLeftTopX = iXres * 1/3;
	atIconLayout[2].iLeftTopY = iYres * 7/10;
	atIconLayout[2].iRightBotX = iXres * 2/3;
	atIconLayout[2].iRightBotY = iYres * 9/10;
}








