#include <stdio.h>
#include <string.h>

#include <disp_manager.h>
#include <config.h>

static PT_DispOpr g_ptDispOprHead;
static PT_DispOpr g_ptDefaultDispOpr;
static PT_VideoMem g_ptVideoMemHead;



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
	PT_DispOpr ptTmp = NULL;
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
		g_ptDefaultDispOpr->CleanScreen(BLACK);
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

/**********************************************************************
 * 函数名称： AllocVideoMem
 * 功能描述： VideoMem: 为加快显示速度,我们事先在缓存中构造好显示的页面的数据,
 *            (这个缓存称为VideoMem)
 *            显示时再把VideoMem中的数据复制到设备的显存上
 * 输入参数： iNum
 * 输出参数： 无
 * 返 回 值： 0  - 成功
 *            -1 - 失败(未使用SelectAndInitDefaultDispDev来选择显示模块)
 ***********************************************************************/
int AllocVideoMem(int iNum)
{
	PT_VideoMem ptMemNew;
	int iFBXres, iFBYres, iFBBpp;
	int iFBLineByte, iFBTotalByte;
	int i;

	GetDefDispResolution(&iFBXres, &iFBYres, &iFBBpp);		/*获得默认设备的硬件信息*/
	iFBLineByte = iFBXres * iFBBpp / 8;
	iFBTotalByte = iFBLineByte * iFBYres;
	
	ptMemNew = malloc(sizeof(T_VideoMem));					/*为 ptMemNew 指针申请内存*/
	if(NULL == ptMemNew)
		return -1;
/*将 FB 缓存放到链表中*/
	ptMemNew->bFBDev = 1;
	ptMemNew->iMemID = 0;
	ptMemNew->eMemState = VMS_FREE;
	ptMemNew->eMemContent = VMC_BLANK;
	ptMemNew->tPixelDatas.pucPixelDatas = g_ptDefaultDispOpr->pdwDispMem;
	ptMemNew->tPixelDatas.ibpp = iFBBpp;
	ptMemNew->tPixelDatas.iHeight = iFBYres;
	ptMemNew->tPixelDatas.iWidth = iFBXres;
	ptMemNew->tPixelDatas.iLineByte = iFBLineByte;
	ptMemNew->tPixelDatas.iTotalByte = iFBTotalByte;

	if (iNum != 0)
	{
		/* 如果下面要分配用于缓存的VideoMem, 
		 * 把设备本身framebuffer对应的VideoMem状态设置为VMS_USED_FOR_CUR,
		 * 表示这个VideoMem不会被作为缓存分配出去
		 */
		ptMemNew->eMemState = VMS_CUR;
	}
	/*将FB的内存放置到链表的最后面*/
	ptMemNew->ptNext = NULL;
	g_ptVideoMemHead = ptMemNew;

/*申请需要的缓存*/

	for (i=0; i<iNum; i++)
	{
		/* 分配T_VideoMem结构体本身和"跟framebuffer同样大小的缓存" */
		ptMemNew = malloc(sizeof(T_VideoMem) + iFBTotalByte);
		if(NULL == ptMemNew)
			return -1;
		
		ptMemNew->bFBDev = 0;
		ptMemNew->iMemID = 0;
		ptMemNew->eMemState = VMS_FREE;
		ptMemNew->eMemContent = VMC_BLANK;
		ptMemNew->tPixelDatas.ibpp = iFBBpp;
		ptMemNew->tPixelDatas.iHeight = iFBYres;
		ptMemNew->tPixelDatas.iWidth = iFBXres;
		ptMemNew->tPixelDatas.iLineByte = iFBLineByte;
		ptMemNew->tPixelDatas.iTotalByte = iFBTotalByte;
		/*像素数据指针指向结构体内存之后的那块位置*/
		ptMemNew->tPixelDatas.pucPixelDatas = (unsigned char *)(ptMemNew + 1);

		/*内存放置到链表中，从头部插入链表*/
	   ptMemNew->ptNext = g_ptVideoMemHead;
	   g_ptVideoMemHead = ptMemNew;
		
	}

	return 0;
}


