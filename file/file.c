#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dirent.h>
#include <string.h>


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


/**********************************************************************
 * 函数名称： isDir
 * 功能描述： 判断一个文件是否为目录
 * 输入参数： strFilePath - 文件的路径
 *            strFileName - 文件的名字
 * 输出参数： 无
 * 返 回 值： 0 - 不是目录
 *            1 - 是目录
 ***********************************************************************/
static int isDir(char *strFilePath, char *strFileName)
{
    char strTmp[256];
    struct stat tStat;

    snprintf(strTmp, 256, "%s/%s", strFilePath, strFileName);
    strTmp[255] = '\0';

    if ((stat(strTmp, &tStat) == 0) && S_ISDIR(tStat.st_mode))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/**********************************************************************
 * 函数名称： isRegFile
 * 功能描述： 判断一个文件是否常规的文件,设备节点/链接文件/FIFO文件等是特殊文件
 * 输入参数： strFilePath - 文件的路径
 *            strFileName - 文件的名字
 * 输出参数： 无
 * 返 回 值： 0 - 不是常规文件
 *            1 - 是常规文件
 ***********************************************************************/
static int isRegFile(char *strFilePath, char *strFileName)
{
    char strTmp[256];
    struct stat tStat;

    snprintf(strTmp, 256, "%s/%s", strFilePath, strFileName);
    strTmp[255] = '\0';

    if ((stat(strTmp, &tStat) == 0) && S_ISREG(tStat.st_mode))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

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
int GetDirContents(char *strDirName, PT_DirContent **pptDirContents, int *piNumber)
{
    PT_DirContent *aptDirContents;			/*数组，指向每个目录或者文件*/
	struct dirent **aptNameList;
	int iNumber;
	int i;
	int j;

	/*扫描目录*/
	iNumber = scandir(strDirName, &aptNameList, 0, alphasort);
	if (iNumber < 0)
	{
		debug("scandir error : %s!\n", strDirName);
		return -1;
	}
	
	/*aptDirContents 指向整个数组的首地址，为所有目录申请内存，但忽略 "." ".."目录*/
	aptDirContents = malloc(sizeof(T_DirContent) * (iNumber - 2));
	if (NULL == aptDirContents)
	{
		debug("malloc error!\n");
		return -1;
	}
	*pptDirContents = aptDirContents; /*!!!!!!!*/

	/*aptDirContents[i] 指向某个数组结构体的首地址，为这个结构体申请内存*/
	for (i=0; i<iNumber-2; i++)
	{
		aptDirContents[i] = malloc(sizeof(T_DirContent));
		if (NULL == aptDirContents[i])
		{
			debug("malloc error!\n");
			return -1;
		}
	}

	/*将目录放到数组中 j - 输出数组下标； i - 扫描得到的数组下标*/
	for (i=0,j=0; i<iNumber; i++)
	{
		/* 忽略".",".."这两个目录 */
		if ((0 == strcmp(aptNameList[i]->d_name, ".")) || (0 == strcmp(aptNameList[i]->d_name, "..")))
			continue;

        if (isDir(strDirName, aptNameList[i]->d_name))
		{
			strncpy(aptDirContents[j]->strName, aptNameList[i]->d_name, 256);
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType    = FILETYPE_DIR;
            free(aptNameList[i]);					//记住内存释放
            aptNameList[i] = NULL;
			j++;
		}

	}

	/*将文件放到数组中 i - 扫描得到的数组下标 j - 输出数组下标，即保存了多少个目录及文件 */
	for (i = 0; i < iNumber; i++)
	{
        if (aptNameList[i] == NULL)		/*说明是已经被放置到Dir中了*/	
            continue;    
		/* 忽略".",".."这两个目录 */
		if ((0 == strcmp(aptNameList[i]->d_name, ".")) || (0 == strcmp(aptNameList[i]->d_name, "..")))
			continue;

        if (isRegFile(strDirName, aptNameList[i]->d_name))
		{
			strncpy(aptDirContents[j]->strName, aptNameList[i]->d_name, 256);
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType    = FILETYPE_FILE;
            free(aptNameList[i]);
            aptNameList[i] = NULL;
			j++;
		}
	}

	/*释放未使用的内存，已经使用过的不在需要的内存*/
	for (i = j; i < iNumber - 2; i++)
	{
		free(aptDirContents[i]);			/*一般是都使用过了*/
	}

	/* 释放scandir函数分配的内存 */
	for (i = 0; i < iNumber; i++)
	{
        if (aptNameList[i])
        {
    		free(aptNameList[i]);
        }
	}
	free(aptNameList);

	*piNumber = j;				/*一共扫描了多少项内容*/
	return 0;
}









