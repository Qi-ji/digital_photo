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
	sprintf(tIconFileMap.strFileName, 128, "%s/%s", ICON_PATH, pcName);
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
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void FlushVideoMemToDev(PT_VideoMem ptVideoMem)
{
	//memcpy(GetDefaultDispDev()->pucDispMem, ptVideoMem->tPixelDatas.aucPixelDatas, ptVideoMem.tPixelDatas.iHeight * ptVideoMem.tPixelDatas.iLineBytes);
	if (!ptVideoMem->bFBDev)
	{
		GetDefaultDispOpr()->ShowOnePage(ptVideoMem);
	}
}



