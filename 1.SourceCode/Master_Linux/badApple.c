#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <pthread.h>

void syser(char* str)
{
	perror("str");
	exit(1);
}

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0) {
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch( nEvent )
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }

    switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 230400:
        cfsetispeed(&newtio, B230400);
        cfsetospeed(&newtio, B230400);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    case 921600:
        printf("B921600\n");
        cfsetispeed(&newtio, B921600);
                cfsetospeed(&newtio, B921600);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    if( nStop == 1 )
        newtio.c_cflag &=  ~CSTOPB;
    else if ( nStop == 2 )
        newtio.c_cflag |=  CSTOPB;
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
    printf("set done!\n\r");
    return 0;
}

int open_port(char* uartname)
{
    int fd = open(uartname, O_RDWR|O_NOCTTY);
    if (-1 == fd)
    {
        perror("Can't Open Serial Port");
        return(-1);
    }
     /*恢复串口为阻塞状态*/
     if(fcntl(fd, F_SETFL, 0)<0)
     {
            printf("fcntl failed!\n");
     }else{
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
     }
     /*测试是否为终端设备*/
     if(isatty(STDIN_FILENO)==0)
     {
        printf("standard input is not a terminal device\n");
     }
     else
     {
        printf("isatty success!\n");
     }
     printf("fd-open=%d\n",fd);
     return fd;
}
int fd = 1;
char recv_done = 0;
void *RecvTask(void *arg)
{
    char recv = 0;
    int read_num;
    while(1)
    {
        read_num = read(fd, &recv, 1);
        if(read_num > 0)
        {
            if(recv == 1)	//stm32 recv done
                recv_done = 1;
            else			//stm32 recv error
                recv_done = 2;
        }
    }
}

int main(int argc, char **argv)
{
	int fd_mv = open("BadApple.bin", O_RDWR, 0644);
	if(fd_mv < 0)
		syser("open er:");
	
	int len = lseek(fd_mv, 0, SEEK_END);	//get the file lenth
		
	unsigned char *p = mmap(NULL, len, PROT_READ, MAP_SHARED, fd_mv, 0);
	if(p == MAP_FAILED)
		syser("mmap er:");
	
    printf("open bin file successfully ! size:%d\r\n", len);

    char* uartname="/dev/ttyUSB0";	//use demsg to checkout what it is in your linux
    if((fd=open_port(uartname))<0)
    {
         printf("open %s is failed\n",uartname);
         return 0;
    }
    else
    {
        pthread_t task_id;
        set_opt(fd, 230400, 8, 'N', 1);
        printf("open port %s\n",uartname);
        pthread_create(&task_id, NULL, RecvTask, NULL);	//make a thread to recv data form stm32
        printf("starting to send frame\n");
        int frame = 0;
        #define FRAME_LEN   1026
        char send_buf[FRAME_LEN] = {0};
        while (frame <= len/1024)
        {
            printf("send frame %d\n", frame);
            send_buf[0] = 0xEE;
            memcpy(send_buf+1, p+frame*1024, FRAME_LEN);
            send_buf[1025] = 0xED;
            recv_done = 0;
            write(fd, send_buf, FRAME_LEN);
            while(recv_done == 0);	//wait for confirm from stm32
            if(recv_done == 1)
                frame += 2;	//skip frame 2
            else if(recv_done == 2)
            {
                printf("Error recv\n");
                sleep(1);
            }
            usleep(1000*35.107);	//delay to sync music
        }
    }
    munmap(p, len);
	close(fd);
    close(fd_mv);
	return 0;
}
















