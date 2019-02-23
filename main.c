#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <disp_manager.h>
#include <config.h>
#include <pic_operation.h>
#include <file.h>
#include <pic_manager.h>

char rgb_buf[RGB_BUF_SIZE];



int main(void)
{
	T_FileMap tFileMap;
	T_PixelDatas tPixelDatas;
	T_PixelDatas tSmallPixelDatas;
	T_PixelDatas tZoomPixelDatas;
	char *strBmpName = "penjurn.bmp";
	//char *strJpgName = "motorbike.jpg";
	char *strJpgName = "girl.jpg";
	char *strSmallJpgName = "shotting.jpg";
	int iDefaultDispXres, iDefaultDispYres, iDefaultDispBpp; 
	

	
	printf("image decode player....\n");


/****************************************************************************/
/*注册并初始化显示设备*/
	FBRegister();
	SetAndInitDefaultDispOpr("fb");
	
	GetDefDispResolution(&iDefaultDispXres, &iDefaultDispYres, &iDefaultDispBpp);
	tPixelDatas.ibpp = iDefaultDispBpp;
	tSmallPixelDatas.ibpp = iDefaultDispBpp;
	tZoomPixelDatas.ibpp = iDefaultDispBpp;
	
	
/*注册并初始化图片解析模块*/	
	PicPraserInit();

/*映射文件并在设备上显示*/
/*实验bmp格式图片*/
	snprintf(tFileMap.strFileName, 128, "%s", strBmpName);
	tFileMap.strFileName[127] = '\0';

	if (!MapFile(&tFileMap))
	{
		debug("mapbmpfile ok\n");
	}
	
	GetPicPraser(&tFileMap)->GetPixelDatas(&tFileMap, &tPixelDatas);
	tZoomPixelDatas.iWidth  = 1024;
	tZoomPixelDatas.iHeight = 600;
	
	tZoomPixelDatas.iLineByte = tZoomPixelDatas.iWidth * 4;
	tZoomPixelDatas.iTotalByte = tZoomPixelDatas.iLineByte * tZoomPixelDatas.iHeight;
	debug("tZoomPixelDatas.iTotalByte = %d\n", tZoomPixelDatas.iTotalByte);
	debug("tPixelDatas.iTotalByte = %d\n", tPixelDatas.iTotalByte);
	tZoomPixelDatas.pucPixelDatas = malloc(tZoomPixelDatas.iTotalByte);
	if (NULL == tZoomPixelDatas.pucPixelDatas) 
	{
		return -1; 
	}
	
	//PicZoom1(&tPixelDatas, &tZoomPixelDatas);
	//PicPraser("bmp")->GetPixelDatas(&tFileMap, &tPixelDatas);
	GetDefaultDispOpr()->ShowOnePage(&tPixelDatas);
	//GetDefaultDispOpr()->ShowPixel(&tZoomPixelDatas);
	free(tZoomPixelDatas.pucPixelDatas);

/*实验jpg格式图片*/
	snprintf(tFileMap.strFileName, 128, "%s", strJpgName);
	tFileMap.strFileName[127] = '\0';
	if (!MapFile(&tFileMap))
	{
		debug("mapjpgfile ok\n");
	}
	PicPraser("jpg")->GetPixelDatas(&tFileMap, &tPixelDatas);
	GetDefaultDispOpr()->ShowPixel(&tPixelDatas);
#if 0
/*实验jpg格式图片*/
	snprintf(tFileMap.strFileName, 128, "%s", strSmallJpgName);
	tFileMap.strFileName[127] = '\0';
	if (!MapFile(&tFileMap))
	{
		debug("mapSmalljpgfile ok\n");
	}
	PicPraser("jpg")->GetPixelDatas(&tFileMap, &tSmallPixelDatas);
	PicMerge(10, 20, &tSmallPixelDatas, &tPixelDatas);
	//GetDefaultDispOpr()->ShowOnePage(&tPixelDatas);
#endif
/****************************************************************************/
/*
	picture_scan("./image");

	while(1)
	{
		picture_display();
	}
	
	*/
	GetDefaultDispOpr()->DeviceClose();

	return 0;
}





























