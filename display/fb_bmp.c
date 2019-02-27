#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#include <pic_operation.h>
#include <config.h>
#include <disp_manager.h>
#include <file.h>
#include <pic_manager.h>


static int isBmp(PT_FileMap ptFileMap);
static int GetPixelDatasFrmBMP(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas);
static int FreePixelDatasFrmBMP(PT_PixelDatas ptPixelDatas);


T_PicFilePraser tBmpFilePraser = {
	.name = "bmp",
	.isSupport = isBmp,
	.GetPixelDatas = GetPixelDatasFrmBMP,
	.FreePixelDatas = FreePixelDatasFrmBMP,
};

/**********************************************************************
 * 函数名称： isBmp
 * 功能描述： 是否是bmp图片
 * 输入参数： ptFileMap - 内含文件信息(使用里面指向数据的指针)
 * 输出参数： 无
 * 返 回 值： 1 - 支持, 0 - 不支持
 ***********************************************************************/
static int isBmp(PT_FileMap ptFileMap)
{
	//size_t ret = 0;
	//char buf[2] = {0};
	//ret = fread(buf, 1, 2, ptFileMap->pFd);
	unsigned char *pucFileHead;
	pucFileHead = ptFileMap->pucFileMapMem;
	
	debug("the first two bite: 0X%x%x\n", pucFileHead[0], pucFileHead[1]);
	if((pucFileHead[0] != 0x42) || (pucFileHead[1] != 0x4d))
	{
		return 0;
	}
	return 1;
}

/**********************************************************************
 * 函数名称： CoverOneLine
 * 功能描述： 把BMP文件中的一行象素数据进行复制(bpp、与数据顺序的改变)
 * 输入参数： Number        - 每行的数据个数
 *		 pucSrcDatas   - 指向bmp图片的每行原始数据
 *		 PicBpp	  - bmp图片的bpp
 *		 DevicBpp	  - 设备的bpp
 * 输出参数： pucDestDatas  - 指向fb设备内存位置
 *           
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
static int CoverOneLine(int Number, unsigned char *pucSrcDatas, 
						unsigned char *pucDestDatas, int PicBpp, int DevicBpp )
{
	unsigned int dwRed;
	unsigned int dwGreen;
	unsigned int dwBlue;
	unsigned int dwColor;
	
	//unsigned short *pucDest16Datas = (unsigned short *)pucDestDatas;
	unsigned int *pucDest32Datas = (unsigned int *)pucDestDatas;

	int i, index;
	if (PicBpp != 24)		/*只支持24的图片*/
	{
		debug("we can not support this pic which bpp = %d.\n", PicBpp);
		return -1;
	}

	if (DevicBpp == 24)
	{
		memcpy(pucDestDatas, pucSrcDatas, Number);
	}
	else 
	{
		for(i=0; i < (Number / 3); i++)
		{
			dwBlue	 = pucSrcDatas[index++];
			dwGreen	 = pucSrcDatas[index++];
			dwRed	 = pucSrcDatas[index++];

			if (DevicBpp == 32)
			{
				dwColor = ((dwBlue ) | (dwGreen << 8) | (dwRed<< 16));
				*pucDest32Datas = dwColor;
				pucDest32Datas ++;
			}
			if (DevicBpp == 16)
			{
				return -1;	
			}
		}
	}
}

/**********************************************************************
 * 函数名称： GetPixelDatasFrmBMP
 * 功能描述： 把BMP文件中的象素数据,取出并转换为能在显示设备上使用的格式
 * 输入参数： ptFileMap    - 内含文件信息
 * 输出参数： ptPixelDatas - 内含象素数据,函数中会为PT_PixelDatas 中的 *pucPixelDatas 进行malloc,获得内存。
 *            ptPixelDatas->iBpp 是输入的参数, 它确定从BMP文件得到的数据要转换为该BPP
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
static int GetPixelDatasFrmBMP(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas)
{
	unsigned char *TheFileHead;
	BitMapFileHeader *ptBmpFileHeader;
	BitMapInfoHeader *ptBmpInfoHeader;

	int iLineWidthReal;
	int iLineWidthAlign;

	int ibfOffBits;
	int iWidth;
	int iHeight;
	int ibpp;		/*图片自己的bpp，我们规定只有bpp=24的照片可以使用*/			
	int i = 0;

	unsigned char *pucSrcDatas;
	unsigned char *pucDestDatas;

	TheFileHead 	= ptFileMap->pucFileMapMem;
	ptBmpFileHeader = (BitMapFileHeader *)TheFileHead;
	ptBmpInfoHeader = (BitMapInfoHeader *)(TheFileHead + sizeof(BitMapFileHeader));

	ibfOffBits  = ptBmpFileHeader->bfOffBits;	//到图像数据的偏移量
	iWidth 	    = ptBmpInfoHeader->biWidth;
	iHeight 	= ptBmpInfoHeader->biHeight;
	ibpp  		= ptBmpInfoHeader->biBitCount;	/*图片自身bpp*/

	debug("iWidth = %d.\n", iWidth);
	debug("iHeight = %d.\n", iHeight);
		
	ptPixelDatas->iHeight = iHeight;
	ptPixelDatas->iWidth  = iWidth;
	//ptPixelDatas->ibpp    = ibpp;  /*这里是设备的bpp，应该在本函数之外定义。*/
	ptPixelDatas->iLineByte = iWidth * ptPixelDatas->ibpp / 8;  
	debug("ptPixelDatas->ibpp = %d.\n", ptPixelDatas->ibpp);
	ptPixelDatas->iTotalByte = ptPixelDatas->iLineByte * iHeight;
	debug("ptPixelDatas->iTotalByte = %d.\n", ptPixelDatas->iTotalByte);
	
	ptPixelDatas->pucPixelDatas = malloc(ptPixelDatas->iTotalByte);
	if(NULL == ptPixelDatas->pucPixelDatas)
	{
		return -1;
	}
	iLineWidthReal = iWidth * ibpp / 8;
	iLineWidthAlign = (iLineWidthReal + 3) & ~0x3;   /* 向4取整 */
	/* 使该宽度值能被4整出
	if ((LineByteWidth % 4) != 0)
	LineByteWidth += 4 - (LineByteWidth % 4);
	*/

	debug("iLineWidthReal = %d.\n", iLineWidthReal);
	debug("iLineWidthAlign = %d.\n", iLineWidthAlign);


	pucSrcDatas = TheFileHead + ibfOffBits;
	pucSrcDatas = pucSrcDatas + (iHeight -1) * iLineWidthAlign;
	
	pucDestDatas = ptPixelDatas->pucPixelDatas;
	/*******************************************************
	*	注意bmp图片从左下角开始读取
		*			 ------------
		*			|		|	
		*			|  bmppic	|
		*			|		|
	*   从这里开始描绘数据->  ------------  
	*******************************************************/
	for(i=0; i<iHeight; i++)
	{
		/*复制一行的数据(主要是修改bpp以及数据顺序)*/
		CoverOneLine(iLineWidthAlign, pucSrcDatas, pucDestDatas, ibpp, ptPixelDatas->ibpp);
		pucSrcDatas  -= iLineWidthAlign;
		pucDestDatas += ptPixelDatas->iLineByte;
	}
	
	return 0;
}

