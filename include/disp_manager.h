#ifndef __DISP_MANAGER_H__
#define __DISP_MANAGER_H__

#include <config.h>
#include <pic_operation.h>
// 宏定义
#define FBDEVICE	"/dev/fb0"

#define WIDTH		1024	
#define HEIGHT		600

#define WHITE		0xffffffff			// test ok
#define BLACK		0x00000000
#define RED			0xffff0000
#define GREEN		0xff00ff00			// test ok
#define BLUE		0xff0000ff			

#define GREENP		0x0000ff00			// 一样，说明前2个ff透明位不起作用

typedef struct DispOpr{
	char 			*name;				/*显示设备名称*/
	int  			iXres;				/*X方向分辨率*/
	int  			iYres;				/*X方向分辨率*/
	int				ibpp;
	int				iMemTotalByte;
	unsigned int 	*pdwDispMem; 		/*mmap 获得的显存地址*/
	int(*DeviceInit)(void);
	int(*DeviceClose)(void);
	int(*ShowOnePiexl)(void);
	void(*CleanScreen)(unsigned int icolor);
	int (*ShowPixel)(PT_PixelDatas ptPixelDatas);
	int(*ShowOnePage)(PT_PixelDatas ptPixelDatas);
	struct DispOpr *next;
}T_DispOpr, *PT_DispOpr;


/**********************************************************************
 * 函数名称： DispOprRegister
 * 功能描述： 注册所有的显示设备到链表
 * 输入参数：
 * 输出参数： 
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int DispOprRegister(PT_DispOpr ptDispOpr);

/**********************************************************************
 * 函数名称： FBRegister
 * 功能描述： 注册FB显示设备到链表
 * 输入参数： 
 * 输出参数： 
 * 返 回 值： 
 **********************************************************************/
int FBRegister(void);

/**********************************************************************
 * 函数名称： ShowDispOpr
 * 功能描述： 打印所有注册过的显示设备
 * 输入参数： 
 * 输出参数： 
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
 int ShowDispOpr(void);

/**********************************************************************
 * 函数名称： GetDispOpr
 * 功能描述： 输入显示设备名称，获得该设备显示模块，例如:输入"fb",获得fb模块结构体
 * 输入参数： pcname - 显示设备名称
 * 输出参数： 无
 * 返 回 值：NULL   - 失败,没有指定的模块, 
 *		 PT_DispOpr - 显示模块结构体指针
 ***********************************************************************/
PT_DispOpr GetDispOpr(char *pcname);

/**********************************************************************
 * 函数名称： SetAndInitDefaultDispOpr
 * 功能描述： 选定默认的显示设备，并初始化该设备
 * 输入参数： pcname - 显示设备名称
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void SetAndInitDefaultDispOpr(char *pcname);

/**********************************************************************
 * 函数名称： GetDefaultDispOpr
 * 功能描述： 返回默认的显示设备
 * 输入参数： pcname - 显示设备名称
 * 输出参数： 无
 * 返 回 值： PT_DispOpr
 ***********************************************************************/
PT_DispOpr GetDefaultDispOpr(void);

/**********************************************************************
 * 函数名称： GetDefaultDispOpr
 * 功能描述： 返回默认的显示设备
 * 输入参数： pcname - 显示设备名称
 * 输出参数： 无
 * 返 回 值： PT_DispOpr
 ***********************************************************************/
int GetDefDispResolution(int *piXres, int *piYres, int *piBpp);







// 函数声明
int is_bmp(const char *path);

void fb_draw_picture(const unsigned char *Ppic);
void fb_draw_picture_test(pic_info *ppic);
void fb_draw_jpeg_test(pic_info *ppic);
void fb_draw_png(pic_info *ppic);





/*
void fb_draw_jpgpicture(void);
void fb_draw_smallpicture(void);
void fb_draw_picture_randomposition(unsigned int x0, unsigned int y0);
void fb_draw_picture_beyond_bounds(unsigned int x0, unsigned int y0);
*/





#endif






