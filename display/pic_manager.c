#include <stdio.h>
#include <string.h>

#include <pic_operation.h>
#include <file.h>


PT_PicFilePraser g_ptPicPraserHead = NULL;

/**********************************************************************
 * 函数名称： PicFilePraserRegister
 * 功能描述： 将tBmpFilePraser结构体注册到链表中
 * 输入参数： ptPicFilePraser - 内含图片解析器的结构体
 * 输出参数： 
 * 返 回 值： 0 - 成功, 其他值 - 失败
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
 * 函数名称： PicFilePraserRegister
 * 功能描述： 打印所有注册过的图片显示模块
 * 输入参数： 
 * 输出参数： 
 * 返 回 值： 0 - 成功, 其他值 - 失败
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
 * 函数名称： PicFilePraserRegister
 * 功能描述： 输入图片解析器的名字，获得该解析器解析模块
 * 输入参数： pcname - 图片解析器的名字�
 * 输出参数： 无
 * 返 回 值：NULL   - 失败,没有指定的模块, 
 *		 PT_PicFilePraser - 解析器解析模块结构体指针
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
 * 函数名称： GetPicParser
 * 功能描述： 找到能支持指定文件的"图片文件解析模块"(调用isSupport函数)
 * 输入参数： ptFileMap - 内含文件信息
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - 支持该文件的"图片文件解析模块"的PT_PicFileParser结构体指针
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
 * 函数名称： PicPraserInit
 * 功能描述： 初始化各个图片解析模块
 * 输入参数： wu
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - 支持该文件的"图片文件解析模块"的PT_PicFileParser结构体指针
 ***********************************************************************/
int PicPraserInit(void)
{
	int error = -1;
	error = BMPPraserInit();
	//error |= PNGPraserInit();
	error |= JPGPraserInit();
	return error;
}



