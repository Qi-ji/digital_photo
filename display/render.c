#include <stdio.h>

#include <config.h>
#include <pic_manager.h>
#include <pic_operation.h>
#include <page_manager.h>
#include <render.h>
#include <file.h>
#include <disp_manager.h>




/**********************************************************************
 * �������ƣ� GetPixelFrmIcon
 * ���������� ���ͼ���������Ϣ��ͼ�����/etc/digitpic/iconsĿ¼��,ʹ�ú���Ҫ
 *		 ����FreePixelFrmIcon()�������ͷ��ڴ档
 * ��������� pcName    - �ں����ź��ͼƬ����������
 * ��������� ptPixelDatas - �ں�ԭʼͼƬ����������(����bpp������Ҫ��ǰָ��)
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int GetPixelFrmIcon(char *pcName, PT_PixelDatas ptPixelDatas)
{
	int ierror;
	T_FileMap tIconFileMap;

	/* FileMap ��ͼ��*/
	snprintf(tIconFileMap.strFileName, 128, "%s/%s", ICON_PATH, pcName);
	tIconFileMap.strFileName[127] = '\0';
	ierror = MapFile(&tIconFileMap);
	if(ierror)
		return -1;
	/* ͼƬ����������ͼ�� */
	ierror = PicPraser("bmp")->isSupport(&tIconFileMap);
	if(!ierror)
	{
		debug("not support Praser Icon: %s.\n", tIconFileMap.strFileName);
		UnMapFile(&tIconFileMap);
		return -1;
	}
	ierror = PicPraser("bmp")->GetPixelDatas(&tIconFileMap, ptPixelDatas);
	if(ierror)
	{
		debug("Praser Icon: %s error.\n", tIconFileMap.strFileName);
		UnMapFile(&tIconFileMap);
		return -1;
	}

debug("Icon original iWidth = %d.\n",ptPixelDatas->iWidth);
debug("Icon original iHeight = %d.\n",ptPixelDatas->iHeight);
debug("Icon original iLineByte = %d.\n",ptPixelDatas->iLineByte);
debug("Icon original iTotalByte = %d.\n",ptPixelDatas->iTotalByte);

	
	UnMapFile(&tIconFileMap);
	return 0;
	
}

/**********************************************************************
 * �������ƣ� FreePixelFrmIcon
 * ���������� �ͷ�GetPixelFrmIcon()����������ڴ�
 * ��������� ptPixelDatas - �ں�ԭʼͼƬ����������
 * ��������� 
 * �� �� ֵ�� 
 ***********************************************************************/
void FreePixelFrmIcon(PT_PixelDatas ptPixelDatas)
{
	PicPraser("bmp")->FreePixelDatas(ptPixelDatas);
}

/**********************************************************************
 * �������ƣ� FlushVideoMemToDev
 * ���������� �ѻ������е�����ˢ����ʾ�豸��ȥ,������ʾ�豸����ʾ�������е�ͼ��
 * ��������� ptVideoMem - ������,�ں���������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
void FlushVideoMemToDev(PT_VideoMem ptVideoMem)
{
	//memcpy((unsigned char *)(GetDefaultDispOpr()->pdwDispMem), ptVideoMem->tPixelDatas.pucPixelDatas, ptVideoMem.tPixelDatas.iHeight * ptVideoMem.tPixelDatas.iLineByte);

	if (!ptVideoMem->bFBDev)
	{
		//GetDefaultDispOpr()->ShowPixel(&ptVideoMem->tPixelDatas);
		GetDefaultDispOpr()->ShowOnePage(ptVideoMem);
	}
	
}
/**********************************************************************
 * �������ƣ� SetColorForPixelInVideoMem
 * ���������� ����VideoMem��ĳ���������ص���ɫ
 * ��������� iX,iY      - ��������
 *            ptVideoMem - ����VideoMem�е�����
 *            dwColor    - ����Ϊ�����ɫ,��ɫ��ʽΪ0x00RRGGBB
 * ��������� ��
 * �� �� ֵ�� �������ռ�ݶ����ֽ�
 * �޸�����        �汾��     �޸���	      �޸�����
 ***********************************************************************/
static int SetColorForPixelInVideoMem(int iX, int iY, PT_VideoMem ptVideoMem, unsigned int dwColor)
{
	unsigned char *pucVideoMem;
	unsigned short *pwVideoMem16bpp;
	unsigned int *pdwVideoMem32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;

	pucVideoMem      = ptVideoMem->tPixelDatas.pucPixelDatas;
	pucVideoMem      += iY * ptVideoMem->tPixelDatas.iLineByte+ iX * ptVideoMem->tPixelDatas.ibpp/ 8;
	pwVideoMem16bpp  = (unsigned short *)pucVideoMem;
	pdwVideoMem32bpp = (unsigned int *)pucVideoMem;

	switch (ptVideoMem->tPixelDatas.ibpp)
	{
		case 8:
		{
			*pucVideoMem = (unsigned char)dwColor;
			return 1;
			break;
		}
		case 16:
		{
			iRed   = (dwColor >> (16+3)) & 0x1f;
			iGreen = (dwColor >> (8+2)) & 0x3f;
			iBlue  = (dwColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			*pwVideoMem16bpp	= wColor16bpp;
			return 2;
			break;
		}
		case 32:
		{
			*pdwVideoMem32bpp = dwColor;
			return 4;
			break;
		}
		default :
		{			
			return -1;
		}
	}

	return -1;
}

/**********************************************************************
 * �������ƣ� ClearRegionVideoMem
 * ���������� ���VideoMem��ĳ����������,��Ϊĳ��ɫ
 * ��������� iTopLeftX,iTopLeftY   - ������������Ͻ�����
 *            iBotRightX,iBotRightY - ������������½�����
 *            ptVideoMem            - ����VideoMem�еľ�������
 *            dwColor               - ����Ϊ�����ɫ,��ɫ��ʽΪ0x00RRGGBB
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void ClearRegionVideoMem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, PT_VideoMem ptVideoMem, unsigned int dwColor)
{
	int x, y;
	for (y = iTopLeftY; y <= iBotRightY; y++)
		for (x = iTopLeftX; x <= iBotRightX; x++)
			SetColorForPixelInVideoMem(x, y, ptVideoMem, dwColor);
}


