#ifndef __PAGE_MANAGER_H__
#define __PAGE_MANAGER_H__

/*ҳ����ͼ������λ�ýṹ��*/
typedef struct IconLayout
{
	int iLeftTopX;
	int iLeftTopY;
	int iRightBotX;
	int iRightBotY;
	char *pcName; 
}T_IconLayout, *PT_IconLayout;

/*����ҳ������λ�ýṹ��*/
typedef struct PageLayout
{
	int iLeftTopX;
	int iLeftTopY;
	int iRightBotX;
	int iRightBotY;
	int iWidth;
	int iHeight;
	int iBpp;
	int iTotalByte;	/*�����ʱ��ÿ��ͼƬ�Դ˽����軭���ϲ����ڴ��ϣ��������������¼��ҳ���������Ǹ�ͼ����ܴ�С*/
	
	PT_IconLayout atIconLayout; 
}T_PageLayout, *PT_PageLayout;






#endif 

