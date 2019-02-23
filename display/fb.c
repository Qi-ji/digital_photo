/*
 * file name:fb.c
 * function:open framebuffer,and use ioctl to get some information of decives
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>


#include <pic_operation.h>
#include <disp_manager.h>
#include <config.h>


// 全局变量
static int g_fbfd = -1;
static unsigned int *g_pFBMem = NULL;  /*指向fb显存*/
static unsigned long g_FBScreenSize = 0;

static int FBInit(void);
static int FBClose(void);
static void FBCleanScrean(unsigned int color);
static int FBShowPage(PT_PixelDatas ptPixelDatas);
static int FBShowPixel(PT_PixelDatas ptPixelDatas);


T_DispOpr g_tFBOpr = {
	.name = "fb",
	.DeviceInit  = FBInit,
	.DeviceClose = FBClose,
	.CleanScreen = FBCleanScrean,
	.ShowPixel	 = FBShowPixel,
	.ShowOnePage = FBShowPage,
};

/**********************************************************************
 * 函数名称： FbInit
 * 功能描述： "framebuffer显示设备"的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
static int FBInit(void)
{
	int ret = -1;
	
	
	struct fb_fix_screeninfo FBFixInfo;
	struct fb_var_screeninfo FBVarInfo;
	
	// 第1步：打开设备
	g_fbfd = open(FBDEVICE, O_RDWR);
	if (g_fbfd < 0)
	{
		perror("open error");
		return -1;
	}
	debug("open %s success.\n", FBDEVICE);
	
	// 第2步：获取设备的硬件信息
	ret = ioctl(g_fbfd, FBIOGET_FSCREENINFO, &FBFixInfo);
	if (ret < 0)
	{
		perror("ioctl error");
		return -1;
	}
	debug("smem_start = 0x%lx, smem_len = %u.\n", FBFixInfo.smem_start, FBFixInfo.smem_len);
	
	ret = ioctl(g_fbfd, FBIOGET_VSCREENINFO, &FBVarInfo);
	if (ret < 0)
	{
		perror("ioctl error");
		return -1;
	}

	g_tFBOpr.iXres = FBVarInfo.xres;
	g_tFBOpr.iYres = FBVarInfo.yres;
	g_tFBOpr.ibpp   = FBVarInfo.bits_per_pixel;
	
	debug("xres = %u, yres = %u.\n", FBVarInfo.xres, FBVarInfo.yres);
	debug("bpp = %u.\n", FBVarInfo.bits_per_pixel);
	
	// 第3步：进行mmap
	g_FBScreenSize = FBVarInfo.xres * FBVarInfo.yres * FBVarInfo.bits_per_pixel / 8;
	g_tFBOpr.iMemTotalByte = (int)g_FBScreenSize;
	debug("FBScreenSize = %ld\n", g_FBScreenSize);
	g_pFBMem = mmap(NULL, g_FBScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, g_fbfd, 0);
	if (NULL == g_pFBMem)
	{
		perror("mmap");
		return -1;
	}
	g_tFBOpr.pdwDispMem = g_pFBMem;
	debug("g_tFBOpr.pucDispMem = %p.\n", g_tFBOpr.pdwDispMem);
	
	return 0;
}

/**********************************************************************
 * 函数名称： FBClose
 * 功能描述： "framebuffer显示设备"的设备关闭函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
static int FBClose(void)
{
	close(g_fbfd);
	
	return 0;
}

/**********************************************************************
 * 函数名称： FBCleanScrean
 * 功能描述： 清除屏幕
 * 输入参数： color - WHITE/BLACK/RED 等
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static void FBCleanScrean(unsigned int color)
{
	
	unsigned int x, y;
	for (y=0; y<g_tFBOpr.iYres; y++)
	{
		for (x=0; x<g_tFBOpr.iXres; x++)
		{
			*(g_pFBMem + (y * g_tFBOpr.iXres) + x) = color;
		}
	}
	/*
	int i;
	while (i < g_FBScreenSize)
			{
				*g_pFBMem	= color;
				g_pFBMem++;
				i += 4;
			}
			*/
}

/**********************************************************************
 * 函数名称： FBShowPixel
 * 功能描述： 显示像素
 * 输入参数： 
 * 输出参数： 
 * 返 回 值： 
 **********************************************************************/
