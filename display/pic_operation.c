#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pic_operation.h>
#include <config.h>


/**********************************************************************
 * �������ƣ� PicMerge
 * ���������� ��СͼƬ�ϲ����ͼƬ��
 * ��������� iX,iY      - СͼƬ�ϲ����ͼƬ��ĳ������, iX/iYȷ�������������Ͻ�����
 *            ptSmallPic - �ں�СͼƬ����������
 *            ptBigPic   - �ں���ͼƬ����������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 ***********************************************************************/
int PicMerge(int iX, int iY, PT_PixelDatas ptSmallPic, PT_PixelDatas ptBigPic)
{
	if ((ptSmallPic->iWidth > ptBigPic->iWidth)  ||
		(ptSmallPic->iHeight > ptBigPic->iHeight) ||
		(ptSmallPic->ibpp != ptBigPic->ibpp))
	{
		return -1;
	}

	unsigned char *pucDest;
	unsigned char *pucSrc;
	int iSmallPicWidth;
	int iSmallPicHeight;
	int i, j, k = 0;

	
	pucSrc 			= ptSmallPic->pucPixelDatas;
	iSmallPicHeight = ptSmallPic->iHeight;
	iSmallPicWidth 	= ptSmallPic->iWidth;
	pucDest 		= ptBigPic->pucPixelDatas + iY * ptBigPic->iLineByte + iX * ptBigPic->ibpp/ 8;
	///debug("iSmallPicHeight = %d\n",iSmallPicHeight);
	//debug("iSmallPicWidth = %d\n",iSmallPicWidth);
	//debug("ptSmallPic->iLineByte = %d\n",ptSmallPic->iLineByte);
	//debug("ptBigPic->iLineByte = %d\n",ptBigPic->iLineByte);
	for (i=0; i<iSmallPicHeight; i++)
	{
		
		pucDest = ptBigPic->pucPixelDatas + (ptBigPic->iLineByte * (iY+k) + (iX * ptBigPic->ibpp / 8));
		k++;
		//debug("pucDest = %x\n", pucDest);
		
		for (j=0; j<iSmallPicWidth*4; j++)
		{
			*pucDest++ = *pucSrc++;
		}
	}
	/* �������Ҳ�ǿ��Եģ����Ҹ���
	for (i = 0; i < ptSmallPic->iHeight; i++)
	{
		memcpy(pucDest, pucSrc, ptSmallPic->iLineByte);
		pucSrc += ptSmallPic->iLineByte;
		pucDest += ptBigPic->iLineByte;
	}*/

	return 0;
}

/**********************************************************************
 * �������ƣ� PicZoom
 * ���������� ����ȡ����ֵ��������ͼƬ
 *            ע��ú���������ڴ���������ź��ͼƬ,�����Ҫ��free�����ͷŵ�
 *            "����ȡ����ֵ"��ԭ����ο�����"lantianyu520"������"ͼ�������㷨"
 * ��������� ptOriginPic - �ں�ԭʼͼƬ����������
 *            ptZoomPic    - �ں����ź��ͼƬ����������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int PicZoom(PT_PixelDatas ptOriginPic, PT_PixelDatas ptZoomPic)
{
	if ((ptOriginPic->ibpp != ptZoomPic->ibpp) ||
		(ptOriginPic->iHeight == 0) || (ptOriginPic->iWidth == 0)  ||
		(ptZoomPic->iHeight == 0)   || (ptOriginPic->iWidth == 0))
		return -1;
	unsigned char *pucSrc;
	unsigned char *pucDst;
	unsigned int *pucSrcXPixelTable;
	int iZoomWidth;
	int iZoomHeight;
	int iOriginWidth;
	int iOriginHeight;
	int iSrcY, iSrcX;
	int x, y;
	int iRateX, iRateY;

	iZoomWidth = ptZoomPic->iWidth;
	iZoomHeight = ptZoomPic->iHeight;
	iOriginWidth = ptOriginPic->iWidth;
	iOriginHeight = ptOriginPic->iHeight;
	
	iRateX = iOriginWidth / iZoomWidth;
	iRateY = iOriginHeight / iZoomHeight;

	//debug("iOriginWidth = %d, iZoomWidth = %d \n", iOriginWidth, iZoomWidth);
	//debug("iRateX = %d, iRateY = %d \n", iRateX, iRateY);
		
	//pucSrcXPixelTable = malloc(ptZoomPic->iLineByte);
	pucSrcXPixelTable = malloc(sizeof(int) * iZoomWidth);
	if(NULL == pucSrcXPixelTable)
	{
		return -1;
	}
	
	//for (x=0; x<ptZoomPic->iLineByte; x++)  /* x ��ѭ���������󣬲�Ӧ���� x<ptZoomPic->iLineByte */
	for (x=0; x<iZoomWidth; x++)
	{
		pucSrcXPixelTable[x] =  x * iRateX;  /*����Ŷ�Ӧ��ԭͼ�е�x����*/
	}
	//debug("ptZoomPic->iLineByte = %d\n", ptZoomPic->iLineByte);
	//debug("ptOriginPic->iLineByte = %d\n", ptOriginPic->iLineByte);
	for (y=0; y<iZoomHeight; y++)
	{
		iSrcY = y * iRateY;
		pucSrc = ptOriginPic->pucPixelDatas + iSrcY * ptOriginPic->iLineByte;
	    pucDst = ptZoomPic->pucPixelDatas + y * ptZoomPic->iLineByte;
		for (x=0; x<iZoomWidth; x++)
		{
			iSrcX = pucSrcXPixelTable[x];
			memcpy(pucDst + x*ptOriginPic->ibpp/8, pucSrc + iSrcX*ptZoomPic->ibpp/8, ptZoomPic->ibpp/8);
		}
	}
	free(pucSrcXPixelTable);	
}

