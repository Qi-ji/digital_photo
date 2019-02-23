#include <stdio.h>
#include <string.h>

#include <disp_manager.h>

PT_DispOpr g_ptDispOprHead;
PT_DispOpr g_ptDefaultDispOpr;



/**********************************************************************
 * 函数名称： DispOprRegister
 * 功能描述： 注册所有的显示设备到链表
 * 输入参数：
 * 输出参数： 
 * 返 回 值： 0 - 成功, 其他值 - 失败
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
 * 函数名称： ShowDispOpr
 * 功能描述： 打印所有注册过的显示设备
 * 输入参数： 
 * 输出参数： 
 * 返 回 值： 0 - 成功, 其他值 - 失败
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
 * 函数名称： GetDispOpr
 * 功能描述： 输入显示设备名称，获得该设备显示模块，例如:输入"fb",获得fb模块结构体
 * 输入参数： pcname - 显示设备名称
 * 输出参数： 无
 * 返 回 值：NULL   - 失败,没有指定的模块, 
 *		 PT_DispOpr - 显示模块结构体指针
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
 * 函数名称： SetAndInitDefaultDispOpr
 * 功能描述： 选定默认的显示设备，并初始化该设备
 * 输入参数： pcname - 显示设备名称
 * 输出参数： 无
 * 返 回 值： 无
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
 * 函数名称： GetDefaultDispOpr
 * 功能描述： 返回默认的显示设备
 * 输入参数： pcname - 显示设备名称
 * 输出参数： 无
 * 返 回 值： PT_DispOpr
 ***********************************************************************/
PT_DispOpr GetDefaultDispOpr(void)
{
	return g_ptDefaultDispOpr;
}

/**********************************************************************
 * 函数名称： GetDefaultDispOpr
 * 功能描述： 返回默认的显示设备
 * 输入参数： pcname - 显示设备名称
 * 输出参数： 无
 * 返 回 值： PT_DispOpr
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






