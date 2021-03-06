#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pic_operation.h>
#include <config.h>


/**********************************************************************
 * 函数名称： PicMerge
 * 功能描述： 把小图片合并入大图片里
 * 输入参数： iX,iY      - 小图片合并入大图片的某个区域, iX/iY确定这个区域的左上角座标
 *            ptSmallPic - 内含小图片的象素数据
 *            ptBigPic   - 内含大图片的象素数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
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
	/* 下面这个也是可以的，而且更简单
	for (i = 0; i < ptSmallPic->iHeight; i++)
	{
		memcpy(pucDest, pucSrc, ptSmallPic->iLineByte);
		pucSrc += ptSmallPic->iLineByte;
		pucDest += ptBigPic->iLineByte;
	}*/

	return 0;
}

/**********************************************************************
 * 函数名称： PicZoom
 * 功能描述： 近邻取样插值方法缩放图片
 *            注意该函数会分配内存来存放缩放后的图片,用完后要用free函数释放掉
 *            "近邻取样插值"的原理请参考网友"lantianyu520"所著的"图像缩放算法"
 * 输入参数： ptOriginPic - 内含原始图片的象素数据
 *            ptZoomPic    - 内含缩放后的图片的象素数据
 * 输出参数： ptZoomPic->pucPixelFatas - 内含缩放后的图片的象素数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
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
	//int iRateX, iRateY;
	float iRateX, iRateY;

	iZoomWidth = ptZoomPic->iWidth;
	iZoomHeight = ptZoomPic->iHeight;
	iOriginWidth = ptOriginPic->iWidth;
	iOriginHeight = ptOriginPic->iHeight;
	
	iRateX = (float)iOriginWidth / iZoomWidth;
	iRateY = (float)iOriginHeight / iZoomHeight;

	debug("iOriginWidth = %d, iZoomWidth = %d \n", iOriginWidth, iZoomWidth);
	debug("iOriginHeight = %d, iZoomHeight = %d \n", iOriginHeight, iZoomHeight);

	debug("iRateX = %f, iRateY = %f \n", iRateX, iRateY);
		
	//pucSrcXPixelTable = malloc(ptZoomPic->iLineByte);
	pucSrcXPixelTable = malloc(sizeof(int) * iZoomWidth);
	if(NULL == pucSrcXPixelTable)
	{
		return -1;
	}
	
	//for (x=0; x<ptZoomPic->iLineByte; x++)  /* x 的循环条件错误，不应该是 x<ptZoomPic->iLineByte */
	for (x=0; x<iZoomWidth; x++)
	{
		pucSrcXPixelTable[x] =  x * iRateX;  /*存放着对应的原图中的x坐标*/
	}

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
	return 0;
}

