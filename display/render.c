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
	snprintf(tIconFileMap.strFileName, 128, "%s/%s", ICON_PATH, pcName);
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

debug("Icon original iWidth = %d.\n",ptPixelDatas->iWidth);
debug("Icon original iHeight = %d.\n",ptPixelDatas->iHeight);
debug("Icon original iLineByte = %d.\n",ptPixelDatas->iLineByte);
debug("Icon original iTotalByte = %d.\n",ptPixelDatas->iTotalByte);

	
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
 ***********************************************************************/
void FlushVideoMemToDev(PT_VideoMem ptVideoMem)
{
	//memcpy((unsigned char *)(GetDefaultDispOpr()->pdwDispMem), ptVideoMem->tPixelDatas.pucPixelDatas, ptVideoMem.tPixelDatas.iHeight * ptVideoMem.tPixelDatas.iLineByte);

	if (!ptVideoMem->bFBDev)
	{
		//GetDefaultDispOpr()->ShowPixel(&ptVideoMem->tPixelDatas);
		GetDefaultDispOpr()->ShowOnePage(ptVideoMem);
	}
	
}
/**********************************************************************
 * 函数名称： SetColorForPixelInVideoMem
 * 功能描述： 设置VideoMem中某个座标象素的颜色
 * 输入参数： iX,iY      - 象素座标
 *            ptVideoMem - 设置VideoMem中的象素
 *            dwColor    - 设置为这个颜色,颜色格式为0x00RRGGBB
 * 输出参数： 无
 * 返 回 值： 这个象素占据多少字节
 * 修改日期        版本号     修改人	      修改内容
 ***********************************************************************/
static int SetColorForPixelInVideoMem(int iX, int iY, PT_VideoMem ptVideoMem, unsigned int dwColor)
{
	unsigned char *pucVideoMem;
	unsigned short *pwVideoMem16bpp;
	unsigned int *pdwVideoMem32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;

	pucVideoMem      = ptVideoMem->tPixelDatas.pucPixelDatas;
	pucVideoMem      += iY * ptVideoMem->tPixelDatas.iLineByte+ iX * ptVideoMem->tPixelDatas.ibpp/ 8;
	pwVideoMem16bpp  = (unsigned short *)pucVideoMem;
	pdwVideoMem32bpp = (unsigned int *)pucVideoMem;

	switch (ptVideoMem->tPixelDatas.ibpp)
	{
		case 8:
		{
			*pucVideoMem = (unsigned char)dwColor;
			return 1;
			break;
		}
		case 16:
		{
			iRed   = (dwColor >> (16+3)) & 0x1f;
			iGreen = (dwColor >> (8+2)) & 0x3f;
			iBlue  = (dwColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			*pwVideoMem16bpp	= wColor16bpp;
			return 2;
			break;
		}
		case 32:
		{
			*pdwVideoMem32bpp = dwColor;
			return 4;
			break;
		}
		default :
		{			
			return -1;
		}
	}

	return -1;
}

/**********************************************************************
 * 函数名称： ClearRegionVideoMem
 * 功能描述： 清除VideoMem中某个矩形区域,设为某颜色
 * 输入参数： iTopLeftX,iTopLeftY   - 矩形区域的左上角座标
 *            iBotRightX,iBotRightY - 矩形区域的右下角座标
 *            ptVideoMem            - 设置VideoMem中的矩形区域
 *            dwColor               - 设置为这个颜色,颜色格式为0x00RRGGBB
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void ClearRegionVideoMem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, PT_VideoMem ptVideoMem, unsigned int dwColor)
{
	int x, y;
	for (y = iTopLeftY; y <= iBotRightY; y++)
		for (x = iTopLeftX; x <= iBotRightX; x++)
			SetColorForPixelInVideoMem(x, y, ptVideoMem, dwColor);
}


