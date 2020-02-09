#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)



int main()
{
   
   int file;
   int num1, num2;
   int value1;
   char buffer[2048];
   printf("\nOpening Driver\n");
   file = open("/dev/adc8", O_RDWR);

   if(file<0)
   {
      printf("File did not open from user space\n");
      return 0;
   }

    printf("Enter ADC channel ID\n");
    scanf("%d",&num1);
    ioctl(file, WR_VALUE,(int32_t*) &num1);

    printf("Enter the data alignment ID 0 or 1\n");
    scanf("%d",&num2);
    printf("Writing Value to Driver\n");
    ioctl(file, WR_VALUE, (int32_t*) &num2);
    
    read(file, buffer, sizeof(buffer));
    puts(buffer);
    printf("Closing Driver\n");
    close(file);
    printf("Writing Value to Driver\n");
}
