#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <page_manager.h>

#include <config.h>
#include "image_manage.h"

pic_info_t picture_info[PICTURE_MAX_NUM];
int pic_index = 0; 


int picture_scan(const char *path)
{
	DIR *dir = NULL;
	//struct stat *file_statbuf;    //这里我定义为指针就会发生段错误?
									//这是因为使用lstat(cur_pathname, &file_statbuf)时，没有给錰ile_statbuf指定位置，所以在输入内容时候就会发生段错误
	struct stat file_statbuf;
	struct dirent *ptr = NULL;
	char cur_pathname[PATHNAMW_MAX_NUM];

	/*1.open the dir*/	
	if((dir = opendir(path)) == NULL)
	{
		fprintf(stderr, "wrong pathname\n");
		return -1;		
	}

	/*2.read the dir,and judge if it is a file or still a dir*/
	 while ((ptr=readdir(dir)) != NULL)
	 {		
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)//current dir OR parrent dir	
		    continue;

		/*2.1 fill the cur_pathname with file readed from dir*/
		memset(cur_pathname, '\0', sizeof(cur_pathname));
		strcpy(cur_pathname, path);
		strcat(cur_pathname, "/");
		strcat(cur_pathname, ptr->d_name);
		lstat(cur_pathname, &file_statbuf);

		/*2.2 */
		if (S_ISREG(file_statbuf.st_mode))
		{
			debug("%s is a file \n", cur_pathname);
			if(is_bmp(cur_pathname))
			{
				picture_info[pic_index].type = BMP_PIC;
				strcpy(picture_info[pic_index].pathname, cur_pathname);
			}
			
			if(is_jpg(cur_pathname))
			{
				picture_info[pic_index].type = JPG_PIC;
				strcpy(picture_info[pic_index].pathname, cur_pathname);
			}
			if(is_png(cur_pathname))
			{
				picture_info[pic_index].type = PNG_PIC;
				strcpy(picture_info[pic_index].pathname, cur_pathname);
			}

			pic_index++;
		}
		
		if (S_ISDIR(file_statbuf.st_mode))
		{
			debug("%s is a dir \n", cur_pathname);
			picture_scan(cur_pathname); 		//recursion the new dir
		}
		
	 }

	closedir(dir);
	return 0;
}


void MySleep(int ms)
{
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = ms * 1000; // 20 ms
	select(0, NULL, NULL, NULL, &delay);
}

extern int g_flag;
extern T_PageCfg g_tPageCfg;
int picture_display(void)
{
	int i = 0;
	for(i=0; i<pic_index; i++)
	{
		if (g_flag == 1)
		{
			return 1;
		}

		switch(picture_info[i].type)
		{
			case BMP_PIC:
				bmp_display(picture_info[i].pathname);
				break;

			case JPG_PIC:
				jpg_display(picture_info[i].pathname);
				break;

			case PNG_PIC:
				png_display(picture_info[i].pathname);
				break;

			default:
				printf("UNKNOWN picture type\n");
				break;
		}
	
		MySleep(g_tPageCfg.iIntervalSecond);
		//sleep(2);
	}
    return 0;
}









