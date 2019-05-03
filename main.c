#include <stdio.h>
#include <conio.h>
#include "mzapo_regs.h"


int main(){
    unsigned char *mem_base;
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (mem_base == NULL) exit(1);

    int ch = '1';

    while(ch != 'q') { 
        if(kbhit())                      
           ch = getch();                
    }

    return 0;
}