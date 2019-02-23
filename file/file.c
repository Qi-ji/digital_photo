#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>



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











