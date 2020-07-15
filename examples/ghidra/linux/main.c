#include <stdio.h>
#include "my_type.h"
#include <stdlib.h>

void test(int n)
{
  int buf[10];
  int *ptr;

  if (n < 10)
    ptr = buf;
  else
    ptr = (int *)malloc(sizeof (int) * n);

  /* do stuff.  */

  /* oops; this free should be conditionalized.  */
  free(ptr);
}

void two_args(int my_arg,my_type *t)
{
    int my_local = my_arg + 2;
    int i;
    printf("my_int=%d\n",t->my_int);
    for (i = 0; i < my_local; ++i)
        printf("i = %d\n", i);

}

int one_arg(int my_arg){
    char buffer[8];
    int i;
    my_type silly;
    silly.my_int=4;

    for (i = 0; i < my_arg; ++i) {
        buffer[i]=0x32;
    }
    printf("My_arg=%d",my_arg);
    printf("silly.my_int=%d\n",silly.my_int);
    return (silly.my_int);    
}

int main(int argc,char *argv)
{
    my_type test = {
       .my_int = 4
    }; 
    one_arg(10);
    two_args(2,&test);
    return 0;
}
