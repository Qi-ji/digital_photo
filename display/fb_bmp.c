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
 * �������ƣ� isBmp
 * ���������� �Ƿ���bmpͼƬ
 * ��������� ptFileMap - �ں��ļ���Ϣ(ʹ������ָ�����ݵ�ָ��)
 * ��������� ��
 * �� �� ֵ�� 1 - ֧��, 0 - ��֧��
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
 * �������ƣ� CoverOneLine
 * ���������� ��BMP�ļ��е�һ���������ݽ��и���(bpp��������˳��ĸı�)
 * ��������� Number        - ÿ�е����ݸ���
 *		 pucSrcDatas   - ָ��bmpͼƬ��ÿ��ԭʼ����
 *		 PicBpp	  - bmpͼƬ��bpp
 *		 DevicBpp	  - �豸��bpp
 * ��������� pucDestDatas  - ָ��fb�豸�ڴ�λ��
 *           
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
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
	if (PicBpp != 24)		/*ֻ֧��24��ͼƬ*/
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
 * �������ƣ� GetPixelDatasFrmBMP
 * ���������� ��BMP�ļ��е���������,ȡ����ת��Ϊ������ʾ�豸��ʹ�õĸ�ʽ
 * ��������� ptFileMap    - �ں��ļ���Ϣ
 * ��������� ptPixelDatas - �ں���������,�����л�ΪPT_PixelDatas �е� *pucPixelDatas ����malloc,����ڴ档
 *            ptPixelDatas->iBpp ������Ĳ���, ��ȷ����BMP�ļ��õ�������Ҫת��Ϊ��BPP
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
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
	int ibpp;		/*ͼƬ�Լ���bpp�����ǹ涨ֻ��bpp=24����Ƭ����ʹ��*/			
	int i = 0;

	unsigned char *pucSrcDatas;
	unsigned char *pucDestDatas;

	TheFileHead 	= ptFileMap->pucFileMapMem;
	ptBmpFileHeader = (BitMapFileHeader *)TheFileHead;
	ptBmpInfoHeader = (BitMapInfoHeader *)(TheFileHead + sizeof(BitMapFileHeader));

	ibfOffBits  = ptBmpFileHeader->bfOffBits;	//��ͼ�����ݵ�ƫ����
	iWidth 	    = ptBmpInfoHeader->biWidth;
	iHeight 	= ptBmpInfoHeader->biHeight;
	ibpp  		= ptBmpInfoHeader->biBitCount;	/*ͼƬ����bpp*/

	debug("iWidth = %d.\n", iWidth);
	debug("iHeight = %d.\n", iHeight);
		
	ptPixelDatas->iHeight = iHeight;
	ptPixelDatas->iWidth  = iWidth;
	//ptPixelDatas->ibpp    = ibpp;  /*�������豸��bpp��Ӧ���ڱ�����֮�ⶨ�塣*/
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
	iLineWidthAlign = (iLineWidthReal + 3) & ~0x3;   /* ��4ȡ�� */
	/* ʹ�ÿ��ֵ�ܱ�4����
	if ((LineByteWidth % 4) != 0)
	LineByteWidth += 4 - (LineByteWidth % 4);
	*/

	debug("iLineWidthReal = %d.\n", iLineWidthReal);
	debug("iLineWidthAlign = %d.\n", iLineWidthAlign);


	pucSrcDatas = TheFileHead + ibfOffBits;
	pucSrcDatas = pucSrcDatas + (iHeight -1) * iLineWidthAlign;
	
	pucDestDatas = ptPixelDatas->pucPixelDatas;
	/*******************************************************
	*	ע��bmpͼƬ�����½ǿ�ʼ��ȡ
		*			 ------------
		*			|		|	
		*			|  bmppic	|
		*			|		|
	*   �����￪ʼ�������->  ------------  
	*******************************************************/
	for(i=0; i<iHeight; i++)
	{
		/*����һ�е�����(��Ҫ���޸�bpp�Լ�����˳��)*/
		CoverOneLine(iLineWidthAlign, pucSrcDatas, pucDestDatas, ibpp, ptPixelDatas->ibpp);
		pucSrcDatas  -= iLineWidthAlign;
		pucDestDatas += ptPixelDatas->iLineByte;
	}
	
	return 0;
}

/**********************************************************************
 * �������ƣ� FreePixelDatasFrmBMP
 * ���������� �ͷ��������ݽṹ���е���������
 * ��������� ptPixelDatas - �ں���������
 * ��������� 
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
static int FreePixelDatasFrmBMP(PT_PixelDatas ptPixelDatas)
{
	free(ptPixelDatas->pucPixelDatas);
	return 0;
}

/**********************************************************************
 * �������ƣ� BMPPraserInit
 * ���������� ��tBmpFilePraser�ṹ��ע�ᵽ������
 * ��������� ptPixelDatas - �ں���������
 * ��������� 
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
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











