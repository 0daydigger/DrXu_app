/* �Լ��ķ����� */
const char DRXU_SERVICE_NAME[] = "DrXu's Sword";
//#define SYMBOLIC_LINK_DRXU "\\DosDevices\\Drxu"
#define SYMBOLIC_LINK_DRXU L"\\\\.\\DrXu"


/*	���Ⱥ� */
#define IOCTL_TEST \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
