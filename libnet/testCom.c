
#include <getopt.h>
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero

int main(int argc, char *argv[])
{

 int c;


  static struct option long_options[] =
  {

    { "help",          0,       NULL, 'h' },
    { NULL, 0, NULL, 0 }
  };
   while ((c = getopt_long(argc, argv, "h:a:b:c:",
                            long_options, NULL)) != EOF)
    {

      switch(c) {
	      case 'a':
          printf("有a=%s\n",optarg);
          break;
        case 'b': /* setup mode */
         printf("有b=%s\n",optarg);
          break;
        case 'c':
          printf("有c=%s\n",optarg);
        break;
        default:
        break;
      }
    }
  printf("有other=%s\n",optarg);
  printf("argc=%d\n",argc);
}
