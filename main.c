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
#include <test.h>
char rgb_buf[RGB_BUF_SIZE];



int main(void)
{	
	printf("image decode player....\n");

/****************************************************************************/
/*注册并初始化显示设备*/
	FBRegister();
	SetAndInitDefaultDispOpr("fb");
/*申请显示页面内存，输入设备初始化*/
	AllocVideoMem(5);		/*为显示页面申请内存*/
	AllInputRegister(); 	    /*所有输入设备进行链表注册*/
	AllInputOprInit();		/*所有输入设备进行初始化以及开启线程,开启输入事件线程，等待是否有输入事件发生(触摸屏输入)*/
/*注册并初始化图片解析模块*/	
	PicPraserInit();
/*初始化页面*/
	PagesInit();

	//BmpTest();
	//JpgTest();

/*实验主页面显示*/
		GetPage("mainpage")->Run(NULL);

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





























