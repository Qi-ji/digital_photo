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
/*ע�Ტ��ʼ����ʾ�豸*/
	FBRegister();
	SetAndInitDefaultDispOpr("fb");
/*������ʾҳ���ڴ棬�����豸��ʼ��*/
	AllocVideoMem(5);		/*Ϊ��ʾҳ�������ڴ�*/
	AllInputRegister(); 	    /*���������豸��������ע��*/
	AllInputOprInit();		/*���������豸���г�ʼ���Լ������߳�,���������¼��̣߳��ȴ��Ƿ��������¼�����(����������)*/
/*ע�Ტ��ʼ��ͼƬ����ģ��*/	
	PicPraserInit();
/*��ʼ��ҳ��*/
	PagesInit();

	//BmpTest();
	//JpgTest();

/*ʵ����ҳ����ʾ*/
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





























