#include <stdio.h>
#include <string.h>

#include <pic_operation.h>
#include <file.h>


PT_PicFilePraser g_ptPicPraserHead = NULL;

/**********************************************************************
 * º¯ÊıÃû³Æ£º PicFilePraserRegister
 * ¹¦ÄÜÃèÊö£º ½«tBmpFilePraser½á¹¹Ìå×¢²áµ½Á´±íÖĞ
 * ÊäÈë²ÎÊı£º ptPicFilePraser - ÄÚº¬Í¼Æ¬½âÎöÆ÷µÄ½á¹¹Ìå
 * Êä³ö²ÎÊı£º 
 * ·µ »Ø Öµ£º 0 - ³É¹¦, ÆäËûÖµ - Ê§°Ü
 ***********************************************************************/
int PicPraserRegister(PT_PicFilePraser ptPicFilePraser)
{
	PT_PicFilePraser ptTmp = NULL;
		
	if (NULL == ptPicFilePraser)
	{
		printf("ptPicFilePraser is NULL\n");
		return -1;
	}

	if (NULL == g_ptPicPraserHead)
	{
		ptPicFilePraser->next = NULL;
		g_ptPicPraserHead = ptPicFilePraser;
	}
	else
	{
		ptTmp = g_ptPicPraserHead;
		while(ptTmp->next)
		{
			ptTmp = ptTmp->next;
		}
		ptTmp->next = ptPicFilePraser;
		ptPicFilePraser->next = NULL;
	}
	
	return 0;
}

/**********************************************************************
 * º¯ÊıÃû³Æ£º PicFilePraserRegister
 * ¹¦ÄÜÃèÊö£º ´òÓ¡ËùÓĞ×¢²á¹ıµÄÍ¼Æ¬ÏÔÊ¾Ä£¿é
 * ÊäÈë²ÎÊı£º 
 * Êä³ö²ÎÊı£º 
 * ·µ »Ø Öµ£º 0 - ³É¹¦, ÆäËûÖµ - Ê§°Ü
 ***********************************************************************/
 int ShowPicPraser(void)
{
	PT_PicFilePraser ptTmp = NULL;
	int i = 0;
	ptTmp = g_ptPicPraserHead;
	while (ptTmp)
	{
		printf("%02d. PicFilePraser:%s.\n", i++, ptTmp->name);
		ptTmp = ptTmp->next;
	}
	return 0;
}


/**********************************************************************
 * º¯ÊıÃû³Æ£º PicFilePraserRegister
 * ¹¦ÄÜÃèÊö£º ÊäÈëÍ¼Æ¬½âÎöÆ÷µÄÃû×Ö£¬»ñµÃ¸Ã½âÎöÆ÷½âÎöÄ£¿é
 * ÊäÈë²ÎÊı£º pcname - Í¼Æ¬½âÎöÆ÷µÄÃû×Ö£
 * Êä³ö²ÎÊı£º ÎŞ
 * ·µ »Ø Öµ£ºNULL   - Ê§°Ü,Ã»ÓĞÖ¸¶¨µÄÄ£¿é, 
 *		 PT_PicFilePraser - ½âÎöÆ÷½âÎöÄ£¿é½á¹¹ÌåÖ¸Õë
 ***********************************************************************/
PT_PicFilePraser PicPraser(char *pcname)
{
	PT_PicFilePraser ptTmp = NULL;
	ptTmp = g_ptPicPraserHead;
	while(ptTmp)
	{
		if (!strcmp(ptTmp->name, pcname))
			return ptTmp;
		ptTmp = ptTmp->next;
	}
	return NULL;
}

/**********************************************************************
 * º¯ÊıÃû³Æ£º GetPicParser
 * ¹¦ÄÜÃèÊö£º ÕÒµ½ÄÜÖ§³ÖÖ¸¶¨ÎÄ¼şµÄ"Í¼Æ¬ÎÄ¼ş½âÎöÄ£¿é"(µ÷ÓÃisSupportº¯Êı)
 * ÊäÈë²ÎÊı£º ptFileMap - ÄÚº¬ÎÄ¼şĞÅÏ¢
 * Êä³ö²ÎÊı£º ÎŞ
 * ·µ »Ø Öµ£º NULL   - Ê§°Ü,Ã»ÓĞÖ¸¶¨µÄÄ£¿é, 
 *            ·ÇNULL - Ö§³Ö¸ÃÎÄ¼şµÄ"Í¼Æ¬ÎÄ¼ş½âÎöÄ£¿é"µÄPT_PicFileParser½á¹¹ÌåÖ¸Õë
 ***********************************************************************/
PT_PicFilePraser GetPicPraser(PT_FileMap ptFileMap)
{
	PT_PicFilePraser ptTmp = NULL;
	ptTmp = g_ptPicPraserHead;
	while(ptTmp)
	{
		if (ptTmp->isSupport(ptFileMap))
		{
			return ptTmp;
		}
			
		ptTmp = ptTmp->next;
	}
	return NULL;
}

/**********************************************************************
 * º¯ÊıÃû³Æ£º PicPraserInit
 * ¹¦ÄÜÃèÊö£º ³õÊ¼»¯¸÷¸öÍ¼Æ¬½âÎöÄ£¿é
 * ÊäÈë²ÎÊı£º wu
 * Êä³ö²ÎÊı£º ÎŞ
 * ·µ »Ø Öµ£º NULL   - Ê§°Ü,Ã»ÓĞÖ¸¶¨µÄÄ£¿é, 
 *            ·ÇNULL - Ö§³Ö¸ÃÎÄ¼şµÄ"Í¼Æ¬ÎÄ¼ş½âÎöÄ£¿é"µÄPT_PicFileParser½á¹¹ÌåÖ¸Õë
 ***********************************************************************/
int PicPraserInit(void)
{
	int error = -1;
	error = BMPPraserInit();
	//error |= PNGPraserInit();
	error |= JPGPraserInit();
	return error;
}



