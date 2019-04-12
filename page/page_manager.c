#include <stdio.h>
#include <stdlib.h>


#include <config.h>
#include <pic_manager.h>
#include <pic_operation.h>
#include <page_manager.h>
#include <render.h>
#include <input_manager.h>


PT_PageAction g_ptPageActionHead;


/**********************************************************************
 * �������ƣ� RegisterPageAction
 * ���������� ע��"ҳ��ģ��", "ҳ��ģ��"����ҳ����ʾ�ĺ���
 * ��������� ptPageAction - һ���ṹ��,�ں�"ҳ��ģ��"�Ĳ�������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 ***********************************************************************/
int PageActionRegister(PT_PageAction ptPageAction)
{
	PT_PageAction ptTmp;

	if (!g_ptPageActionHead)
	{
		g_ptPageActionHead   = ptPageAction;
		ptPageAction->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPageActionHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptPageAction;
		ptPageAction->ptNext = NULL;
	}

	return 0;
}

/**********************************************************************
 * �������ƣ� GetPageId
 * ���������� ���ҳ��ר����ID
 * ��������� pcPageName - ҳ������ ����main brose setting ��
 * ��������� ��
 * �� �� ֵ�� ҳ��ר��ID
 ***********************************************************************/
int GetPageId(char *pcPageName)
{
	return (pcPageName[0] + pcPageName[1] + pcPageName[2] + pcPageName[3]);
}

/**********************************************************************
 * �������ƣ� GeneratePage
 * ���������� ��ͼ���ļ��н�����ͼ�����ݲ�����ָ������,�Ӷ�����ҳ������
 * ��������� ptPageLayout - �ں����ͼ����ļ�������ʾ����
 *            ptVideoMem   - �����VideoMem�ﹹ��ҳ������
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem)
{
debug("***************************star GeneratePage***************************\n");
	int iError;
/* 1.����ͼ������(���ͼ�����ء��������š��ϲ��Ȳ���)*/
	PT_IconLayout atIconLayout;
	atIconLayout = ptPageLayout->atIconLayout;	/* ָ��ͼ������*/
	T_PixelDatas tOriginalPixelDatas;
	T_PixelDatas tZoonPixelDatas;				/*��ʱ����ͼ�����ź�����ݣ����ںϲ����Դ���*/

	if (ptVideoMem->eMemContent != VMC_GENERATE)
	{
		ClearVideoMem(ptVideoMem, BLACK);
		
		tOriginalPixelDatas.ibpp = ptPageLayout->iBpp; /*bpp ��Ϣ��Ҫ��ǰ�趨*/
		tZoonPixelDatas.ibpp 	 = ptPageLayout->iBpp; /*bpp ��Ϣ��Ҫ��ǰ�趨*/
		tZoonPixelDatas.pucPixelDatas = malloc(ptPageLayout->iMaxTotalByte);
		if (NULL == tZoonPixelDatas.pucPixelDatas)
			return -1;
	
		while (atIconLayout->pcName)
		{
debug("***************************star GetPixelFrmIcon***************************\n");
			/* ���ͼ����������*/
			iError = GetPixelFrmIcon(atIconLayout->pcName, &tOriginalPixelDatas);
			if (iError)
			{
				debug("GetPixelFrmIcon: %s error.\n", atIconLayout->pcName);
				free(tZoonPixelDatas.pucPixelDatas);
				return -1;
			}
debug("***************************end GetPixelFrmIcon***************************\n");

			/* ������ź�ͼ���������ݣ�ͨ��atIconLyout�����ͼ��ĳ������Ϣ*/
			tZoonPixelDatas.iHeight 	= atIconLayout->iRightBotY - atIconLayout->iLeftTopY ;
			tZoonPixelDatas.iWidth 		= atIconLayout->iRightBotX - atIconLayout->iLeftTopX ;
			tZoonPixelDatas.iLineByte	= tZoonPixelDatas.iWidth * tZoonPixelDatas.ibpp / 8;
			tZoonPixelDatas.iTotalByte  = tZoonPixelDatas.iLineByte * tZoonPixelDatas.iHeight;
debug("***************************start PicZoom***************************\n");

			debug("tZoonPixelDatas.iHeight = %d.\n", tZoonPixelDatas.iHeight);
			debug("tZoonPixelDatas.iWidth = %d.\n", tZoonPixelDatas.iWidth);
			debug("tZoonPixelDatas.iLineByte = %d.\n", tZoonPixelDatas.iLineByte);
			debug("tZoonPixelDatas.iTotalByte = %d.\n", tZoonPixelDatas.iTotalByte);
			/*����ͼ�겢�ϲ����Դ���*/
			PicZoom(&tOriginalPixelDatas, &tZoonPixelDatas);
debug("***************************end PicZoom***************************\n");
			PicMerge(atIconLayout->iLeftTopX, atIconLayout->iLeftTopY, &tZoonPixelDatas, &ptVideoMem->tPixelDatas);
			/* �ͷŻ��ͼ��Ԫ���ݣ���ָ����һ��ͼ��*/
			FreePixelFrmIcon(&tOriginalPixelDatas);
			atIconLayout++;
		}
		ptVideoMem->eMemContent = VMC_GENERATE;
		free(tZoonPixelDatas.pucPixelDatas);

	}
	debug("***************************end GeneratePage***************************\n");
	return 0;
}


/**********************************************************************
 * �������ƣ� GenericGetInputEvent
 * ���������� ��ȡ��������,���ж���λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(PageLayout->atLayout�������һ��)
 ***********************************************************************/
int GenericGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	int iRet;
	int i = 0;
	PT_IconLayout atIconLayout = ptPageLayout->atIconLayout; /*ָ��ͼ������*/
	
	/* ���ԭʼ�Ĵ��������� 
	 * ���ǵ���input_manager.c�ĺ������˺������õ�ǰ�߷�����
	 * ���������̻߳�����ݺ󣬻��������
	 */
	iRet = GetInputEvent(&tInputEvent);
	if (iRet)
	{
		return -1;
	}

	if (tInputEvent.iType != TOUCHSCREEN)
	{
		return -1;
	}

	*ptInputEvent = tInputEvent;
	
	/* �������� */
	/* ȷ������λ����һ����ť�� */
	while (atIconLayout[i].pcName)
	{
		if ((tInputEvent.iX >= atIconLayout[i].iLeftTopX) && (tInputEvent.iX <= atIconLayout[i].iRightBotX) && \
			 (tInputEvent.iY >= atIconLayout[i].iLeftTopY) && (tInputEvent.iY <= atIconLayout[i].iRightBotY))
		{
			debug("\nthe index is %d\n", i);
			/* �ҵ��˱����еİ�ť */
			return i;
		}
		else
		{
			i++;
		}			
	}

	/* ����û�����ڰ�ť�� */
	return -1;
}


/**********************************************************************
 * �������ƣ� GetPage
 * ���������� ��������ȡ��ָ����"ҳ��ģ��"
 * ��������� pcName - ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - "ҳ��ģ��"��PT_PageAction�ṹ��ָ��
 ***********************************************************************/
PT_PageAction GetPage(char *pcName)
{
	PT_PageAction ptTmp = g_ptPageActionHead;
	
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

/**********************************************************************
 * �������ƣ� PagesInit
 * ���������� ���ø���"ҳ��ģ��"�ĳ�ʼ������,����ע������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int PagesInit(void)
{
	int iError;

	iError  = MainPageInit();
	iError |= BrowsePageInit();
	iError |= ManualPageInit();
	iError |= AutoPageInit();
	iError |= SettingPageInit();
	return iError;
}

