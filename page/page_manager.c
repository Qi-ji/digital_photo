#include <stdio.h>
#include <stdlib.h>


#include <config.h>
#include <pic_manager.h>
#include <pic_operation.h>
#include <page_manager.h>
#include <render.h>
#include <input_manager.h>



/**********************************************************************
 * 函数名称： GetPageId
 * 功能描述： 获得页面专属的ID
 * 输入参数： pcPageName - 页面名字 例如main brose setting 等
 * 输出参数： 无
 * 返 回 值： 页面专属ID
 ***********************************************************************/
int GetPageId(char *pcPageName)
{
	return (pcPageName[0] + pcPageName[1] + pcPageName[2] + pcPageName[3]);
}

/**********************************************************************
 * 函数名称： GeneratePage
 * 功能描述： 从图标文件中解析出图像数据并放在指定区域,从而生成页面数据
 * 输入参数： ptPageLayout - 内含多个图标的文件名和显示区域
 *            ptVideoMem   - 在这个VideoMem里构造页面数据
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem)
{
debug("***************************star GeneratePage***************************\n");
	int iError;
/* 1.生成图标数据(获得图标像素、进行缩放、合并等操作)*/
	PT_IconLayout atIconLayout;
	atIconLayout = ptPageLayout->atIconLayout;	/* 指向图标数组*/
	T_PixelDatas tOriginalPixelDatas;
	T_PixelDatas tZoonPixelDatas;				/*临时储存图标缩放后的数据，用于合并到显存上*/

	if (ptVideoMem->eMemContent != VMC_GENERATE)
	{
		ClearVideoMem(ptVideoMem, BLACK);
		
		tOriginalPixelDatas.ibpp = ptPageLayout->iBpp; /*bpp 信息需要提前设定*/
		tZoonPixelDatas.ibpp 	 = ptPageLayout->iBpp; /*bpp 信息需要提前设定*/
		tZoonPixelDatas.pucPixelDatas = malloc(ptPageLayout->iTotalByte);
		if (NULL == tZoonPixelDatas.pucPixelDatas)
			return -1;
	
		while (atIconLayout->pcName)
		{
debug("***************************star GetPixelFrmIcon***************************\n");
			/* 获得图标像素数据*/
			iError = GetPixelFrmIcon(atIconLayout->pcName, &tOriginalPixelDatas);
			if (iError)
			{
				debug("GetPixelFrmIcon: %s error.\n", atIconLayout->pcName);
				free(tZoonPixelDatas.pucPixelDatas);
				return -1;
			}
debug("***************************end GetPixelFrmIcon***************************\n");

			/* 获得缩放后图标像素数据，通过atIconLyout来获得图标的长宽等信息*/
			tZoonPixelDatas.iHeight 	= atIconLayout->iRightBotY - atIconLayout->iLeftTopY + 1;
			tZoonPixelDatas.iWidth 		= atIconLayout->iRightBotX - atIconLayout->iLeftTopX + 1;
			tZoonPixelDatas.iLineByte	= tZoonPixelDatas.iWidth * tZoonPixelDatas.ibpp / 8;
			tZoonPixelDatas.iTotalByte  = tZoonPixelDatas.iLineByte * tZoonPixelDatas.iHeight;
debug("***************************start PicZoom***************************\n");

			debug("tZoonPixelDatas.iHeight = %d.\n", tZoonPixelDatas.iHeight);
			debug("tZoonPixelDatas.iWidth = %d.\n", tZoonPixelDatas.iWidth);
			debug("tZoonPixelDatas.iLineByte = %d.\n", tZoonPixelDatas.iLineByte);
			debug("tZoonPixelDatas.iTotalByte = %d.\n", tZoonPixelDatas.iTotalByte);
			/*缩放图标并合并到显存上*/
			PicZoom(&tOriginalPixelDatas, &tZoonPixelDatas);
debug("***************************end PicZoom***************************\n");
			PicMerge(atIconLayout->iLeftTopX, atIconLayout->iLeftTopY, &tZoonPixelDatas, &ptVideoMem->tPixelDatas);
			/* 释放获得图标元数据，并指向下一个图标*/
			FreePixelFrmIcon(&tOriginalPixelDatas);
			atIconLayout++;
		}
		ptVideoMem->eMemContent = VMC_GENERATE;
		free(tZoonPixelDatas.pucPixelDatas);

	}
	debug("***************************end GeneratePage***************************\n");
	return 0;
}


/**********************************************************************
 * 函数名称： GenericGetInputEvent
 * 功能描述： 读取输入数据,并判断它位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 ***********************************************************************/
int GenericGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	int iRet;
	int i = 0;
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout; /*指向图标数组*/
	
	/* 获得原始的触摸屏数据 
	 * 它是调用input_manager.c的函数，此函数会让当前线否休眠
	 * 当触摸屏线程获得数据后，会把它唤醒
	 */
	iRet = GetInputEvent(&tInputEvent);
	if (iRet)
	{
		return -1;
	}

	if (tInputEvent.iType != TOUCHSCREEN)
	{
		return -1;
	}

	*ptInputEvent = tInputEvent;
	
	/* 处理数据 */
	/* 确定触点位于哪一个按钮上 */
	while (atIconLayout[i].pcName)
	{
		if ((tInputEvent.iX >= atIconLayout[i].iLeftTopX) && (tInputEvent.iX <= atIconLayout[i].iRightBotX) && \
			 (tInputEvent.iY >= atIconLayout[i].iLeftTopY) && (tInputEvent.iY <= atIconLayout[i].iRightBotY))
		{
			debug("\nthe index is %d\n", i);
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


