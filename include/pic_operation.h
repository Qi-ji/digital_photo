#ifndef   __PIC_OPERATION_H__
#define   __PIC_OPERATION_H__
#include <config.h>
#include <file.h>




/***************************structure definition*************************************/		
//BMP file header
typedef struct 
{ 
	unsigned char    bfType[2];// ���� //�ļ�����
	unsigned long    bfSize;//�������� //λͼ��С
	unsigned short   bfReserved1;//    //λ0 
	unsigned short   bfReserved2;//    //λ0
	unsigned long    bfOffBits;// ���� //������ƫ����
} __attribute__((packed)) BitMapFileHeader;// //�ֽڶ������⣬ʹ���СΪ14�ֽ�


//BMP message header
typedef struct 
{ 
	unsigned long    biSize;// ���� ���������� // BitMapFileHeader �ֽ���
	long 			 biWidth;// ������������   //λͼ��� 
	long 			 biHeight;// ��������������//λͼ�߶ȣ���λ���򣬷�֮Ϊ��ͼ 
	unsigned short 	 biPlanes;// ������������  //ΪĿ���豸˵��λ��������ֵ�����Ǳ���Ϊ1
	unsigned short   biBitCount;// ��������    //˵��������/���أ�Ϊ1��4��8��16��24����32�� 
	unsigned long    biCompression;// ����     //ͼ������ѹ��������û��ѹ�������ͣ�BI_RGB 
	unsigned long    biSizeImage;// ��������   //˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ 
	long 			 biXPelsPerMeter;//��������//˵��ˮƽ�ֱ��� 
	long 			 biYPelsPerMeter;//��������//˵����ֱ�ֱ��� 
	unsigned long    biClrUsed;//������������  //˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ������
	unsigned long    biClrImportant;//�������� //��ͼ����ʾ����ҪӰ�����������0����Ҫ�� 
} __attribute__((packed)) BitMapInfoHeader;// 


typedef struct PixelDatas{
	int iWidth;			/**/
	int iHeight;
	int ibpp;			/*��ǰָ��Ϊ��ʾ�豸��bpp*/
	int iLineByte;		/*���豸��һ�е������ݴ�С�������豸bppΪ32����������ǰ���32bpp���һ�д�С*/
	int iTotalByte;		/*���豸��ͼƬ�������ݴ�С�������豸bppΪ32����������ǰ���32bpp����ܴ�С��Ϊmalloc��׼��*/	
	unsigned char *pucPixelDatas;		/*ָ��ת��Ϊ�豸bpp(32λ)ͼƬ�������ݣ�ʹ��֮ǰ��Ҫ��malloc��ʹ�ú�Ҫfree*/
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
 * �������ƣ� PicMerge
 * ���������� ��СͼƬ�ϲ����ͼƬ��
 * ��������� iX,iY      - СͼƬ�ϲ����ͼƬ��ĳ������, iX/iYȷ�������������Ͻ�����
 *            ptSmallPic - �ں�СͼƬ����������
 *            ptBigPic   - �ں���ͼƬ����������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 ***********************************************************************/
int PicMerge(int iX, int iY, PT_PixelDatas ptSmallPic, PT_PixelDatas ptBigPic);

/**********************************************************************
 * �������ƣ� PicZoom
 * ���������� ����ȡ����ֵ��������ͼƬ
 *            ע��ú���������ڴ���������ź��ͼƬ,�����Ҫ��free�����ͷŵ�
 *            "����ȡ����ֵ"��ԭ����ο�����"lantianyu520"������"ͼ�������㷨"
 * ��������� ptOriginPic - �ں�ԭʼͼƬ����������
 *            ptZoomPic    - �ں����ź��ͼƬ����������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int PicZoom(PT_PixelDatas ptOriginPic, PT_PixelDatas ptZoomPic);




#endif