static int FBShowPixel(PT_PixelDatas ptPixelDatas)
{
	unsigned char *pucSrc;
	unsigned char *pucDst;
	int iLineByteSm, iHeightSm;		/*fb 与 pic 中较小的那个iLinebyte 与 iHeight*/
	int iFBXres, iFBYres, iFBBpp;
	int iFBLineByte;
	int i,y;

	pucSrc = ptPixelDatas->pucPixelDatas;
	pucDst = (unsigned char*)g_pFBMem;

	GetDefDispResolution(&iFBXres, &iFBYres, &iFBBpp);
	iFBLineByte = iFBXres * iFBBpp / 8;
	/*使用三目运算来判读fb 与 pic 哪个的宽，高更大，防止memcpy时内存溢出*/
	iLineByteSm = (ptPixelDatas->iLineByte >= iFBLineByte) ? iFBLineByte : ptPixelDatas->iLineByte;
	iHeightSm = (ptPixelDatas->iHeight >= iFBYres) ? iFBYres : ptPixelDatas->iHeight;

	//debug(" iLineByteS= %d.\n", iLineByteSm);
	//debug(" iHeightSm= %d.\n", iHeightSm);
	//debug(" iFBLineByte= %d.\n", iFBLineByte);
	//debug(" ptPixelDatas->iLineByte = %d.\n", ptPixelDatas->iLineByte);

	for (y=0; y<iHeightSm; y++)
	{
		memcpy(pucDst, pucSrc, iLineByteSm);
		pucDst = pucDst +  iFBLineByte;
		pucSrc = pucSrc +  ptPixelDatas->iLineByte;
		//debug("pucDst - (unsigned char*)g_tFBOpr.pdwDispMem = %d\n", pucDst - (unsigned char*)g_tFBOpr.pdwDispMem);
		//debug("pucSrc - ptPixelDatas->pucPixelDatas = %d\n", pucSrc - ptPixelDatas->pucPixelDatas);
	}
	
	
	return 0;
}

/**********************************************************************
 * 函数名称： FBShowPage
 * 功能描述：显示页面(可以是单张图片，也可以是几张图片合成的一个页面)
 * 输入参数： 
 * 输出参数： 
 * 返 回 值： 
 **********************************************************************/
static int FBShowPage(PT_PixelDatas ptPixelDatas)
{
	int iTotal;

	if (ptPixelDatas->iTotalByte > g_tFBOpr.iMemTotalByte)
	{
		iTotal =  g_tFBOpr.iMemTotalByte;
	}
	else
	{
		iTotal = ptPixelDatas->iTotalByte;
	}
	
	memcpy(g_pFBMem, ptPixelDatas->pucPixelDatas, iTotal);
	return 0;
}

/**********************************************************************
 * 函数名称： FBRegister
 * 功能描述： 注册FB显示设备到链表
 * 输入参数： 
 * 输出参数： 
 * 返 回 值： 
 **********************************************************************/