/**********************************************************************
 * 函数名称： FreePixelDatasFrmBMP
 * 功能描述： 释放像素数据结构体中的像素数据
 * 输入参数： ptPixelDatas - 内含象素数据
 * 输出参数： 
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
static int FreePixelDatasFrmBMP(PT_PixelDatas ptPixelDatas)
{
	free(ptPixelDatas->pucPixelDatas);
	return 0;
}

/**********************************************************************
 * 函数名称： BMPPraserInit
 * 功能描述： 将tBmpFilePraser结构体注册到链表中
 * 输入参数： ptPixelDatas - 内含象素数据
 * 输出参数： 
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int BMPPraserInit(void)
{
	return PicPraserRegister(&tBmpFilePraser);
}


int is_bmp(const char *path)
{
	FILE *file = NULL;
	size_t ret = 0;
	char buf[2] = {0};
	
	if(NULL == path)
	{
		fprintf(stderr, "uselee pathname\n");
		return -1;
	}


	file = fopen(path, "r");
	if(NULL == file)
	{
		fprintf(stderr, "open file %s error.\n", path);
		fclose(file);
		return -1;
	}
	
	ret = fread(buf, 1, 2, file);
	debug("the first two bite: 0X%x%x\n", buf[0], buf[1]);
	if((buf[0] != 'B') || (buf[1] != 'M'))
	{
		fclose(file);
		return 0;
	}
	
	fclose(file);
	return 1;
	
}

/*
 *function:
 *parameter:the path of your bmp path.eg:"123.bmp"
 *return:-1 is error.
 */
static int bmp_analyze(pic_info  *pbmp)
{
	int fd = -1;
	BitMapFileHeader bmp_header;
	BitMapInfoHeader bmp_info;
	ssize_t ret = 0;
	unsigned long bmp_date_len = 0;
	
	if(NULL == pbmp)
		return -1;

	fd = open(pbmp->pathname, O_RDONLY);
	if(fd == -1)
	{
		debug("open %s failed.\n", pbmp->pathname);
		return -1;
	}

	 ret = read(fd, &bmp_header, sizeof(bmp_header));
	 ret = read(fd, &bmp_info, sizeof(bmp_info)); 
	 if(ret < 0)
	 {
		debug("read %s head massage failed.\n", pbmp->pathname);
		close(fd);
		return -1;
	 }
	 
	 //lseek(fd, bmp_header.bfOffBits, SEEK_SET);
	 bmp_date_len = bmp_info.biWidth * bmp_info.biHeight * bmp_info.biBitCount / 3;
	 ret = read(fd, rgb_buf, bmp_date_len);

	 pbmp->width = bmp_info.biWidth;
	 pbmp->height = bmp_info.biHeight;
	 pbmp->bpp = bmp_info.biBitCount;
	 //pbmp->pic_date = rgb_buf;
/*
	if(bmp_head_buf[0]!='B' || bmp_head_buf[1]!='M')
	{
	    debug("%s is not a bmp picture.\n", path);
		return -1;	
	}
*/
	debug("the bmp bfSize is %ld\n", bmp_header.bfSize);
	debug("the bmp biHeight is %d\n",pbmp->height);
	debug("the bmp biWidth is %d\n",pbmp->width);	

	fb_draw_picture_test(pbmp);
	
	close(fd);
	
	return 0;
}


	
int bmp_display(const char *path)
{
	pic_info b_picture;
	int ret;
	
	if(NULL == path)
	{
		fprintf(stderr, "uselee pathname\n");
		return -1;
	}

	ret = is_bmp(path);
	if(ret != 1)
	{
		fprintf(stderr, "%s is not a bmp picture.\n", path);
		return -1;
	}
	
	b_picture.pathname =(char *)path;
	b_picture.pic_date= rgb_buf;

	ret = bmp_analyze(&b_picture);
	if(ret == -1)
	{
		fprintf(stderr, "jpeg_analyze error.\n");
		return -1;
	}

	return 0;
	
}











