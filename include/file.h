#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>



/***************************structure definition*************************************/

typedef struct FileMap
{
	char 			strFileName[256];
	FILE 			*pFd;				//文fopen打开的文件句柄
	int 			iFilesize;	  
	unsigned char	*pucFileMapMem;
}T_FileMap, *PT_FileMap;

/* 文件类别 */
typedef enum {
	FILETYPE_DIR = 0,  /* 目录 */
	FILETYPE_FILE,     /* 文件 */
}E_FileType;

/* 目录里的内容 */
typedef struct DirContent {
	char strName[256];     /* 名字 */
	E_FileType eFileType;  /* 类别 */	
}T_DirContent, *PT_DirContent;


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
/**********************************************************************
 * 函数名称： GetDirContents
 * 功能描述： 把某目录下所含的顶层子目录、顶层文件都记录下来,并且按名字排序
 * 输入参数： strDirName - 目录名(含绝对路径)
 * 输出参数： pptDirContents - (*pptDirContents)指向一个PT_DirContent数组,
 *                             (*pptDirContents)[0,1,...]指向T_DirContent结构体,
 *                             T_DirContent中含有"目录/文件"的名字等信息
 *            piNumber       - strDirName下含有多少个"顶层子目录/顶层文件",
 *                             即数组(*pptDirContents)[0,1,...]有多少项
 * 返 回 值： 0 - 成功
 *            1 - 失败
 ***********************************************************************/
int GetDirContents(char *strDirName, PT_DirContent **pptDirContents, int *piNumber);



#endif


