#ifndef __DISP_MANAGER_H__
#define __DISP_MANAGER_H__

#include <config.h>
#include <pic_operation.h>
#include <disp_manager.h>
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



typedef enum {
	VMS_FREE = 0,		/*该缓存空闲*/
	VMS_CUR,				/*该缓存被当前线程使用*/
	VMS_PRE,				/*该缓存为预备线程使用*/
}E_VideoMemState;

typedef enum {
	VMC_BLANK = 0,			/*该缓存内空白*/
	VMC_GENERATING,			/*该缓存的内容正在绘制*/
	VMC_GENERATE,			/*该缓存的内容绘制结束*/
}E_VideoMemContent;


typedef struct VideoMem{
	int iMemID;							/*该缓存的ID号*/
	int bFBDev;							/* 1:表示该缓存是FB; 0:表示这是一个普通缓存*/
	E_VideoMemState eMemState;			/*该缓存的状态：free/current threat/prepare threat*/
	E_VideoMemContent eMemContent;		/*该缓存的内容：blank/generate/generating*/
	T_PixelDatas tPixelDatas;			/*内存中内容的像素数据*/
	struct VideoMem *ptNext;
}T_VideoMem, *PT_VideoMem;

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
	int(*ShowOnePage)(PT_VideoMem ptVideoMem);
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

/**********************************************************************
 * 函数名称： AllocVideoMem
 * 功能描述： VideoMem: 为加快显示速度,我们事先在缓存中构造好显示的页面的数据,
 *            (这个缓存称为VideoMem)
 *            显示时再把VideoMem中的数据复制到设备的显存上
 * 输入参数： iNum
 * 输出参数： 无
 * 返 回 值： 0  - 成功
 *            -1 - 失败(未使用SelectAndInitDefaultDispDev来选择显示模块)
 ***********************************************************************/
int AllocVideoMem(int iNum);

/**********************************************************************
 * 函数名称： GetVideoMem
 * 功能描述： 获得一块可操作的VideoMem(它用于存储要显示的数据), 
 *            用完后用PutVideoMem来释放
 * 输入参数： iID  - ID值,先尝试在众多VideoMem中找到ID值相同的
 *            bCur - 1表示当前程序马上要使用VideoMem,无法如何都要返回一个VideoMem
 *                   0表示这是为了改进性能而提前取得VideoMem,不是必需的
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有可用的VideoMem
 *            非NULL - 成功,返回PT_VideoMem结构体
 ***********************************************************************/
PT_VideoMem GetVideoMem(int iID, int bCur);

/**********************************************************************
 * 函数名称： ClearVideoMem
 * 功能描述： 把VideoMem中内存全部清为某种颜色
 * 输入参数： ptVideoMem - VideoMem结构体指针, 内含要操作的内存
 *            dwColor    - 设置为该颜色
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ClearVideoMem(PT_VideoMem ptVideoMem, unsigned int dwColor);




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






