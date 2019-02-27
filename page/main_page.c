#include <stdio.h>

#include <disp_manager.h>
#include <page_manager.h>

static PT_IconLayout g_atMainPageIconLayout[] = {
	{0,0,0,0,"browse_button.bmp"},
	{0,0,0,0,"continue_button.bmp"},
	{0,0,0,0,"setting_button.bmp"},
	{0,0,0,0,NULL},
};

static T_PageLayout g_tPageLayout = {
	.iTotalByte = 0,
	.atIconLayout = g_atMainPageIconLayout,
};


/**********************************************************************
 * �������ƣ� CalcMainPageLayout
 * ���������� ������ҳ���и�ͼ������ֵ
 * ��������� ��
 * ��������� ptPageLayout - �ں���ͼ������Ͻ�/���½�����ֵ
 * �� �� ֵ�� ��
 ***********************************************************************/
static int  CalcMainPageLayout(PT_PageLayout ptPageLayout)
{
	if (NULL == ptPageLayout || NULL == ptPageLayout->atIconLayout)
		return -1;
		
	int iXres, iYres, iBpp;  /*��ʾ�豸�Ĳ���*/
	int iIconWidth, iIconHeight; /*ÿ��ͼ��Ŀ��߶���һ���ģ����Լ����һ��ͼ��Ϳ�����*/
	int iIconTotalByte;			/*ÿ��ͼ����ܴ�С����һ����*/
	PT_IconLayout atIconLayout;

	GetDefDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;
	atIconLayout = ptPageLayout->atIconLayout;

	/*browser_button.jpg ͼ��λ�ü���*/
	atIconLayout[0].iLeftTopX = iXres * 1/3;
	atIconLayout[0].iLeftTopY = iYres * 1/10;
	atIconLayout[0].iRightBotX = iXres * 2/3;
	atIconLayout[0].iRightBotY = iYres * 3/10;

		/* �����ҳ����Byte �� */
	iIconWidth = atIconLayout[0].iRightBotX - atIconLayout[0].iLeftTopX +1; /*ע�����������Ҫ��1 ��*/
	iIconHeight = atIconLayout[0].iRightBotY - atIconLayout[0].iLeftTopY +1;
	iIconTotalByte = iIconWidth * iIconHeight * iBpp / 8;

	if (ptPageLayout->iTotalByte < iIconTotalByte)
	{
		ptPageLayout->iTotalByte = iIconTotalByte;
	}
	
	/*continue_button.jpg ͼ��λ�ü���*/
	atIconLayout[1].iLeftTopX = iXres * 1/3;
	atIconLayout[1].iLeftTopY = iYres * 4/10;
	atIconLayout[1].iRightBotX = iXres * 2/3;
	atIconLayout[1].iRightBotY = iYres * 6/10;
	
	/*setting_button.jpg ͼ��λ�ü���*/
	atIconLayout[2].iLeftTopX = iXres * 1/3;
	atIconLayout[2].iLeftTopY = iYres * 7/10;
	atIconLayout[2].iRightBotX = iXres * 2/3;
	atIconLayout[2].iRightBotY = iYres * 9/10;
	return 0;
}

/**********************************************************************
 * �������ƣ� MainPageShow
 * ���������� "��ҳ��"����ʾ����������ҳ���ϵ�3��ͼ�������ʾ
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void MainPageShow(PT_PageLayout ptMainPageLayout)
{
	/* 1.��û���������ҳ������*/
	/* 2.��ҳ����ͼ��δ����λ�õĻ�������ͼ��λ��*/
	/* 3.����ͼ������(���ͼ�����ء��������š��ϲ��Ȳ���)*/
	/* 4.ˢ���豸�Ͻ�����ʾ*/
	/* 5.�ͷ������ʱ�ڴ�*/
	PT_VideoMem ptMainPageMem;
	PT_IconLayout atIconLayout;
	atIconLayout = ptMainPageLayout->atIconLayout;
debug("00\n");
/* 1.��û���������ҳ������*/
	ptMainPageMem = GetVideoMem(GetPageId("main"), 1);   /* 1 ��ʾ��������ڴ���Ϊ��ǰ�߳�ʹ��*/
debug("11\n");
/* 2.��ҳ����ͼ��δ����λ�õĻ�������ͼ��λ��*/

	if(atIconLayout[0].iLeftTopX == 0)
	{
		CalcMainPageLayout(ptMainPageLayout);
	}
debug("22\n");	
/* 3.����ͼ������(���ͼ�����ء��������š��ϲ��Ȳ���)*/
	GeneratePage(ptMainPageLayout, ptMainPageMem);
debug("33\n");

/* 4.ˢ���豸�Ͻ�����ʾ*/
	FlushVideoMemToDev(ptMainPageMem);
debug("44\n");

}


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
void MainPageRun(void)
{
	/*��ʾҳ��*/
	MainPageShow(&g_tPageLayout);

	/*׼�����̣߳�Ԥ����һҳ��*/

	/*��ô������������¼�*/

}






