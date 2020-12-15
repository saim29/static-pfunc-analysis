#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include<pthread.h>
#include <sys/types.h>
#include <linux/userfaultfd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <stdio_ext.h>
#include "mpk_apis.h"

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                                  } while (0)

#define GROUP_1 100

#define DOMAIN 20

int vpkeys[DOMAIN];

char global;
// private functions to manipulate domains


static void untrusted(){

    BRIDGE_DOMAINRW(vpkeys[0]);

    int a; 
    int b;
    a = 10;
    b = a;

    EXIT_DOMAIN(vpkeys[0]);

}

static void private_func1(char **addr) {
    //BRIDGE_DOMAINRW(vpkeys[0]);

    unsigned i;
    for(i =0;i< 4096;i++) {
        addr[0][i] = '@';
    }

    EXIT_DOMAIN(vpkeys[0]);
}

static void private_func2(char** addr) {
    
    BRIDGE_DOMAINRW(vpkeys[0]);

    unsigned i;
    for(i =0;i< 4096;i++) {
        addr[0][i] = 'a';
    }

    global = addr[0][0];
    EXIT_DOMAIN(vpkeys[0]);
}

static void private_func3(char** addr) {

    BRIDGE_DOMAINRW(vpkeys[1]);

    unsigned i;
    int a;
    for(i =0;i< 4096;i++) {
        addr[1][i] = 'b';
    }

    a = 1;
    addr[0][a] = global;
    EXIT_DOMAIN(vpkeys[1]);
    
}

static void private_func4(char **addr) {

    BRIDGE_DOMAINRW(vpkeys[1]);

    unsigned i;
    for(i =0;i< 4096;i++) {
        printf("%c\n", addr[1][i]);
    }

    //EXIT_DOMAIN(vpkeys[1]);

}

int main(void)
{

/**/

    // mpk initialization code
    _init(0.5);
    char* addr[DOMAIN];
    unsigned i;
    for(i = 0; i< DOMAIN; i++) {
        vpkeys[i] = rwmmap((void**)&addr[i]);
    }

    BRIDGE_DOMAINRW(vpkeys[1]);

    private_func1(addr);
    private_func2(addr);
    untrusted();
    private_func3(addr);
    private_func4(addr);

    // printf("Creating 100 domains each with one page memory.........\n");
    // BRIDGE_DOMAINRW(vpkeys[0]);
    // /*Fill up the first protected memory region with '@' */
    // printf("Changed the read write permission fo the domain [0]\n");
    // for(i =0;i< 4096;i++) {
    //     addr[0][i] = '@';
    // }
    // printf("Filled the page with a character\n");

    // for(i =0;i< 4096;i++)
    //  printf("%c",addr[0][i]);
    //  printf("\n");
	// EXIT_DOMAIN(vpkeys[0]);
    
    // //DESTROY_DOMAIN(vpkeys[0]);
    //  printf("**The access to the page is disabled by writing to the pkru regsiter (AD,WD)\n\
    //          Any attempt to access this domain would genererate segfault\n");
    //  printf("%c\n",*addr[0]);

    return 0;
}
