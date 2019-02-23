#include <stdio.h>
#include <string.h>

#include <pic_operation.h>
#include <file.h>


PT_PicFilePraser g_ptPicPraserHead = NULL;

/**********************************************************************
 * �������ƣ� PicFilePraserRegister
 * ���������� ��tBmpFilePraser�ṹ��ע�ᵽ������
 * ��������� ptPicFilePraser - �ں�ͼƬ�������Ľṹ��
 * ��������� 
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int PicPraserRegister(PT_PicFilePraser ptPicFilePraser)
{
	PT_PicFilePraser ptTmp = NULL;
		
	if (NULL == ptPicFilePraser)
	{
		printf("ptPicFilePraser is NULL\n");
		return -1;
	}

	if (NULL == g_ptPicPraserHead)
	{
		ptPicFilePraser->next = NULL;
		g_ptPicPraserHead = ptPicFilePraser;
	}
	else
	{
		ptTmp = g_ptPicPraserHead;
		while(ptTmp->next)
		{
			ptTmp = ptTmp->next;
		}
		ptTmp->next = ptPicFilePraser;
		ptPicFilePraser->next = NULL;
	}
	
	return 0;
}

/**********************************************************************
 * �������ƣ� PicFilePraserRegister
 * ���������� ��ӡ����ע�����ͼƬ��ʾģ��
 * ��������� 
 * ��������� 
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
 int ShowPicPraser(void)
{
	PT_PicFilePraser ptTmp = NULL;
	int i = 0;
	ptTmp = g_ptPicPraserHead;
	while (ptTmp)
	{
		printf("%02d. PicFilePraser:%s.\n", i++, ptTmp->name);
		ptTmp = ptTmp->next;
	}
	return 0;
}


/**********************************************************************
 * �������ƣ� PicFilePraserRegister
 * ���������� ����ͼƬ�����������֣���øý���������ģ��
 * ��������� pcname - ͼƬ�����������֣
 * ��������� ��
 * �� �� ֵ��NULL   - ʧ��,û��ָ����ģ��, 
 *		 PT_PicFilePraser - ����������ģ��ṹ��ָ��
 ***********************************************************************/
PT_PicFilePraser PicPraser(char *pcname)
{
	PT_PicFilePraser ptTmp = NULL;
	ptTmp = g_ptPicPraserHead;
	while(ptTmp)
	{
		if (!strcmp(ptTmp->name, pcname))
			return ptTmp;
		ptTmp = ptTmp->next;
	}
	return NULL;
}

/**********************************************************************
 * �������ƣ� GetPicParser
 * ���������� �ҵ���֧��ָ���ļ���"ͼƬ�ļ�����ģ��"(����isSupport����)
 * ��������� ptFileMap - �ں��ļ���Ϣ
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - ֧�ָ��ļ���"ͼƬ�ļ�����ģ��"��PT_PicFileParser�ṹ��ָ��
 ***********************************************************************/
PT_PicFilePraser GetPicPraser(PT_FileMap ptFileMap)
{
	PT_PicFilePraser ptTmp = NULL;
	ptTmp = g_ptPicPraserHead;
	while(ptTmp)
	{
		if (ptTmp->isSupport(ptFileMap))
		{
			return ptTmp;
		}
			
		ptTmp = ptTmp->next;
	}
	return NULL;
}

/**********************************************************************
 * �������ƣ� PicPraserInit
 * ���������� ��ʼ������ͼƬ����ģ��
 * ��������� wu
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - ֧�ָ��ļ���"ͼƬ�ļ�����ģ��"��PT_PicFileParser�ṹ��ָ��
 ***********************************************************************/
int PicPraserInit(void)
{
	int error = -1;
	error = BMPPraserInit();
	//error |= PNGPraserInit();
	error |= JPGPraserInit();
	return error;
}



