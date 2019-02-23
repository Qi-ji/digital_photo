#include <stdio.h>
#include <setjmp.h>
#include <string.h>

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
#include <Pic_manager.h>

#include "jpeglib.h"
#include "jerror.h"



struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */
  jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}



static int isJpg(PT_FileMap ptFileMap);
static int GetPixelDatasFrmJPG(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas);
static int FreePixelDatasFrmJPG(PT_PixelDatas ptPixelDatas);


T_PicFilePraser tJpgFilePraser = {
	.name = "jpg",
	.isSupport = isJpg,
	.GetPixelDatas = GetPixelDatasFrmJPG,
	.FreePixelDatas = FreePixelDatasFrmJPG,
};

/**********************************************************************
 * 函数名称： isJpg
 * 功能描述： 是否是Jpeg图片
 * 输入参数： ptFileMap - 内含文件信息(使用里面的文件句柄)
 * 输出参数： 无
 * 返 回 值： 1 - 支持, 0 - 不支持
 ***********************************************************************/
static int isJpg(PT_FileMap ptFileMap)
{

	unsigned char *pucFileHead;
	pucFileHead = ptFileMap->pucFileMapMem;
	
	debug("the first two bite: 0X%x%x\n", pucFileHead[0], pucFileHead[1]);
	if((pucFileHead[0] != 0xff) || (pucFileHead[1] != 0xd8))
	{
		return 0;
	}
	return 1;
}

/**********************************************************************
 * 函数名称： CoverOneLine
 * 功能描述： 把BMP文件中的一行象素数据进行复制(bpp、与数据顺序的改变)
 * 输入参数： Number        - 图片中每行的数据个数
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
		printf("we can not support this pic which bpp = %d.\n", PicBpp);
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
				dwColor = ((dwBlue << 16) | (dwGreen << 8) | (dwRed));
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
 * 函数名称： GetPixelDatasFrmJPG
 * 功能描述： 把BMP文件中的象素数据,取出并转换为能在显示设备上使用的格式
 * 输入参数： ptFileMap    - 内含文件信息
 * 输出参数： ptPixelDatas - 内含象素数据
 *            ptPixelDatas->iBpp 是输入的参数, 它确定从BMP文件得到的数据要转换为该BPP
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
static int GetPixelDatasFrmJPG(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	
    //JSAMPARRAY buffer;           								 /* Output row buffer */
    unsigned char *pucLinebuffer;					/*指向一行的数据*/
    int iRowStride;               					/* physical row width in output buffer */
	int iJpgBpp;										/*图片自己的bpp，我们规定只有bpp=24的照片可以使用*/
	unsigned char *pucDestDatas;

	/* Step 1: allocate and initialize JPEG decompression object */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) 
	{
	  /* If we get here, the JPEG code has signaled an error.
	   * We need to clean up the JPEG object, close the input file, and return.
	   */
	  jpeg_destroy_decompress(&cinfo);
	  return -1;
	}
								
	jpeg_create_decompress(&cinfo);				/* initialize the JPEG decompression object. */
    jpeg_stdio_src(&cinfo, ptFileMap->pFd);				/* Step 2: specify data source (eg, a file) */
    (void) jpeg_read_header(&cinfo, TRUE);		/* Step 3: read file parameters with jpeg_read_header() */  
   												/* Step 4: set parameters for decompression */	
    (void) jpeg_start_decompress(&cinfo);			/* Step 5: Start decompressor */
												
	iRowStride = cinfo.output_width * cinfo.output_components; 				/*jpg图片中一行数据的大小*/
	ptPixelDatas->iWidth = cinfo.output_width;
	ptPixelDatas->iHeight = cinfo.output_height;
	iJpgBpp = cinfo.output_components * 8;
	//buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	pucLinebuffer = malloc(iRowStride);
	if (NULL == pucLinebuffer)
	{
		return -1;
	}
	
	debug("the jpg height is %d.\n",ptPixelDatas->iHeight);
	debug("the jpg width is %d.\n",ptPixelDatas->iWidth);
	debug("the jpg bpp is %d.\n",iJpgBpp);
	
	ptPixelDatas->iLineByte = ptPixelDatas->iWidth * ptPixelDatas->ibpp / 8; /*设备中一行的数据大小*/
	ptPixelDatas->iTotalByte = ptPixelDatas->iHeight * ptPixelDatas->iLineByte;
	ptPixelDatas->pucPixelDatas = malloc(ptPixelDatas->iTotalByte); 
	if (NULL == ptPixelDatas->pucPixelDatas)
	{
		return -1;
	}

	pucDestDatas = ptPixelDatas->pucPixelDatas;

	 /* Step 6: while (scan lines remain to be read) */
	while (cinfo.output_scanline < cinfo.output_height) 
	{
		 jpeg_read_scanlines(&cinfo, &pucLinebuffer, 1);
		 /* Assume put_scanline_someplace wants a pointer and sample count. */
		 CoverOneLine(iRowStride, pucLinebuffer, pucDestDatas, iJpgBpp, ptPixelDatas->ibpp);
		// memcpy(pjpeg->pic_date + (cinfo.output_scanline * iRowStride), buffer, iRowStride);//这句代码要好好看一下，很关键
		 															//是每次将pic_date后移一行数据的大小
		pucDestDatas += ptPixelDatas->iLineByte;
	 }

	jpeg_finish_decompress(&cinfo);	
	jpeg_destroy_decompress(&cinfo); 
	free(pucLinebuffer);

	return 0;
}

