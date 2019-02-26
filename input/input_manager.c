#include <stdio.h>


#include <input_manager.h>
#include <config.h>

static PT_InputOpr g_ptIputOprHead;

/**********************************************************************
 * �������ƣ� InpuOprtRegister
 * ���������� ע���¼�����ģ��
 * ��������� ptInputOpr - �¼�����ģ��ṹ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int InputOprtRegister(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTempOpr = g_ptIputOprHead;
	
	if(NULL == g_ptIputOprHead)
	{
		g_ptIputOprHead = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	else
	{
		ptTempOpr = g_ptIputOprHead;
		while (ptTempOpr->ptNext)
		{
			ptTempOpr = ptTempOpr->ptNext;
			
		}
		ptTempOpr->ptNext = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	return 0;	
}

/**********************************************************************
 * �������ƣ� ShowInpuOprt
 * ���������� ע���¼�����ģ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void ShowInpuOprt(void)
{
	int iNumber = 0;
	PT_InputOpr ptTempOpr = g_ptIputOprHead;
	while (ptTempOpr->ptNext)
	{
		debug("InputOPr: %2d:%s.\n", iNumber++, ptTempOpr->name);
		ptTempOpr = ptTempOpr->ptNext;
	}
}






