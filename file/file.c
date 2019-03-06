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
 * �������ƣ� MapFile
 * ���������� ʹ��mmap����ӳ��һ���ļ����ڴ�,�Ժ�Ϳ���ֱ��ͨ���ڴ��������ļ�
 * ��������� ptFileMap - �ں��ļ���strFileName
 * ��������� ptFileMap - tFp           : ���򿪵��ļ����
 *                        iFileSize     : �ļ���С
 *                        pucFileMapMem : ӳ���ڴ���׵�ַ
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
int MapFile(PT_FileMap ptFileMap)
{
	FILE *pfd;
	int ifd;
	struct stat FileStat;
	/*1.���ļ�������ļ�fd�Լ��ļ���С������*/
	
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
	
	/*2.ͨ��mmapӳ���ļ�*/
	ptFileMap->pucFileMapMem = (unsigned char *)mmap(NULL , ptFileMap->iFilesize, PROT_READ | PROT_WRITE, MAP_SHARED, ifd, 0);

	if (ptFileMap->pucFileMapMem == (unsigned char *)-1)
	{
		printf("mmap error!\n");
		return -1;
	}
	return 0;
}


/**********************************************************************
 * �������ƣ� UnMapFile
 * ���������� ʹ��mmap����ӳ��һ���ļ����ڴ�,�Ժ�Ϳ���ֱ��ͨ���ڴ��������ļ�
 * ��������� 
 * ���������
 * �� �� ֵ��
 ***********************************************************************/
void UnMapFile(PT_FileMap ptFileMap)
{
	munmap(ptFileMap->pucFileMapMem, ptFileMap->iFilesize);
	fclose(ptFileMap->pFd);
}


/**********************************************************************
 * �������ƣ� isDir
 * ���������� �ж�һ���ļ��Ƿ�ΪĿ¼
 * ��������� strFilePath - �ļ���·��
 *            strFileName - �ļ�������
 * ��������� ��
 * �� �� ֵ�� 0 - ����Ŀ¼
 *            1 - ��Ŀ¼
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
 * �������ƣ� isRegFile
 * ���������� �ж�һ���ļ��Ƿ񳣹���ļ�,�豸�ڵ�/�����ļ�/FIFO�ļ����������ļ�
 * ��������� strFilePath - �ļ���·��
 *            strFileName - �ļ�������
 * ��������� ��
 * �� �� ֵ�� 0 - ���ǳ����ļ�
 *            1 - �ǳ����ļ�
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
 * �������ƣ� GetDirContents
 * ���������� ��ĳĿ¼�������Ķ�����Ŀ¼�������ļ�����¼����,���Ұ���������
 * ��������� strDirName - Ŀ¼��(������·��)
 * ��������� pptDirContents - (*pptDirContents)ָ��һ��PT_DirContent����,
 *                             (*pptDirContents)[0,1,...]ָ��T_DirContent�ṹ��,
 *                             T_DirContent�к���"Ŀ¼/�ļ�"�����ֵ���Ϣ
 *            piNumber       - strDirName�º��ж��ٸ�"������Ŀ¼/�����ļ�",
 *                             ������(*pptDirContents)[0,1,...]�ж�����
 * �� �� ֵ�� 0 - �ɹ�
 *            1 - ʧ��
 ***********************************************************************/
int GetDirContents(char *strDirName, PT_DirContent **pptDirContents, int *piNumber)
{
    PT_DirContent *aptDirContents;			/*���飬ָ��ÿ��Ŀ¼�����ļ�*/
	struct dirent **aptNameList;
	int iNumber;
	int i;
	int j;

	/*ɨ��Ŀ¼*/
	iNumber = scandir(strDirName, &aptNameList, 0, alphasort);
	if (iNumber < 0)
	{
		debug("scandir error : %s!\n", strDirName);
		return -1;
	}
	
	/*aptDirContents ָ������������׵�ַ��Ϊ����Ŀ¼�����ڴ棬������ "." ".."Ŀ¼*/
	aptDirContents = malloc(sizeof(T_DirContent) * (iNumber - 2));
	if (NULL == aptDirContents)
	{
		debug("malloc error!\n");
		return -1;
	}
	*pptDirContents = aptDirContents; /*!!!!!!!*/

	/*aptDirContents[i] ָ��ĳ������ṹ����׵�ַ��Ϊ����ṹ�������ڴ�*/
	for (i=0; i<iNumber-2; i++)
	{
		aptDirContents[i] = malloc(sizeof(T_DirContent));
		if (NULL == aptDirContents[i])
		{
			debug("malloc error!\n");
			return -1;
		}
	}

	/*��Ŀ¼�ŵ������� j - ��������±ꣻ i - ɨ��õ��������±�*/
	for (i=0,j=0; i<iNumber; i++)
	{
		/* ����".",".."������Ŀ¼ */
		if ((0 == strcmp(aptNameList[i]->d_name, ".")) || (0 == strcmp(aptNameList[i]->d_name, "..")))
			continue;

        if (isDir(strDirName, aptNameList[i]->d_name))
		{
			strncpy(aptDirContents[j]->strName, aptNameList[i]->d_name, 256);
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType    = FILETYPE_DIR;
            free(aptNameList[i]);					//��ס�ڴ��ͷ�
            aptNameList[i] = NULL;
			j++;
		}

	}

	/*���ļ��ŵ������� i - ɨ��õ��������±� j - ��������±꣬�������˶��ٸ�Ŀ¼���ļ� */
	for (i = 0; i < iNumber; i++)
	{
        if (aptNameList[i] == NULL)		/*˵�����Ѿ������õ�Dir����*/	
            continue;    
		/* ����".",".."������Ŀ¼ */
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

	/*�ͷ�δʹ�õ��ڴ棬�Ѿ�ʹ�ù��Ĳ�����Ҫ���ڴ�*/
	for (i = j; i < iNumber - 2; i++)
	{
		free(aptDirContents[i]);			/*һ���Ƕ�ʹ�ù���*/
	}

	/* �ͷ�scandir����������ڴ� */
	for (i = 0; i < iNumber; i++)
	{
        if (aptNameList[i])
        {
    		free(aptNameList[i]);
        }
	}
	free(aptNameList);

	*piNumber = j;				/*һ��ɨ���˶���������*/
	return 0;
}









