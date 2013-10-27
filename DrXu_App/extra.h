#ifndef _Windows_
#define _Windows_
#include <Windows.h>
#endif

#ifndef _WinIoCtl_
#define _WinIoCtl_
#include <WinIoCtl.h>
#endif

/* 自己的服务名 */
const char DRXU_SERVICE_NAME[] = "DrXu's Sword";
//#define SYMBOLIC_LINK_DRXU "\\DosDevices\\Drxu" Windows NT6.x不允许这么命名。
#define SYMBOLIC_LINK_DRXU L"\\\\.\\DrXu"

/* IoControl调度区 */
//调度宏
//定义设备名称
#define FILE_DEVICE_DRXU 0x19930410
//At the beginning there's FILE_DEVICE_UNKNOWN

// IoDeviceControl消息
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_DRXU, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DRXU_TEST_READ CTL_CODE(FILE_DEVICE_DRXU, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DRXU_TEST_WRITE CTL_CODE(FILE_DEVICE_DRXU, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
//传递NtLoadDriver事件
#define IOCTL_DRXU_OPEN_EVENT_NtLoadDriver CTL_CODE(FILE_DEVICE_DRXU, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

//应用获取要加载的驱动路径
#define IOCTL_DRXU_DRIVERNAME_REQUEST CTL_CODE(FILE_DEVICE_DRXU, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
//应用发送IOCTL_DRXU_DRIVERLOAD_REQUEST，亦即是否允许加载
#define IOCTL_DRXU_DRIVERLOAD_REQUEST CTL_CODE(FILE_DEVICE_DRXU, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define NtLoadDriverEventName L"eventNtLoadDriver"
typedef struct CDrXu_Data
{
	//包含的数据类型
	DWORD dwDataType;
	//包含的数据长度
	DWORD dwDataLength;
	//包含的数据指针
	LPVOID lpData;
}DrXu_Data;