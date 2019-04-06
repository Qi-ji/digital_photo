#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>


#include <disp_manager.h>
#include <config.h>
#include <pic_operation.h>
#include <file.h>
#include <pic_manager.h>
#include <input_manager.h>
#include <page_manager.h>

char *strJpgName = "/album_proj/image/jpg picture/motorbike.jpg";
char *strBmpName = "nextpage_button.bmp";
//char *strBmpName = "continue_button.bmp";
//char *strJpgName = "motorbike.jpg";
char *strSmallJpgName = "shotting.jpg";

int g_iDefaultDispXres, g_iDefaultDispYres, g_iDefaultDispBpp;

/*BMP格式图片显示测试*/
int BmpTest(void)
{
	T_FileMap tFileMap;
	T_PixelDatas tPixelDatas;				/*原始图片数据*/
	T_PixelDatas tZoomPixelDatas;			/*压缩后图片数据*/

	 
		
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
	GetDefaultDispOpr()->ShowPixel(&tPixelDatas);	/*显示原始图片*/
	//GetDefaultDispOpr()->ShowPixel(&tZoomPixelDatas);  /*显示缩小后图片*/
	free(tZoomPixelDatas.pucPixelDatas);

	return 0;
}

/*JPG格式图片显示测试*/
int JpgTest(void)
{
	T_FileMap tFileMap;
	T_PixelDatas tPixelDatas;/*原始图片数据*/
	T_PixelDatas tSmallPixelDatas;
	T_PixelDatas tZoomPixelDatas;			/*压缩后图片数据*/

	GetDefDispResolution(&g_iDefaultDispXres, &g_iDefaultDispYres, &g_iDefaultDispBpp);
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

/*实验合并照片功能*/
/*
	snprintf(tFileMap.strFileName, 128, "%s", strSmallJpgName);
	tFileMap.strFileName[127] = '\0';
	if (!MapFile(&tFileMap))
	{
		debug("mapSmalljpgfile ok\n");
	}
	PicPraser("jpg")->GetPixelDatas(&tFileMap, &tSmallPixelDatas);
	PicMerge(10, 20, &tSmallPixelDatas, &tPixelDatas);
	GetDefaultDispOpr()->ShowOnePage(&tPixelDatas);	
*/
}




/*触摸屏测试*/
static int GetTouchScreenEvent(int *type, int *code, int *value)
{
	int ret;
	int g_TSfd;
	/* 1.打开设备*/
	g_TSfd = open(TOUCHSCREENDEV, O_RDONLY);
	if (g_TSfd < 0)
	{
		debug("TouchScreenDev open error.\n");
		return -1;
	}

	/* 2.读取数据*/
	struct input_event t_TSInput;

	while(1)
	{
		/*阻塞读*/
		memset(&t_TSInput, 0, sizeof(struct input_event));  /*先将结构体全部清零*/
		ret = read(g_TSfd, &t_TSInput, sizeof(struct input_event));
		
		if (ret != sizeof(struct input_event))
		{
			debug("read TouchScreenDev error.\n");
			TSDevExit();
			return -1;
		}
		if ((t_TSInput.type != 3) || (t_TSInput.type == 3 && t_TSInput.code > 1) )
		{
			return -1;
		}
		*type = t_TSInput.type;
		*code = t_TSInput.code;
		*value = t_TSInput.value;
		
		return 0;
	}
	
}

int TouchScreenTest(void)
{
	int type, code, value;
	while(1)
	{
		if (0 == GetTouchScreenEvent(&type, &code, &value))
	    {
			debug("Type = %d, code = %d, valuee = %d.\n",type,code, value);
			debug("----------------------------------------------------------\n");
			debug("\n");
	    }
	}

	return 0;
	
}



/*Browse page 点击显示JPG格式图片显示测试*/
int BPJpgTest(char *strjpgName)
{
	T_FileMap tFileMap;
	T_PixelDatas tPixelDatas;/*原始图片数据*/

	GetDefDispResolution(&g_iDefaultDispXres, &g_iDefaultDispYres, &g_iDefaultDispBpp);
	tPixelDatas.ibpp = g_iDefaultDispBpp;

	snprintf(tFileMap.strFileName, 128, "%s", strjpgName);
	tFileMap.strFileName[127] = '\0';
	if (!MapFile(&tFileMap))
	{
		debug("mapjpgfile ok\n");
	}
	PicPraser("jpg")->GetPixelDatas(&tFileMap, &tPixelDatas);
	GetDefaultDispOpr()->ShowPixel(&tPixelDatas);
}

