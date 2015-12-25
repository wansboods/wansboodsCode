#include <stdio.h>

#include "scand_harddisk.h"

int main(){

    SCAND_HD_INFO_LIST * list = NULL;
	start_loading_hard_disk_list( &list );
	return 0;
}
