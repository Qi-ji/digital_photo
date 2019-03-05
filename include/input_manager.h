#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include <sys/time.h>
#include <pthread.h>

#define TOUCHSCREENDEV "/dev/input/event2"
#define TOUCHSCREEN 1

typedef struct InputEvent{
	int iX;			/*触摸屏的按下的X/Y地址*/
	int iY;
	int iType;
	int ikey;
	int iPressure;  /*0 - 松开；1 - 按下*/
}T_InputEvent, *PT_InputEvent;

typedef struct InputOpr{
	char *name;
	pthread_t tTreadID;	/*子线程ID*/
	int (*DeviceInit)(void);
	int (*DeviceExit)(void);
	int (*DevGetInputEvent)(PT_InputEvent ptInputEvent);
	struct InputOpr *ptNext;
}T_InputOpr, *PT_InputOpr;

int TSDevInit(void);
int TSGetInputEvent(PT_InputEvent ptInputEvent);
/**********************************************************************
 * 函数名称： InpuOprtRegister
 * 功能描述： 注册事件输入模块
 * 输入参数： ptInputOpr - 事件输入模块结构体
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int InputOprtRegister(PT_InputOpr ptInputOpr);

/**********************************************************************
 * 函数名称： ShowInpuOprt
 * 功能描述： 注册事件输入模块
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void ShowInpuOprt(void);


/**********************************************************************
 * 函数名称： InputDevicesInit
 * 功能描述： 调用所有"输入模块"的设备相关的初始化函数
 *            并创建用于读取输入数据的子线程,在main函数时进行初始化
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int AllInputOprInit(void);

/**********************************************************************
 * 函数名称： AllInputInit
 * 功能描述： 调用各个输入模块的初始化函数,就是注册各个输入模块
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int AllInputRegister(void);

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
int GetInputEvent(PT_InputEvent ptInputEvent);


#endif

