#ifndef   __PIC_OPERATION_H__
#define   __PIC_OPERATION_H__
#include <config.h>
#include <file.h>




/***************************structure definition*************************************/		
//BMP file header
typedef struct 
{ 
	unsigned char    bfType[2];// 　　 //文件类型
	unsigned long    bfSize;//　　　　 //位图大小
	unsigned short   bfReserved1;//    //位0 
	unsigned short   bfReserved2;//    //位0
	unsigned long    bfOffBits;// 　　 //到数据偏移量
} __attribute__((packed)) BitMapFileHeader;// //字节对其问题，使其大小为14字节


//BMP message header
typedef struct 
{ 
	unsigned long    biSize;// 　　 　　　　　 // BitMapFileHeader 字节数
	long 			 biWidth;// 　　　　　　   //位图宽度 
	long 			 biHeight;// 　　　　　　　//位图高度，正位正向，反之为倒图 
	unsigned short 	 biPlanes;// 　　　　　　  //为目标设备说明位面数，其值将总是被设为1
	unsigned short   biBitCount;// 　　　　    //说明比特数/象素，为1、4、8、16、24、或32。 
	unsigned long    biCompression;// 　　     //图象数据压缩的类型没有压缩的类型：BI_RGB 
	unsigned long    biSizeImage;// 　　　　   //说明图象的大小，以字节为单位 
	long 			 biXPelsPerMeter;//　　　　//说明水平分辨率 
	long 			 biYPelsPerMeter;//　　　　//说明垂直分辨率 
	unsigned long    biClrUsed;//　　　　　　  //说明位图实际使用的彩色表中的颜色索引数
	unsigned long    biClrImportant;//　　　　 //对图象显示有重要影响的索引数，0都重要。 
} __attribute__((packed)) BitMapInfoHeader;// 


typedef struct PixelDatas{
	int iWidth;			/**/
	int iHeight;
	int ibpp;			/*提前指定为显示设备的bpp*/
	int iLineByte;		/*在设备上一行的总数据大小，例如设备bpp为32，则这里就是按照32bpp算的一行大小*/
	int iTotalByte;		/*在设备上图片的总数据大小，例如设备bpp为32，则这里就是按照32bpp算的总大小，为malloc做准备*/	
	unsigned char *pucPixelDatas;		/*指向转换为设备bpp(32位)图片像素数据，使用之前需要先malloc，使用后要free*/
}T_PixelDatas, *PT_PixelDatas;

typedef struct PicFilePraser{
	char *name;
	int (*isSupport)(PT_FileMap ptFileMap);
	int (*GetPixelDatas)(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas);
	int (*FreePixelDatas)(PT_PixelDatas ptPixelDatas);
	struct PicFilePraser *next;
}T_PicFilePraser, *PT_PicFilePraser;


/***************************function definition*************************************/
/**********************************************************************
 * 函数名称： PicMerge
 * 功能描述： 把小图片合并入大图片里
 * 输入参数： iX,iY      - 小图片合并入大图片的某个区域, iX/iY确定这个区域的左上角座标
 *            ptSmallPic - 内含小图片的象素数据
 *            ptBigPic   - 内含大图片的象素数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 ***********************************************************************/
int PicMerge(int iX, int iY, PT_PixelDatas ptSmallPic, PT_PixelDatas ptBigPic);

/**********************************************************************
 * 函数名称： PicZoom
 * 功能描述： 近邻取样插值方法缩放图片
 *            注意该函数会分配内存来存放缩放后的图片,用完后要用free函数释放掉
 *            "近邻取样插值"的原理请参考网友"lantianyu520"所著的"图像缩放算法"
 * 输入参数： ptOriginPic - 内含原始图片的象素数据
 *            ptZoomPic    - 内含缩放后的图片的象素数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int PicZoom(PT_PixelDatas ptOriginPic, PT_PixelDatas ptZoomPic);




#endif


