#ifndef __DISP_MANAGER_H__
#define __DISP_MANAGER_H__

#include <config.h>
#include <pic_operation.h>
// �궨��
#define FBDEVICE	"/dev/fb0"

#define WIDTH		1024	
#define HEIGHT		600

#define WHITE		0xffffffff			// test ok
#define BLACK		0x00000000
#define RED			0xffff0000
#define GREEN		0xff00ff00			// test ok
#define BLUE		0xff0000ff			

#define GREENP		0x0000ff00			// һ����˵��ǰ2��ff͸��λ��������

typedef struct DispOpr{
	char 			*name;				/*��ʾ�豸����*/
	int  			iXres;				/*X����ֱ���*/
	int  			iYres;				/*X����ֱ���*/
	int				ibpp;
	int				iMemTotalByte;
	unsigned int 	*pdwDispMem; 		/*mmap ��õ��Դ��ַ*/
	int(*DeviceInit)(void);
	int(*DeviceClose)(void);
	int(*ShowOnePiexl)(void);
	void(*CleanScreen)(unsigned int icolor);
	int (*ShowPixel)(PT_PixelDatas ptPixelDatas);
	int(*ShowOnePage)(PT_PixelDatas ptPixelDatas);
	struct DispOpr *next;
}T_DispOpr, *PT_DispOpr;


/**********************************************************************
 * �������ƣ� DispOprRegister
 * ���������� ע�����е���ʾ�豸������
 * ���������
 * ��������� 
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int DispOprRegister(PT_DispOpr ptDispOpr);

/**********************************************************************
 * �������ƣ� FBRegister
 * ���������� ע��FB��ʾ�豸������
 * ��������� 
 * ��������� 
 * �� �� ֵ�� 
 **********************************************************************/
int FBRegister(void);

/**********************************************************************
 * �������ƣ� ShowDispOpr
 * ���������� ��ӡ����ע�������ʾ�豸
 * ��������� 
 * ��������� 
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
 int ShowDispOpr(void);

/**********************************************************************
 * �������ƣ� GetDispOpr
 * ���������� ������ʾ�豸���ƣ���ø��豸��ʾģ�飬����:����"fb",���fbģ��ṹ��
 * ��������� pcname - ��ʾ�豸����
 * ��������� ��
 * �� �� ֵ��NULL   - ʧ��,û��ָ����ģ��, 
 *		 PT_DispOpr - ��ʾģ��ṹ��ָ��
 ***********************************************************************/
PT_DispOpr GetDispOpr(char *pcname);

/**********************************************************************
 * �������ƣ� SetAndInitDefaultDispOpr
 * ���������� ѡ��Ĭ�ϵ���ʾ�豸������ʼ�����豸
 * ��������� pcname - ��ʾ�豸����
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void SetAndInitDefaultDispOpr(char *pcname);

/**********************************************************************
 * �������ƣ� GetDefaultDispOpr
 * ���������� ����Ĭ�ϵ���ʾ�豸
 * ��������� pcname - ��ʾ�豸����
 * ��������� ��
 * �� �� ֵ�� PT_DispOpr
 ***********************************************************************/
PT_DispOpr GetDefaultDispOpr(void);

/**********************************************************************
 * �������ƣ� GetDefaultDispOpr
 * ���������� ����Ĭ�ϵ���ʾ�豸
 * ��������� pcname - ��ʾ�豸����
 * ��������� ��
 * �� �� ֵ�� PT_DispOpr
 ***********************************************************************/
int GetDefDispResolution(int *piXres, int *piYres, int *piBpp);







// ��������
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






