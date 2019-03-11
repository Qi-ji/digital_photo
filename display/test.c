#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <disp_manager.h>
#include <config.h>
#include <pic_operation.h>
#include <file.h>
#include <pic_manager.h>
#include <input_manager.h>
#include <page_manager.h>

char *strJpgName = "girl.jpg";
char *strBmpName = "nextpage_button.bmp";
//char *strBmpName = "continue_button.bmp";
//char *strJpgName = "motorbike.jpg";
char *strSmallJpgName = "shotting.jpg";

int g_iDefaultDispXres, g_iDefaultDispYres, g_iDefaultDispBpp;

/*BMP��ʽͼƬ��ʾ����*/
int BmpTest(void)
{
	T_FileMap tFileMap;
	T_PixelDatas tPixelDatas;				/*ԭʼͼƬ����*/
	T_PixelDatas tZoomPixelDatas;			/*ѹ����ͼƬ����*/

	 
		
	GetDefDispResolution(&g_iDefaultDispXres, &g_iDefaultDispYres, &g_iDefaultDispBpp);
	tPixelDatas.ibpp = g_iDefaultDispBpp;
	tZoomPixelDatas.ibpp = g_iDefaultDispBpp;

	snprintf(tFileMap.strFileName, 128, "%s", strBmpName);
	tFileMap.strFileName[127] = '\0';

	if (!MapFile(&tFileMap))
	{
		debug("mapbmpfile ok\n");
	}
	
	GetPicPraser(&tFileMap)->GetPixelDatas(&tFileMap, &tPixelDatas);
	
	tZoomPixelDatas.iWidth  = 342;
	tZoomPixelDatas.iHeight = 121;
	tZoomPixelDatas.iLineByte = tZoomPixelDatas.iWidth * 4;
	tZoomPixelDatas.iTotalByte = tZoomPixelDatas.iLineByte * tZoomPixelDatas.iHeight;
	//debug("tZoomPixelDatas.iTotalByte = %d\n", tZoomPixelDatas.iTotalByte);
	//debug("tPixelDatas.iTotalByte = %d\n", tPixelDatas.iTotalByte);
	tZoomPixelDatas.pucPixelDatas = malloc(tZoomPixelDatas.iTotalByte);
	if (NULL == tZoomPixelDatas.pucPixelDatas) 
	{
		return -1; 
	}
	
	//PicZoom(&tPixelDatas, &tZoomPixelDatas);
	//PicMerge(500, 120, &tZoomPixelDatas, &tPixelDatas);
	//PicPraser("bmp")->GetPixelDatas(&tFileMap, &tPixelDatas);
	GetDefaultDispOpr()->ShowPixel(&tPixelDatas);	/*��ʾԭʼͼƬ*/
	//GetDefaultDispOpr()->ShowPixel(&tZoomPixelDatas);  /*��ʾ��С��ͼƬ*/
	free(tZoomPixelDatas.pucPixelDatas);

	return 0;
}

/*JPG��ʽͼƬ��ʾ����*/
int JpgTest(void)
{
	T_FileMap tFileMap;
	T_PixelDatas tPixelDatas;/*ԭʼͼƬ����*/
	T_PixelDatas tSmallPixelDatas;
	T_PixelDatas tZoomPixelDatas;			/*ѹ����ͼƬ����*/
	
	tPixelDatas.ibpp = g_iDefaultDispBpp;
	tSmallPixelDatas.ibpp = g_iDefaultDispBpp;
	tZoomPixelDatas.ibpp = g_iDefaultDispBpp;
	
	snprintf(tFileMap.strFileName, 128, "%s", strJpgName);
	tFileMap.strFileName[127] = '\0';
	if (!MapFile(&tFileMap))
	{
		debug("mapjpgfile ok\n");
	}
	PicPraser("jpg")->GetPixelDatas(&tFileMap, &tPixelDatas);
	GetDefaultDispOpr()->ShowPixel(&tPixelDatas);

/*ʵ��ϲ���Ƭ����*/
	snprintf(tFileMap.strFileName, 128, "%s", strSmallJpgName);
	tFileMap.strFileName[127] = '\0';
	if (!MapFile(&tFileMap))
	{
		debug("mapSmalljpgfile ok\n");
	}
	PicPraser("jpg")->GetPixelDatas(&tFileMap, &tSmallPixelDatas);
	PicMerge(10, 20, &tSmallPixelDatas, &tPixelDatas);
	GetDefaultDispOpr()->ShowOnePage(&tPixelDatas);	
}



