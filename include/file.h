#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>


/***************************macro definition*************************************/



/***************************macro definition*************************************/



/***************************structure definition*************************************/

typedef struct FileMap
{
	char 			strFileName[256];
	FILE 			*pFd;				//文fopen打开的文件句柄
	int 			iFilesize;	  
	unsigned char	*pucFileMapMem;
}T_FileMap, *PT_FileMap;


/**********************************************************************
 * 函数名称： MapFile
 * 功能描述： 使用mmap函数映射一个文件到内存,以后就可以直接通过内存来访问文件
 * 输入参数： ptFileMap - 内含文件名strFileName
 * 输出参数： ptFileMap - tFp           : 所打开的文件句柄
 *                        iFileSize     : 文件大小
 *                        pucFileMapMem : 映射内存的首地址
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
int MapFile(PT_FileMap ptFileMap);

/**********************************************************************
 * 函数名称： UnMapFile
 * 功能描述： 使用mmap函数映射一个文件到内存,以后就可以直接通过内存来访问文件
 * 输入参数： 
 * 输出参数：
 * 返 回 值：
 ***********************************************************************/
void UnMapFile(PT_FileMap ptFileMap);




#endif


