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
//#define SYMBOLIC_LINK_DRXU "\\DosDevices\\Drxu" Windows NT6.x��������ô������
#define SYMBOLIC_LINK_DRXU L"\\\\.\\DrXu"

/* IoControl������ */
//���Ⱥ�
//�����豸����
#define FILE_DEVICE_DRXU 0x19930410
//At the beginning there's FILE_DEVICE_UNKNOWN

// IoDeviceControl��Ϣ
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_DRXU, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DRXU_TEST_READ CTL_CODE(FILE_DEVICE_DRXU, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DRXU_TEST_WRITE CTL_CODE(FILE_DEVICE_DRXU, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
//����NtLoadDriver�¼�
#define IOCTL_DRXU_OPEN_EVENT_NtLoadDriver CTL_CODE(FILE_DEVICE_DRXU, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

//Ӧ�û�ȡҪ���ص�����·��
#define IOCTL_DRXU_DRIVERNAME_REQUEST CTL_CODE(FILE_DEVICE_DRXU, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
//Ӧ�÷���IOCTL_DRXU_DRIVERLOAD_REQUEST���༴�Ƿ��������
#define IOCTL_DRXU_DRIVERLOAD_REQUEST CTL_CODE(FILE_DEVICE_DRXU, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define NtLoadDriverEventName L"eventNtLoadDriver"
typedef struct CDrXu_Data
{
	//��������������
	DWORD dwDataType;
	//���������ݳ���
	DWORD dwDataLength;
	//����������ָ��
	LPVOID lpData;
}DrXu_Data;