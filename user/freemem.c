#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  uint64 freemem_size = freemem();
  printf("Free memory: %lu bytes\n", freemem_size);
  printf("Free memory: %lu kb\n", freemem_size/1024);
  printf("Free memory: %lu mb\n", freemem_size/1024/1024);
  exit(0);
}