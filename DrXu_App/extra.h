#ifndef _Windows_
#define _Windows_
#include <Windows.h>
#endif

#ifndef _WinIoCtl_
#define _WinIoCtl_
#include <WinIoCtl.h>
#endif

/* �Լ��ķ����� */
const char DRXU_SERVICE_NAME[] = "DrXu's Sword";
//#define SYMBOLIC_LINK_DRXU "\\DosDevices\\Drxu"
#define SYMBOLIC_LINK_DRXU L"\\\\.\\DrXu"

/* IoControl������ */
//���Ⱥ�
//�����豸����
#define FILE_DEVICE_DRXU 0x19930410
//At the beginning there's FILE_DEVICE_UNKNOWN
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_DRXU, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

