#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>


/***************************macro definition*************************************/



/***************************macro definition*************************************/



/***************************structure definition*************************************/

typedef struct FileMap
{
	char 			strFileName[256];
	FILE 			*pFd;				//��fopen�򿪵��ļ����
	int 			iFilesize;	  
	unsigned char	*pucFileMapMem;
}T_FileMap, *PT_FileMap;


int MapFile(PT_FileMap ptFileMap);


#endif


