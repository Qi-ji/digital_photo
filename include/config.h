#ifndef __CONFIG_H__
#define __CONFIG_H__

/***************************macro definition*************************************/

#define DEBUG
//#undef DEBUG
#ifdef DEBUG
#define debug(fmt, args...) do{\
			fprintf(stderr, 							\
				"\033[1;32m  debug(%s: %s: %d):\t\033[0m" 	\
				fmt, __FILE__,__func__, __LINE__, ## args);		\
			}while(0);
		
#else
#define debug(fmt, args...)
#endif

/***************************macro definition*************************************/

//The picture's max resolution is 1096*1086,and the bpp is 24.
#define PIC_MAX_RESOLUTION  	(1096*1080)
#define RGB_BUF_SIZE			(PIC_MAX_RESOLUTION * 3)


extern char rgb_buf[RGB_BUF_SIZE];


/***************************structure definition*************************************/

typedef struct
{
	char 			*pathname;//path name and file name
	unsigned int 	width;	  
	unsigned int 	height;
	unsigned int	bpp;
	char			*pic_date;
}pic_info;


int is_jpg(const char *path);
int is_bmp(const char *path);
int is_png(const char *path);


int jpg_display(const char *path);
int bmp_display(const char *path);
int png_display(const char *path);

int picture_scan(const char *path);
int picture_display(void);





#endif


