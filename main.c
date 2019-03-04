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

char rgb_buf[RGB_BUF_SIZE];



int main(void)
{
	T_FileMap tFileMap;
	T_PixelDatas tPixelDatas;
	T_PixelDatas tSmallPixelDatas;
	T_PixelDatas tZoomPixelDatas;
	//char *strBmpName = "penjurn.bmp";
	char *strBmpName = "continue_button.bmp";
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

	AllocVideoMem(5);
	
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
	
	tZoomPixelDatas.iWidth  = 342;
	tZoomPixelDatas.iHeight = 121;
	tZoomPixelDatas.iLineByte = tZoomPixelDatas.iWidth * 4;
	tZoomPixelDatas.iTotalByte = tZoomPixelDatas.iLineByte * tZoomPixelDatas.iHeight;
	debug("tZoomPixelDatas.iTotalByte = %d\n", tZoomPixelDatas.iTotalByte);
	debug("tPixelDatas.iTotalByte = %d\n", tPixelDatas.iTotalByte);
	tZoomPixelDatas.pucPixelDatas = malloc(tZoomPixelDatas.iTotalByte);
	if (NULL == tZoomPixelDatas.pucPixelDatas) 
	{
		return -1; 
	}
	
	PicZoom(&tPixelDatas, &tZoomPixelDatas);
	//PicMerge(500, 120, &tZoomPixelDatas, &tPixelDatas);
	//PicPraser("bmp")->GetPixelDatas(&tFileMap, &tPixelDatas);
	//GetDefaultDispOpr()->ShowPixel(&tPixelDatas);
	GetDefaultDispOpr()->ShowPixel(&tZoomPixelDatas);
	free(tZoomPixelDatas.pucPixelDatas);

	//while(1)
	//{
		MainPageRun();
	//}
	
#if 0
/*实验jpg格式图片*/
	snprintf(tFileMap.strFileName, 128, "%s", strJpgName);
	tFileMap.strFileName[127] = '\0';
	if (!MapFile(&tFileMap))
	{
		debug("mapjpgfile ok\n");
	}
	PicPraser("jpg")->GetPixelDatas(&tFileMap, &tPixelDatas);
	//GetDefaultDispOpr()->ShowPixel(&tPixelDatas);

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

/*测试触摸屏函数*/
	//T_InputEvent tInputEvent;
	//TSDevInit();
	//TSGetInputEvent(&tInputEvent);
/****************************************************************************/
/*
	picture_scan("./image");

	while(1)
	{
		picture_display();
	}
	
	*/
	//GetDefaultDispOpr()->DeviceClose();

	return 0;
}





























