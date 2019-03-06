#ifndef   __RENDER_H__
#define   __RENDER_H__

#include <pic_operation.h>

/**********************************************************************
 * 函数名称： GetPixelFrmIcon
 * 功能描述： 获得图标的数据信息，图标放在/etc/digitpic/icons目录下,使用后需要
 *		 调用FreePixelFrmIcon()函数来释放内存。
 * 输入参数： pcName    - 内含缩放后的图片的象素数据
 * 输出参数： ptPixelDatas - 内含原始图片的象素数据(其中bpp数据需要提前指定)
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int GetPixelFrmIcon(char *pcName, PT_PixelDatas ptPixelDatas);


/**********************************************************************
 * 函数名称： FreePixelFrmIcon
 * 功能描述： 释放GetPixelFrmIcon()函数申请的内存
 * 输入参数： ptPixelDatas - 内含原始图片的象素数据
 * 输出参数： 
 * 返 回 值： 
 ***********************************************************************/
void FreePixelFrmIcon(PT_PixelDatas ptPixelDatas);

/**********************************************************************
 * 函数名称： FlushVideoMemToDev
 * 功能描述： 把缓冲区中的数据刷到显示设备上去,即在显示设备上显示缓冲区中的图像
 * 输入参数： ptVideoMem - 缓冲区,内含象素数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
void FlushVideoMemToDev(PT_VideoMem ptVideoMem);



#endif

