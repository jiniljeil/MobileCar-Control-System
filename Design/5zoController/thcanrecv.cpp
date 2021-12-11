#include "thcanrecv.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

thCanRecv::thCanRecv()
{

}

void thCanRecv::run()
{

//    int can_fd ;

//    struct sockaddr_can addr ;
//    struct can_frame can_recv ;
//    struct ifreq ifr ;

//    ssize_t recv_bytes = 0 ;
//    char data[13] = {0, };

//    const char *ifname = "can0";

//    if ((can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1) {
//        perror("Erorr while opening socket") ;
//        return ;
//    }

//    strcpy(ifr.ifr_name, ifname);
//    ioctl(can_fd, SIOCGIFINDEX, &ifr);

//    addr.can_family = AF_CAN ;
//    addr.can_ifindex = ifr.ifr_ifindex;

//    printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

//    if (bind(can_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
//        perror("Error in socket bind");
//        return ;
//    }

//    while(1)
//    {
//        recv_bytes = read(ca)
//    }
}