int FBRegister(void)
{
	return DispOprRegister(&g_tFBOpr);
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/



/*
 *function:show bmp the picture
 *         the screen:1020*600 and the picture:1020*600
 */
void fb_draw_picture(const unsigned char *Ppic)
{
    unsigned int *pdwFB = g_pFBMem;
	const unsigned char *pdate = Ppic;
	unsigned int x, y;
    unsigned int cnt_fb;
	unsigned int cnt_pic=HEIGHT * WIDTH *3;
	
	for (y=0; y<HEIGHT; y++)
	{
        for (x=0; x<WIDTH; x++)
        {
			cnt_fb = WIDTH* y + x;

			*(pdwFB+cnt_fb) = ((pdate[cnt_pic-0]<<0) |(pdate[cnt_pic-1]<<8) | (pdate[cnt_pic-2]<<16));
			cnt_pic -= 3;
			
		}
	}
}


/*
 *function:
 *         
 */
void fb_draw_picture_test(pic_info *ppic)
{
    unsigned int *p = g_pFBMem;
	const char *pdate = ppic->pic_date;
	unsigned int x, y;
    unsigned int cnt_fb;
	//unsigned int cnt_pic= ppic->width * ppic->height * 3 -3;
	unsigned int cnt_pic= ppic->width * ppic->height * 3 -1;//这里的-1是因为
															//从0开始计算,所以最后会多一位
	
	for (y=0; y < ppic->height; y++)
	{
        for (x=0; x < ppic->width; x++)
        {
			cnt_fb = WIDTH* y + x;  //注意这里是width!!!!是屏幕的宽，在这里犯错误了

			*(p+cnt_fb) = ((pdate[cnt_pic-2]<<0) |(pdate[cnt_pic-1]<<8) | (pdate[cnt_pic-0]<<16));
//			*(p+cnt_fb) = ((pdate[cnt_pic+0]<<0) |(pdate[cnt_pic+1]<<8) | (pdate[cnt_pic+2]<<16));

			cnt_pic -= 3;
			
		}
	}
}
/*
 *function:
 *         
 */
void fb_draw_jpeg_test(pic_info *ppic)
{
    unsigned int *p = g_pFBMem;
	const char *pdate = ppic->pic_date;
	unsigned int x, y;
    unsigned int cnt_fb ;
	unsigned int cnt_pic = 0;
	//unsigned int cnt_pic= ppic->width * ppic->height * 3 -1;//这里的-1是因为
															//从0开始计算,所以最后会多一位
	
	for (y=0; y < ppic->height; y++)
	{
        for (x=0; x < ppic->width; x++)
        {
			cnt_fb = WIDTH* y + x;  //注意这里是width!!!!是屏幕的宽，在这里犯错误了

//			*(p+cnt_fb) = ((pdate[cnt_pic-2]<<0) |(pdate[cnt_pic-1]<<8) | (pdate[cnt_pic-0]<<16));
			*(p+cnt_fb) = ((pdate[cnt_pic+2]<<0) |(pdate[cnt_pic+1]<<8) | (pdate[cnt_pic+0]<<16));

			//cnt_pic -= 3;
			cnt_pic += 3;
		}
	}
}


void fb_draw_png(pic_info *ppic)
{
    unsigned int *p = g_pFBMem;
	const char *pdate = ppic->pic_date;
	unsigned int x, y;
    unsigned int cnt_fb ;
	unsigned int cnt_pic = 0;
	//unsigned int cnt_pic= ppic->width * ppic->height * 3 -1;//这里的-1是因为
															//从0开始计算,所以最后会多一位
	
	for (y=0; y < ppic->height; y++)
	{
        for (x=0; x < ppic->width; x++)
        {
			cnt_fb = WIDTH* y + x;  //注意这里是width!!!!是屏幕的宽，在这里犯错误了

//			*(p+cnt_fb) = ((pdate[cnt_pic-2]<<0) |(pdate[cnt_pic-1]<<8) | (pdate[cnt_pic-0]<<16));
			*(p+cnt_fb) = ((pdate[cnt_pic+2]<<0) |(pdate[cnt_pic+1]<<8) | (pdate[cnt_pic+0]<<16));

			//cnt_pic -= 3;
			cnt_pic += 3;
		}
	}
}


#if 0
/*
 *function:show the picture which is created by Img2Lcd.
 *        
 */
void fb_draw_jpgpicture(void)
{
    unsigned int *p = g_pFBMem;
	const unsigned char *pdate = gImage_1024600;
	//const unsigned char *pdate = gImage_19201080;
	unsigned int i, j;
    unsigned int cnt;
	
	for (i=0; i<HEIGHT; i++)
	{
        for (j=0; j<WIDTH; j++)
        {
			cnt = WIDTH* i + j;
			cnt *= 3;

			*p = ((pdate[cnt+0]<<0) |(pdate[cnt+1]<<8) | (pdate[cnt+2]<<16));
			p++;
		}
	}
}

/*
 *function:show the picture which is smaller than screen.
 *         the screen:1020*600 and the picture:500*281
 */
void fb_draw_smallpicture(void)
{
    unsigned int *p = g_pFBMem;
	const unsigned char *pdate = gImage_500281;
	unsigned int x, y;
    unsigned int cnt_fb, cnt_pic=0;
	
	for (y=0; y<281; y++)
	{
        for (x=0; x<500; x++)
        {
			cnt_fb = WIDTH* y + x;

			*(p+cnt_fb) = ((pdate[cnt_pic+0]<<0) |(pdate[cnt_pic+1]<<8) | (pdate[cnt_pic+2]<<16));
			cnt_pic += 3;
			
		}
	}
}

/*
 *function:show the small picture in the random position.
 *         the screen:1020*600 and the small picture:500*281
 *parameter:(x0,y0),starting coordinates of the picture
 */
void fb_draw_picture_randomposition(unsigned int x0, unsigned int y0)
{
    unsigned int *p = g_pFBMem;
	const unsigned char *pdate = gImage_500281;
	unsigned int x, y;
    unsigned int cnt_fb, cnt_pic=0;
	
	for (y=0; y<281; y++)
	{
        for (x=0; x<500; x++)
        {
			cnt_fb = WIDTH* (y+y0) + (x+x0);

			*(p+cnt_fb) = ((pdate[cnt_pic+0]<<0) |(pdate[cnt_pic+1]<<8) | (pdate[cnt_pic+2]<<16));
			cnt_pic += 3;
		}

	}
}

/*
 *function:when showing the picture if it beyond the screen's bounds,
 *			skip the pixels that beyond the bounds.
 *         the screen:1020*600 and the small picture:500*281
 *parameter:(x0,y0),starting coordinates of the picture
 */
void fb_draw_picture_beyond_bounds(unsigned int x0, unsigned int y0)
{
    unsigned int *p = g_pFBMem;
	const unsigned char *pdate = gImage_500281;
	unsigned int x, y;
    unsigned int cnt_fb, cnt_pic=0;
	
	for (y=0; y<281; y++)
	{

		if((y+y0) >= HEIGHT)
			break;				//刚开始自己写的continue，但是这里直接break就可以了
								//，因为已经到边界外了，剩下的直接退出就可以了		
        for (x=0; x<500; x++)
        {
			if((x+x0)>=WIDTH)
			{
				cnt_pic += 3; 		//skip the pixels that beyond the bounds
				continue;
			}
	
			cnt_fb = WIDTH* (y+y0) + (x+x0);

			*(p+cnt_fb) = ((pdate[cnt_pic+0]<<0) |(pdate[cnt_pic+1]<<8) | (pdate[cnt_pic+2]<<16));
			cnt_pic += 3;
		}

	}
}
#endif















