#ifndef __IMAGE_MANAGE_H__
#define __IMAGE_MANAGE_H__

#define PATHNAMW_MAX_NUM 256
#define PICTURE_MAX_NUM  100


typedef enum pic_type
{
	BMP_PIC,
	JPG_PIC,
	PNG_PIC,
	UNKOWN_PIC,
}pic_type_t;

typedef struct 
{
	char 		pathname[PATHNAMW_MAX_NUM];
	pic_type_t 	 type;
}pic_info_t;





#endif 

