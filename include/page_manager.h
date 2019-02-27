#ifndef __PAGE_MANAGER_H__
#define __PAGE_MANAGER_H__

#include <disp_manager.h>
#include <page_manager.h>


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


/**********************************************************************
 * �������ƣ� GetPageId
 * ���������� ���ҳ��ר����ID
 * ��������� pcPageName - ҳ������ ����main brose setting ��
 * ��������� ��
 * �� �� ֵ�� ҳ��ר��ID
 ***********************************************************************/
int GetPageId(char *pcPageName);

/**********************************************************************
 * �������ƣ� GeneratePage
 * ���������� ��ͼ���ļ��н�����ͼ�����ݲ�����ָ������,�Ӷ�����ҳ������
 * ��������� ptPageLayout - �ں����ͼ����ļ�������ʾ����
 *            ptVideoMem   - �����VideoMem�ﹹ��ҳ������
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem);

/**********************************************************************
 * �������ƣ� MainPageRun
 * ���������� "��ҳ��"�����к���: ��ʾ�˵�ͼ��,�������û��Դ���������������Ӧ
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void MainPageRun(void);


#endif 

