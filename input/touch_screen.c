#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include <linux/input.h>
#include <input_manager.h>
#include <config.h>

static int g_TSfd;		/*触摸屏设备fd*/

/**********************************************************************
 * 函数名称： TSDevInit
 * 功能描述： 触摸屏输入模块的设备初始化函数
 *            注意: 由于要用到LCD的分辨率, 此函数要在SelectAndInitDisplay之后才能调用 
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int TSDevInit(void)
{
	/*1.打开设备*/	
	g_TSfd = open(TOUCHSCREENDEV, O_RDONLY);
	if (g_TSfd < 0)
	{
		debug("TouchScreenDev open error.\n");
		return -1;
	}
	return 0;
}

/**********************************************************************
 * 函数名称： TSDevExit
 * 功能描述： 触摸屏输入模块的设备退出函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void TSDevExit(void)
{
	close(g_TSfd);
}

/**********************************************************************
 * 函数名称： TouchScreenGetInputEvent
 * 功能描述： 触摸屏输入模块的读取数据函数,它在触摸屏输入模块的子线程中被调用
 * 输入参数： 无
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int TSGetInputEvent(PT_InputEvent ptInputEvent)
{
	int ret, i = 0;
	struct input_event t_TSInput;
	
	while(1)
	{
	/*阻塞读*/
		//memset(&t_TSInput, 0, sizeof(struct input_event));  /*先将结构体全部清零*/
		ret = read(g_TSfd, &t_TSInput, sizeof(struct input_event));
		if (ret == sizeof(struct input_event))
		{
			/*X坐标*/
			if ((t_TSInput.type == EV_ABS) && (t_TSInput.code == ABS_X))
				ptInputEvent->iX = t_TSInput.value;
			if ((t_TSInput.type == EV_ABS) && (t_TSInput.code == ABS_Y))
				ptInputEvent->iY = t_TSInput.value;
/*
			if ((t_TSInput.type == 0) && (t_TSInput.code == 0) && (t_TSInput.value == 0))
			{
				ptInputEvent->iPressure = 0; 
			}
*/		
		}
		else
		{
			debug("read TouchScreenDev error.\n");
			TSDevExit();
			return -1;
		}
		
		if( t_TSInput.type == EV_ABS )
		{
			ptInputEvent->iType = TOUCHSCREEN;
			ptInputEvent->iPressure = 1;
		}
		else 
		{
			ptInputEvent->iPressure = 0;
		}

		debug("X = %d, Y = %d, iType = %d, iPressur = %d.\n",ptInputEvent->iX, ptInputEvent->iY, ptInputEvent->iType, ptInputEvent->iPressure);
		//return 0;   /*这点不清楚为什么要这样*/
		//if(i++ == 2)
			return 0;
	}
	return 0;   /*这点不清楚为什么要这样*/
}

T_InputOpr g_tTSOpr = {
	.DeviceInit = TSDevInit,
	.DeviceExit = TSDevExit,
	.DevGetInputEvent = TSGetInputEvent,
};
/**********************************************************************
 * 函数名称： TouchScreenInit
 * 功能描述： 注册"触摸屏输入模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int TouchScreenInit(void)
{
	return InputOprtRegister(&g_tTSOpr);
}









