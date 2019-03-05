#include <stdio.h>
#include <pthread.h>

#include <input_manager.h>
#include <config.h>

static PT_InputOpr g_ptInputOprHead;
static T_InputEvent g_tInputEvent;

static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;  //互斥锁
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;	   //条件变量

/**********************************************************************
 * 函数名称： InpuOprtRegister
 * 功能描述： 注册事件输入模块
 * 输入参数： ptInputOpr - 事件输入模块结构体
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int InputOprtRegister(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTempOpr = g_ptInputOprHead;
	
	if(NULL == g_ptInputOprHead)
	{
		g_ptInputOprHead = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	else
	{
		ptTempOpr = g_ptInputOprHead;
		while (ptTempOpr->ptNext)
		{
			ptTempOpr = ptTempOpr->ptNext;
			
		}
		ptTempOpr->ptNext = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	return 0;	
}

/**********************************************************************
 * 函数名称： ShowInpuOprt
 * 功能描述： 注册事件输入模块
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void ShowInpuOprt(void)
{
	int iNumber = 0;
	PT_InputOpr ptTempOpr = g_ptInputOprHead;
	while (ptTempOpr->ptNext)
	{
		debug("InputOPr: %2d:%s.\n", iNumber++, ptTempOpr->name);
		ptTempOpr = ptTempOpr->ptNext;
	}
}
	
/**********************************************************************
 * 函数名称： InputEventFunc  (线程函数)
 * 功能描述： "输入模块"的线程函数,
 *		  每个输入模块都是通过创建子线程来读取输入数据, 读到数据后它会唤醒等得数据的其他线程
 * 输入参数： pVoid - 输入模块的"读输入数据函数"
 * 输出参数： 无
 * 返 回 值： NULL - 正常退出
 ***********************************************************************/
static void *InputEventFunc(void *pVoid)
{
	T_InputEvent tInputEvent;
	
	/* 定义函数指针 */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	GetInputEvent = (int (*)(PT_InputEvent))pVoid;

	while (1)
	{
		if(0 == GetInputEvent(&tInputEvent))   /*GetInputEvent 现在是TSGetInputEvent()函数*/
		{
			/* 唤醒主线程, 把tInputEvent的值赋给一个全局变量 */
			/* 访问临界资源前，先获得互斥量 */
			pthread_mutex_lock(&g_tMutex);
			g_tInputEvent = tInputEvent;			/*将获得的输入事件相关信息赋值给全部事件变量中*/

			/*	唤醒主线程 */
			pthread_cond_signal(&g_tConVar);

			/* 释放互斥量 */
			pthread_mutex_unlock(&g_tMutex);
		}
	}

	return NULL;
}

/**********************************************************************
 * 函数名称： AllInputOprInit
 * 功能描述： 调用所有"输入模块"的设备相关的初始化函数
 *            并创建用于读取输入数据的子线程,在main函数时进行初始化
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int AllInputOprInit(void)
{
	PT_InputOpr ptTmp = g_ptInputOprHead;
	int iError = -1;

	while (ptTmp)
	{
		if (0 == ptTmp->DeviceInit())		//打开输入设备，按键/触摸屏等
		{
			/* 创建子线程 */
			pthread_create(&ptTmp->tTreadID, NULL, InputEventFunc, ptTmp->DevGetInputEvent);			
			iError = 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return iError;
}


/**********************************************************************
 * 函数名称： GetInputEvent (主线程获得输入事件的函数)
 * 功能描述： 获得输入数据,它会使得当前线程休眠,
 *            当各输入模块的子线程读到数据后会把它唤醒
 * 输入参数： 无
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： 0 - 成功
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	/* 休眠 */
	pthread_mutex_lock(&g_tMutex);
	pthread_cond_wait(&g_tConVar, &g_tMutex);	

	/* 被唤醒后,返回数据 */
	*ptInputEvent = g_tInputEvent;			/*g_tInputEvent结构体中的内容赋值到输入指针指向的空间中*/
	pthread_mutex_unlock(&g_tMutex);

	debug("12321iX = %d, iY = %d\n",ptInputEvent->iX, ptInputEvent->iY);
	return 0;	
}

/**********************************************************************
 * 函数名称： AllInputRegister
 * 功能描述： 调用各个输入模块的初始化函数,就是注册各个输入模块
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int AllInputRegister(void)
{
	int iError = 0;
	// iError = StdinInit();
	iError |= TouchScreenInit();
	return iError;
}




