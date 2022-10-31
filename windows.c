#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[1;31m"
#define KGRN "\x1B[1;32m"
#define KYEL "\x1B[1;33m"
#define KBLU "\x1B[1;34m"
#define KMAG "\x1B[1;35m"
#define KCYN "\x1B[1;36m"
#define KWHT "\x1B[1;37m"

int main(int argc, char **argv)
{
#ifdef _WIN32 // Includes both 32 bit and 64 bit
  if (argc < 2)
  {
    printf("%sGive a filename%s\n", KYEL, KNRM);
    return 1;
  }

  if (access(argv[1], F_OK) == -1)
  {
    printf("%sError:%s file does not exist\n", KRED, KNRM);
    return 2;
  }

  struct stat st;
  stat(argv[1], &st);

  struct passwd *pw = getpwuid(st.st_uid);
  printf("%sFile %s has owner %s%s\n", KGRN, argv[1], pw->pw_name, KNRM);
#endif
  return 0;
}