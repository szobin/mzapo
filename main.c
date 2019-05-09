#include <stdio.h>

#include "proglib/mzapo_func.h"
#include "devlib/mzapo_lib.h"

int main(){
    fp = fopen("log.txt", "w");
    fprintf(fp, "mzAPO log started\n");
    if (init_dev() != 0) {
      fprintf(fp, "-error: device init failed\n");
      return(1);
    }
    fprintf(fp, "-device init OK\n");

    main_cycle();

    clear_dev();

    fclose(fp);
    return 0;
}