/**********************************************************************
 * 函数名称： FreePixelDatasFrmJPG
 * 功能描述： 释放像素数据结构体中的像素数据
 * 输入参数： ptPixelDatas - 内含象素数据
 * 输出参数： 
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
static int FreePixelDatasFrmJPG(PT_PixelDatas ptPixelDatas)
{
	free(ptPixelDatas->pucPixelDatas);
	return 0;
}

/**********************************************************************
 * 函数名称： JPGPraserInit
 * 功能描述： 将tBmpFilePraser结构体注册到链表中
 * 输入参数： ptPixelDatas - 内含象素数据
 * 输出参数： 
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int JPGPraserInit(void)
{
	return PicPraserRegister(&tJpgFilePraser);
}











/*
 *function:charge if it is a jpg picture;
 *para: the path and name of the picture
 *return:0--not jpg,1--is jpg, -1--error
 */

int is_jpg(const char *path)
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
	if((buf[0] != 0xff) || (buf[1] != 0xd8))
	{
		fclose(file);
		return 0;
	}
	/*不用检查后面两位了，因为发现自己下载的jpg文件，最后是ffd9 0a；
	ret = fseek(file, -3, SEEK_END);
	ret = fread(buf, 1, 2, file);
	debug("the last two bite: 0X%x%x\n", buf[0], buf[1]);
	if((buf[0] != 0xff) || (buf[1] != 0xd9))
	{
		fclose(file);
		return 0;
	}
	*/
	
	fclose(file);
	return 1;
	
}


static int jpg_analyze (pic_info  *pjpeg)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	
	FILE * infile;                /* source file */
    //JSAMPARRAY buffer;            /* Output row buffer */
    char *buffer;					//只有这样才能正常显示
    int row_stride;               /* physical row width in output buffer */

	if ((infile = fopen(pjpeg->pathname, "rb")) == NULL) {
	  fprintf(stderr, "can't open %s\n", pjpeg->pathname);
	  return -1;
     }
	/* Step 1: allocate and initialize JPEG decompression object */
	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
	  /* If we get here, the JPEG code has signaled an error.
	   * We need to clean up the JPEG object, close the input file, and return.
	   */
	  jpeg_destroy_decompress(&cinfo);
	  fclose(infile);
	  return -1;
	}
								
	jpeg_create_decompress(&cinfo);				/* initialize the JPEG decompression object. */
    jpeg_stdio_src(&cinfo, infile);				/* Step 2: specify data source (eg, a file) */
    (void) jpeg_read_header(&cinfo, TRUE);		/* Step 3: read file parameters with jpeg_read_header() */  
   												/* Step 4: set parameters for decompression */	
    (void) jpeg_start_decompress(&cinfo);		/* Step 5: Start decompressor */
	row_stride = cinfo.output_width * cinfo.output_components;/* JSAMPLEs per row in output buffer */
	/* output_width: scaled image width */ /*  nominal image: width (from SOF marker)   */
	 /* Make a one-row-high sample array that will go away when done with image */
	//buffer = (*cinfo.mem->alloc_sarray)
			//((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	buffer = (char *)malloc(row_stride);
   
	 /* Step 6: while (scan lines remain to be read) */
	while (cinfo.output_scanline < cinfo.output_height) {
		 (void) jpeg_read_scanlines(&cinfo, &buffer, 1);
		 /* Assume put_scanline_someplace wants a pointer and sample count. */
		 memcpy(pjpeg->pic_date + \
		 (cinfo.output_scanline * row_stride), buffer, row_stride);//这句代码要好好看一下，很关键
		 															//是每次将pic_date后移一行数据的大小
		// put_scanline_someplace(buffer[0], row_stride);
	 }
	 
    (void) jpeg_finish_decompress(&cinfo);
	
	pjpeg->height = cinfo.output_height;
	pjpeg->width = cinfo.output_width;
	pjpeg->bpp = cinfo.output_components * 8;
	
    jpeg_destroy_decompress(&cinfo);   
	fclose(infile);

	debug("the jpg height is %d.\n",pjpeg->height);
	debug("the jpg width is %d.\n",pjpeg->width);
	debug("the jpg bpp is %d.\n",pjpeg->bpp);
	
	fb_draw_jpeg_test(pjpeg);

	return 0;
}
	
int jpg_display(const char *path)
{
	pic_info j_picture;
	int ret;
	
	if(NULL == path)
	{
		fprintf(stderr, "uselee pathname\n");
		return -1;
	}

	ret = is_jpg(path);
	if(ret != 1)
	{
		fprintf(stderr, "%s is not a jpg picture.\n", path);
		return -1;
	}
	
	j_picture.pathname = path;
	j_picture.pic_date= rgb_buf;

	ret = jpg_analyze(&j_picture);
	if(ret == -1)
	{
		fprintf(stderr, "jpeg_analyze error.\n");
		return -1;
	}

	return 0;
	
}




