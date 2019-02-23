#include <stdio.h>
#include <string.h>

#include <disp_manager.h>

PT_DispOpr g_ptDispOprHead;
PT_DispOpr g_ptDefaultDispOpr;



/**********************************************************************
 * �������ƣ� DispOprRegister
 * ���������� ע�����е���ʾ�豸������
 * ���������
 * ��������� 
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int DispOprRegister(PT_DispOpr ptDispOpr)
{
	PT_DispOpr ptTmp = NULL;
		
	if (NULL == ptDispOpr)
	{
		printf("ptDispOpr is NULL\n");
		return -1;
	}

	if (NULL == g_ptDispOprHead)
	{
		ptDispOpr->next = NULL;
		g_ptDispOprHead = ptDispOpr;
	}
	else
	{
		ptTmp = g_ptDispOprHead;
		while(ptTmp->next)
		{
			ptTmp = ptTmp->next;
		}
		ptTmp->next = ptDispOpr;
		ptDispOpr->next = NULL;
	}
	
	return 0;
}

/**********************************************************************
 * �������ƣ� ShowDispOpr
 * ���������� ��ӡ����ע�������ʾ�豸
 * ��������� 
 * ��������� 
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
 int ShowDispOpr(void)
{
	PT_PicFilePraser ptTmp = NULL;
	int i = 0;
	ptTmp = g_ptDispOprHead;
	while (ptTmp)
	{
		printf("%02d. DispOpr: %s.\n", i++, ptTmp->name);
		ptTmp = ptTmp->next;
	}
	return 0;
}

/**********************************************************************
 * �������ƣ� GetDispOpr
 * ���������� ������ʾ�豸���ƣ���ø��豸��ʾģ�飬����:����"fb",���fbģ��ṹ��
 * ��������� pcname - ��ʾ�豸����
 * ��������� ��
 * �� �� ֵ��NULL   - ʧ��,û��ָ����ģ��, 
 *		 PT_DispOpr - ��ʾģ��ṹ��ָ��
 ***********************************************************************/
PT_DispOpr GetDispOpr(char *pcname)
{
	if (NULL == pcname)
	{
		printf("pcname error\n");
		return NULL;
	}
	PT_PicFilePraser ptTmp = NULL;
	ptTmp = g_ptDispOprHead;
	while(ptTmp)
	{
		if (!strcmp(ptTmp->name, pcname))
			return ptTmp;
		ptTmp = ptTmp->next;
	}
	return NULL;
}

/**********************************************************************
 * �������ƣ� SetAndInitDefaultDispOpr
 * ���������� ѡ��Ĭ�ϵ���ʾ�豸������ʼ�����豸
 * ��������� pcname - ��ʾ�豸����
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void SetAndInitDefaultDispOpr(char *pcname)
{
	g_ptDefaultDispOpr = GetDispOpr(pcname);
	if(g_ptDefaultDispOpr)
	{
		g_ptDefaultDispOpr->DeviceInit();
		g_ptDefaultDispOpr->CleanScreen(WHITE);
	}
}

/**********************************************************************
 * �������ƣ� GetDefaultDispOpr
 * ���������� ����Ĭ�ϵ���ʾ�豸
 * ��������� pcname - ��ʾ�豸����
 * ��������� ��
 * �� �� ֵ�� PT_DispOpr
 ***********************************************************************/
PT_DispOpr GetDefaultDispOpr(void)
{
	return g_ptDefaultDispOpr;
}

/**********************************************************************
 * �������ƣ� GetDefaultDispOpr
 * ���������� ����Ĭ�ϵ���ʾ�豸
 * ��������� pcname - ��ʾ�豸����
 * ��������� ��
 * �� �� ֵ�� PT_DispOpr
 ***********************************************************************/
int GetDefDispResolution(int *piXres, int *piYres, int *piBpp)
{
	if ((!piXres) || (!piYres) || (!piBpp)) 
	{
		return -1;
	}
	if(g_ptDefaultDispOpr)
	{
	*piXres = g_ptDefaultDispOpr->iXres;
	*piYres = g_ptDefaultDispOpr->iYres;
	*piBpp = g_ptDefaultDispOpr->ibpp;
	return 0;
	}
}






