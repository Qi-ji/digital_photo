#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>



#include <file.h>
#include <config.h>


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
int MapFile(PT_FileMap ptFileMap)
{
	FILE *pfd;
	int ifd;
	struct stat FileStat;
	/*1.打开文件，获得文件fd以及文件大小等数据*/
	
	pfd = fopen(ptFileMap->strFileName, "r+");
	if (NULL == pfd)
	{
		printf("Mmap file: %s failed.\n", ptFileMap->strFileName);
		return -1;
	}
	ifd = fileno(pfd);
	
	if((fstat(ifd, &FileStat)) != 0)
	{
		return -1;
	}
	ptFileMap->pFd = pfd;
	ptFileMap->iFilesize = FileStat.st_size;
	debug("Filesize = %d \n", ptFileMap->iFilesize);
	
	/*2.通过mmap映射文件*/
	ptFileMap->pucFileMapMem = (unsigned char *)mmap(NULL , ptFileMap->iFilesize, PROT_READ | PROT_WRITE, MAP_SHARED, ifd, 0);

	if (ptFileMap->pucFileMapMem == (unsigned char *)-1)
	{
		printf("mmap error!\n");
		return -1;
	}
	return 0;
}


/**********************************************************************
 * 函数名称： UnMapFile
 * 功能描述： 使用mmap函数映射一个文件到内存,以后就可以直接通过内存来访问文件
 * 输入参数： 
 * 输出参数：
 * 返 回 值：
 ***********************************************************************/
void UnMapFile(PT_FileMap ptFileMap)
{
	munmap(ptFileMap->pucFileMapMem, ptFileMap->iFilesize);
	fclose(ptFileMap->pFd);
}











