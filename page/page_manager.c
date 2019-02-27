#include <stdio.h>
#include <stdlib.h>


#include <config.h>
#include <pic_manager.h>
#include <pic_operation.h>
#include <page_manager.h>
#include <render.h>



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
	int iError;
/* 1.����ͼ������(���ͼ�����ء��������š��ϲ��Ȳ���)*/
	PT_IconLayout atIconLayout;
	atIconLayout = ptPageLayout->atIconLayout;	/* ָ��ͼ������*/
	T_PixelDatas tOriginalPixelDatas;
	T_PixelDatas tZoonPixelDatas;				/*��ʱ����ͼ�����ź�����ݣ����ںϲ����Դ���*/

	if (ptVideoMem->eMemContent != VMC_GENERATE)
	{
		
		tOriginalPixelDatas.ibpp = ptPageLayout->iBpp; /*bpp ��Ϣ��Ҫ��ǰ�趨*/
		tZoonPixelDatas.ibpp 	 = ptPageLayout->iBpp; /*bpp ��Ϣ��Ҫ��ǰ�趨*/
		tZoonPixelDatas.pucPixelDatas = malloc(ptPageLayout->iTotalByte);
		if (NULL == tZoonPixelDatas.pucPixelDatas)
			return -1;
		
		while (atIconLayout->pcName)
		{
			/* ���ͼ����������*/
			iError = GetPixelFrmIcon(atIconLayout->pcName, &tOriginalPixelDatas);
			if (iError)
			{
				debug("GetPixelFrmIcon: %s error.\n", atIconLayout->pcName);
				free(tZoonPixelDatas.pucPixelDatas);
				return -1;
			}
			/* ������ź�ͼ���������ݣ�ͨ��atIconLyout�����ͼ��ĳ�������Ϣ*/
			tZoonPixelDatas.iHeight 	= atIconLayout->iRightBotY - atIconLayout->iLeftTopY + 1;
			tZoonPixelDatas.iWidth 		= atIconLayout->iRightBotX - atIconLayout->iLeftTopX + 1;
			tZoonPixelDatas.iLineByte	= tZoonPixelDatas.iWidth * tZoonPixelDatas.ibpp / 8;
			tZoonPixelDatas.iTotalByte  = tZoonPixelDatas.iLineByte * tZoonPixelDatas.iHeight;

			/*����ͼ�겢�ϲ����Դ���*/
			PicZoom(&tOriginalPixelDatas, &tZoonPixelDatas);
			PicMerge(atIconLayout->iLeftTopX, atIconLayout->iRightBotY, &tZoonPixelDatas, &ptVideoMem->tPixelDatas);
			/* �ͷŻ��ͼ��Ԫ���ݣ���ָ����һ��ͼ��*/
			FreePixelFrmIcon(&tOriginalPixelDatas);
			atIconLayout++;
		}
		ptVideoMem->eMemContent = VMC_GENERATE;
		free(tZoonPixelDatas.pucPixelDatas);
	}
	return 0;
}


