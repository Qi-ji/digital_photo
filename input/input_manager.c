#include <stdio.h>


#include <input_manager.h>
#include <config.h>

static PT_InputOpr g_ptIputOprHead;

/**********************************************************************
 * 函数名称： InpuOprtRegister
 * 功能描述： 注册事件输入模块
 * 输入参数： ptInputOpr - 事件输入模块结构体
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
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
 * 函数名称： ShowInpuOprt
 * 功能描述： 注册事件输入模块
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
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






