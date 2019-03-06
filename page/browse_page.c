#include <stdio.h>

#include <disp_manager.h>
#include <page_manager.h>
#include <render.h>

static T_IconLayout g_atMenuIconLayout[] = {
	{0,0,0,0,"up_button.bmp"},
	{0,0,0,0,"select_button.bmp"},
	{0,0,0,0,"prepage_button.bmp"},
	{0,0,0,0,"nextpage_button.bmp"},
	{0,0,0,0,NULL},
};

static T_PageLayout g_tPageLayout = {
	.iTotalByte = 0,
	.atIconLayout = g_atMenuIconLayout,
};

static char g_strCurDir[256] = DEFAULT_DIR;
//static char g_strSelectedDir[256] = DEFAULT_DIR;

/* ��������ĳĿ¼������� */
static PT_DirContent *g_aptDirContents;  /* ����:����Ŀ¼��"������Ŀ¼","�ļ�"������ */
static int g_iDirContentsNumber;         /* g_aptDirContents�����ж����� */
//static int g_iStartIndex = 0;            /* ����Ļ����ʾ�ĵ�1��"Ŀ¼���ļ�"��g_aptDirContents���������һ�� */

/**********************************************************************
 * �������ƣ� CalcBPMenuLayout
 * ���������� ����"���ҳ��"�Ĺ��ܲ˵�λ����Ϣ
 * ��������� ptParentPageParams - ҳ����Ϣ,��ҳ�湦�ܲ˵���Ϣ
 * ��������� ptParentPageParams - ҳ����Ϣ,���㷵��ҳ�湦�ܲ˵�λ����Ϣ
 * �� �� ֵ�� 
 ***********************************************************************/
static int CalcBPMenuLayout(PT_PageLayout ptPageLayout)
{
	
}

/**********************************************************************
 * �������ƣ� CalcBPInterfaceLayout
 * ���������� ����"���ҳ��"�Ľ���ͼ��λ����Ϣ���������ļ����Լ��ļ�ͼ�����Ϣ
 *		 ͨ��ȫ�ֱ��������ݺ���
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 
 ***********************************************************************/

static int CalcBPInterfaceLayout(void)
{

}

/**********************************************************************
 * �������ƣ� BrowsePageShow
 * ���������� "���ҳ��"����ʾ����
 * ��������� ptParentPageParams - ҳ����Ϣ
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
static void BrowsePageShow(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout;		/*ָ��4���˵�ͼ��*/

/*����Դ�*/
	ptVideoMem = GetVideoMem(GetPageId("browespage"), 1);
	if (ptVideoMem == NULL)
	{
		debug("can't get video mem for browse page!\n");
		return;
	}
/*����˵�ͼ��λ��*/
	if (atIconLayout[0].iLeftTopX == 0)
	{
		CalcBPMenuLayout(ptPageLayout);
		CalcBPInterfaceLayout();
	}

}

/**********************************************************************
 * �������ƣ� BrowsePageRun
 * ���������� "���ҳ��"�����к���: ��ʾ·���е�Ŀ¼���ļ�
 * ��������� ptParentPageParams - ҳ����Ϣ
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
static int BrowsePageRun(PT_PageParams ptParentPageParams)
{
	int iIndex;
	int iError;
	T_InputEvent tInputEvent;
	T_PageParams tPageParams;
	PT_DirContent aptDirContents;

	tPageParams.iPageID = GetPageId("browsepage");
	iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
	if (iError)
	{
		debug("GetDirContents error\n");
		return -1;
	}
	debug("g_iDirContentsNumber = %d.\n", g_iDirContentsNumber);

	BrowsePageShow(&g_tPageLayout);
	
}


T_PageAction g_tBPAction = {
	.name 			= "browsepage",
	.Run 			= BrowsePageRun,
};

/**********************************************************************
 * �������ƣ� MainPageInit
 * ���������� ע��"��ҳ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 ***********************************************************************/
int BrowsePageInit(void)
{
	return PageActionRegister(&g_tBPAction);
}