/**********************************************************************
 * 函数名称： GetVideoMem
 * 功能描述： 获得一块可操作的VideoMem(它用于存储要显示的数据), 
 *            用完后用PutVideoMem来释放
 * 输入参数： iID  - ID值,先尝试在众多VideoMem中找到ID值相同的
 *            bCur - 1表示当前程序马上要使用VideoMem,无法如何都要返回一个VideoMem
 *                   0表示这是为了改进性能而提前取得VideoMem,不是必需的
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有可用的VideoMem
 *            非NULL - 成功,返回PT_VideoMem结构体
 ***********************************************************************/
PT_VideoMem GetVideoMem(int iID, int bCur)
{
	PT_VideoMem ptMemTemp;
	ptMemTemp = g_ptVideoMemHead;

/*1.先取出ID 相同 且 现在是空闲的内存*/
	while (!ptMemTemp->ptNext)
	{
		if((ptMemTemp->iMemID == iID) && (ptMemTemp->eMemState == VMS_FREE)) /*防止有ID相同情况*/
		{
			ptMemTemp->eMemState = bCur ? VMS_CUR : VMS_PRE;
			return ptMemTemp;
		}
		ptMemTemp = ptMemTemp->ptNext;
	}
	
/*2.上述情况下的内存不存在，取出空闲的 且 无内容的内存*/
	ptMemTemp = g_ptVideoMemHead;
	while (!ptMemTemp->ptNext)
	{
		if((ptMemTemp->eMemContent == VMC_BLANK) && (ptMemTemp->eMemState == VMS_FREE)) /*防止有ID相同情况*/
		{
			ptMemTemp->iMemID = iID;
			ptMemTemp->eMemState = bCur ? VMS_CUR : VMS_PRE;
			return ptMemTemp;
		}
		ptMemTemp = ptMemTemp->ptNext;
	}

/*3.上述情况下的内存不存在，取出空闲的内存*/
	ptMemTemp = g_ptVideoMemHead;
	while (!ptMemTemp->ptNext)
	{
		if(ptMemTemp->eMemState == VMS_FREE) 
		{
			ptMemTemp->iMemID = iID;
			ptMemTemp->eMemContent = VMC_BLANK;
			ptMemTemp->eMemState = bCur ? VMS_CUR : VMS_PRE;
			return ptMemTemp;
		}
		ptMemTemp = ptMemTemp->ptNext;
	}

/*3.上述情况下的内存不存在，而且iCur 为1 ，即该内存马上就要使用，则任意取出一个内存*/
	if (bCur)
	{
		ptMemTemp = g_ptVideoMemHead;
		ptMemTemp->iMemID = iID;
		ptMemTemp->eMemContent = VMC_BLANK;
		ptMemTemp->eMemState = bCur ? VMS_CUR : VMS_PRE;
		return ptMemTemp;
	}

	return NULL;
}

/**********************************************************************
 * 函数名称： ClearVideoMem
 * 功能描述： 把VideoMem中内存全部清为某种颜色,方便进行后期图片合并	
 * 输入参数： ptVideoMem - VideoMem结构体指针, 内含要操作的内存
 *            dwColor    - 设置为该颜色
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ClearVideoMem(PT_VideoMem ptVideoMem, unsigned int dwColor)
{
	unsigned char *pucVMpixel;
	unsigned short *pucVM16pixel;
	unsigned int *pucVM32pixel;
	//int iRed, iGreen, iBlue;
	int i = 0;
	
	pucVMpixel = ptVideoMem->tPixelDatas.pucPixelDatas;
	pucVM16pixel = (unsigned short*)pucVMpixel; 
	pucVM32pixel = (unsigned int*)pucVMpixel; 	


	
	switch(ptVideoMem->tPixelDatas.ibpp)
	{
		case 8:
		{
			memset(pucVMpixel, dwColor, ptVideoMem->tPixelDatas.iTotalByte);
			break;
		}
		case 16:
		{
			break;
		}
		case 32:
		{
			while(i < ptVideoMem->tPixelDatas.iTotalByte)
			{
				*pucVM32pixel = dwColor;
				pucVM32pixel++;
				i += 4;
			}
			break;
		}
		default:
		{
			debug("not support this %d bpp.\n", ptVideoMem->tPixelDatas.ibpp)
			break;
		}

	}
	
}


