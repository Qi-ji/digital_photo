#include <stdio.h>

#include <config.h>
#include <pic_manager.h>
#include <pic_operation.h>
#include <page_manager.h>
#include <render.h>
#include <file.h>
#include <disp_manager.h>




/**********************************************************************
 * 函数名称： GetPixelFrmIcon
 * 功能描述： 获得图标的数据信息，图标放在/etc/digitpic/icons目录下,使用后需要
 *		 调用FreePixelFrmIcon()函数来释放内存。
 * 输入参数： pcName    - 内含缩放后的图片的象素数据
 * 输出参数： ptPixelDatas - 内含原始图片的象素数据(其中bpp数据需要提前指定)
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int GetPixelFrmIcon(char *pcName, PT_PixelDatas ptPixelDatas)
{
	int ierror;
	T_FileMap tIconFileMap;

	/* FileMap 打开图标*/
	sprintf(tIconFileMap.strFileName, 128, "%s/%s", ICON_PATH, pcName);
	tIconFileMap.strFileName[127] = '\0';
	ierror = MapFile(&tIconFileMap);
	if(ierror)
		return -1;

	/* 图片解析器解析图标 */
	ierror = PicPraser("bmp")->isSupport(&tIconFileMap);
	if(!ierror)
	{
		debug("not support Praser Icon: %s.\n", tIconFileMap.strFileName);
		UnMapFile(&tIconFileMap);
		return -1;
	}
	ierror = PicPraser("bmp")->GetPixelDatas(&tIconFileMap, ptPixelDatas);
	if(ierror)
	{
		debug("Praser Icon: %s error.\n", tIconFileMap.strFileName);
		UnMapFile(&tIconFileMap);
		return -1;
	}
	
	UnMapFile(&tIconFileMap);
	return 0;
	
}

/**********************************************************************
 * 函数名称： FreePixelFrmIcon
 * 功能描述： 释放GetPixelFrmIcon()函数申请的内存
 * 输入参数： ptPixelDatas - 内含原始图片的象素数据
 * 输出参数： 
 * 返 回 值： 
 ***********************************************************************/
void FreePixelFrmIcon(PT_PixelDatas ptPixelDatas)
{
	PicPraser("bmp")->FreePixelDatas(ptPixelDatas);
}

/**********************************************************************
 * 函数名称： FlushVideoMemToDev
 * 功能描述： 把缓冲区中的数据刷到显示设备上去,即在显示设备上显示缓冲区中的图像
 * 输入参数： ptVideoMem - 缓冲区,内含象素数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void FlushVideoMemToDev(PT_VideoMem ptVideoMem)
{
	//memcpy(GetDefaultDispDev()->pucDispMem, ptVideoMem->tPixelDatas.aucPixelDatas, ptVideoMem.tPixelDatas.iHeight * ptVideoMem.tPixelDatas.iLineBytes);
	if (!ptVideoMem->bFBDev)
	{
		GetDefaultDispOpr()->ShowOnePage(ptVideoMem);
	}
}



