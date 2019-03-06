#include <stdio.h>
#include <string.h>

#include <disp_manager.h>
#include <config.h>

static PT_DispOpr g_ptDispOprHead;
static PT_DispOpr g_ptDefaultDispOpr;
static PT_VideoMem g_ptVideoMemHead;



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
		g_ptDefaultDispOpr->CleanScreen(BLACK);
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

/**********************************************************************
 * �������ƣ� AllocVideoMem
 * ���������� VideoMem: Ϊ�ӿ���ʾ�ٶ�,���������ڻ����й������ʾ��ҳ�������,
 *            (��������ΪVideoMem)
 *            ��ʾʱ�ٰ�VideoMem�е����ݸ��Ƶ��豸���Դ���
 * ��������� iNum
 * ��������� ��
 * �� �� ֵ�� 0  - �ɹ�
 *            -1 - ʧ��(δʹ��SelectAndInitDefaultDispDev��ѡ����ʾģ��)
 ***********************************************************************/
int AllocVideoMem(int iNum)
{
	PT_VideoMem ptMemNew;
	int iFBXres, iFBYres, iFBBpp;
	int iFBLineByte, iFBTotalByte;
	int i;

	GetDefDispResolution(&iFBXres, &iFBYres, &iFBBpp);		/*���Ĭ���豸��Ӳ����Ϣ*/
	iFBLineByte = iFBXres * iFBBpp / 8;
	iFBTotalByte = iFBLineByte * iFBYres;
	
	ptMemNew = malloc(sizeof(T_VideoMem));					/*Ϊ ptMemNew ָ�������ڴ�*/
	if(NULL == ptMemNew)
		return -1;
/*�� FB ����ŵ�������*/
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
		/* �������Ҫ�������ڻ����VideoMem, 
		 * ���豸����framebuffer��Ӧ��VideoMem״̬����ΪVMS_USED_FOR_CUR,
		 * ��ʾ���VideoMem���ᱻ��Ϊ��������ȥ
		 */
		ptMemNew->eMemState = VMS_CUR;
	}
	/*��FB���ڴ���õ�����������*/
	ptMemNew->ptNext = NULL;
	g_ptVideoMemHead = ptMemNew;

/*������Ҫ�Ļ���*/

	for (i=0; i<iNum; i++)
	{
		/* ����T_VideoMem�ṹ�屾���"��framebufferͬ����С�Ļ���" */
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
		/*��������ָ��ָ��ṹ���ڴ�֮����ǿ�λ��*/
		ptMemNew->tPixelDatas.pucPixelDatas = (unsigned char *)(ptMemNew + 1);

		/*�ڴ���õ������У���ͷ����������*/
	   ptMemNew->ptNext = g_ptVideoMemHead;
	   g_ptVideoMemHead = ptMemNew;
		
	}

	return 0;
}


/**********************************************************************
 * �������ƣ� GetVideoMem
 * ���������� ���һ��ɲ�����VideoMem(�����ڴ洢Ҫ��ʾ������), 
 *            �������PutVideoMem���ͷ�
 * ��������� iID  - IDֵ,�ȳ������ڶ�VideoMem���ҵ�IDֵ��ͬ��
 *            bCur - 1��ʾ��ǰ��������Ҫʹ��VideoMem,�޷���ζ�Ҫ����һ��VideoMem
 *                   0��ʾ����Ϊ�˸Ľ����ܶ���ǰȡ��VideoMem,���Ǳ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û�п��õ�VideoMem
 *            ��NULL - �ɹ�,����PT_VideoMem�ṹ��
 ***********************************************************************/
PT_VideoMem GetVideoMem(int iID, int bCur)
{
	PT_VideoMem ptMemTemp;
	ptMemTemp = g_ptVideoMemHead;

/*1.��ȡ��ID ��ͬ �� �����ǿ��е��ڴ�*/
	while (!ptMemTemp->ptNext)
	{
		if((ptMemTemp->iMemID == iID) && (ptMemTemp->eMemState == VMS_FREE)) /*��ֹ��ID��ͬ���*/
		{
			ptMemTemp->eMemState = bCur ? VMS_CUR : VMS_PRE;
			return ptMemTemp;
		}
		ptMemTemp = ptMemTemp->ptNext;
	}
	
/*2.��������µ��ڴ治���ڣ�ȡ�����е� �� �����ݵ��ڴ�*/
	ptMemTemp = g_ptVideoMemHead;
	while (!ptMemTemp->ptNext)
	{
		if((ptMemTemp->eMemContent == VMC_BLANK) && (ptMemTemp->eMemState == VMS_FREE)) /*��ֹ��ID��ͬ���*/
		{
			ptMemTemp->iMemID = iID;
			ptMemTemp->eMemState = bCur ? VMS_CUR : VMS_PRE;
			return ptMemTemp;
		}
		ptMemTemp = ptMemTemp->ptNext;
	}

/*3.��������µ��ڴ治���ڣ�ȡ�����е��ڴ�*/
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

/*3.��������µ��ڴ治���ڣ�����iCur Ϊ1 �������ڴ����Ͼ�Ҫʹ�ã�������ȡ��һ���ڴ�*/
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
 * �������ƣ� ClearVideoMem
 * ���������� ��VideoMem���ڴ�ȫ����Ϊĳ����ɫ,������к���ͼƬ�ϲ�	
 * ��������� ptVideoMem - VideoMem�ṹ��ָ��, �ں�Ҫ�������ڴ�
 *            dwColor    - ����Ϊ����ɫ
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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


