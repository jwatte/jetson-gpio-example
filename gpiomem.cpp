#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>

/* Based on this example definition:
#define GPIO_CNF_     0x6000d100
#define GPIO_OE_      0x6000d000+0x110 // / GPIO_MSK_OE_* Output Enable
#define GPIO_OUT_      0x6000d000+0x120 // / GPIO_MSK_OUT_* GPIO Output Value
#define GPIO_IN_      0x6000d000+0x130 // GPIO Input Value (Read Only)
#define GPIO_INT_STA_     0x6000d000+0x140 // / GPIO_MSK_INT_STA_* GPIO Interrupt Status
#define GPIO_INT_ENB_     0x6000d000+0x150 // / GPIO_MSK_INT_ENB_* Interrupt Enable
#define GPIO_INT_LVL_      0x6000d000+0x160 // / GPIO_MSK_INT_LVL_* Interrupt Selection (Edge/Level)
#define GPIO_INT_CLR_      0x6000d000+0x170 // Interrupt Flag Set-to-Clear
*/

//  The only address we really need
#define GPIO_CNF_     0x6000d100

//  layout based on the definitions above
struct GPIO_mem {
    uint32_t CNF;
    uint32_t _pad1[3];
    uint32_t OE;
    uint32_t _pad2[3];
    uint32_t OUT;
    uint32_t _pad3[3];
    uint32_t IN;
    uint32_t _pad4[3];
    uint32_t INT_STA;
    uint32_t _pad5[3];
    uint32_t INT_ENB;
    uint32_t _pad6[3];
    uint32_t INT_LVL;
    uint32_t _pad7[3];
    uint32_t INT_CLR;
    uint32_t _pad8[3];
};

int main(void)
{
    //  read physical memory (needs root)
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("/dev/mem");
        fprintf(stderr, "please run this program as root (for example with sudo)\n");
        exit(1);
    }

    //  map a particular physical address into our address space
    int pagesize = getpagesize();
    int pagemask = pagesize-1;
    void *base = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (GPIO_CNF_ & ~pagemask));
    if (base == NULL) {
        perror("mmap()");
        exit(1);
    }

    //  set up a pointer for convenient access
    GPIO_mem volatile *pin = (GPIO_mem volatile *)((char *)base + (GPIO_CNF_ & pagemask));

    /* based on this example code:
    *(int *)(GPIO_CNF_) = 0x00ff; 
    *(int *)(GPIO_OE_)  = 0xff ;
    *(int *)(GPIO_OUT_) = 0xff ;
    *(int *)(GPIO_IN_) = 0x00 ;
    *(int *)(GPIO_INT_STA_) = 0x00;
    *(int *)(GPIO_INT_ENB_) = 0x00;
     *(int *)(GPIO_INT_LVL_) = 0x000000;
    */
    pin->CNF = 0x00ff;
    pin->OE = 0xff;
    pin->OUT = 0xff;
    //  pin->IN = 0x00; read only
    //  disable interrupts
    pin->INT_ENB = 0x00;
    //  don't worry about these for now
    //pin->INT_STA = 0x00;
    //pin->INT_LVL = 0x000000;
    //pin->INT_CLR = 0xffffff;

    fprintf(stderr, "press ctrl-C to stop\n");

    //  "blink" the output values
    uint8_t val = 0xff;
    while (true) {
        sleep(1);
        val = val ^ 0xff;
        pin->OE = val;
    }

    return 0 ;
}
