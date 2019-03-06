#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>



/***************************structure definition*************************************/

typedef struct FileMap
{
	char 			strFileName[256];
	FILE 			*pFd;				//��fopen�򿪵��ļ����
	int 			iFilesize;	  
	unsigned char	*pucFileMapMem;
}T_FileMap, *PT_FileMap;

/* �ļ���� */
typedef enum {
	FILETYPE_DIR = 0,  /* Ŀ¼ */
	FILETYPE_FILE,     /* �ļ� */
}E_FileType;

/* Ŀ¼������� */
typedef struct DirContent {
	char strName[256];     /* ���� */
	E_FileType eFileType;  /* ��� */	
}T_DirContent, *PT_DirContent;


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
int MapFile(PT_FileMap ptFileMap);

/**********************************************************************
 * �������ƣ� UnMapFile
 * ���������� ʹ��mmap����ӳ��һ���ļ����ڴ�,�Ժ�Ϳ���ֱ��ͨ���ڴ��������ļ�
 * ��������� 
 * ���������
 * �� �� ֵ��
 ***********************************************************************/
void UnMapFile(PT_FileMap ptFileMap);
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
int GetDirContents(char *strDirName, PT_DirContent **pptDirContents, int *piNumber);



#endif


