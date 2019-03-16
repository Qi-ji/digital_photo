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
void ClearRegionVideoMem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, PT_VideoMem ptVideoMem, unsigned int dwColor);

/**********************************************************************
 * 函数名称： PressIcon
 * 功能描述： 按下图标,只是改变显示设备上的图标按钮颜色
 * 输入参数： ptLayout   - 图标所在矩形区域
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void PressIcon(PT_IconLayout ptLayout);



#endif

