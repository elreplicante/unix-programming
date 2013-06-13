#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  int fd[2];
  pid_t pid;

  /* se crea la tubería*/
  if (pipe(fd) < 0) {
    perror("Error al crear la tuberia \n");
    return 0;
  }

  pid = fork();
  switch (pid) {
    case -1:                                /* error */
      perror("Error en el fork");
      return 0;
    case 0:                                 /* proceso hijo ejecuta ls */
      close(fd[0]);
      close(STDOUT_FILENO);
      dup(fd[1]);
      close(fd[1]);
      execlp("ls", "ls", NULL);
      perror("Error en el exec");
      break;
    default:                                /* proceeso padre ejecuta wc */
      close(fd[1]);
      close(STDIN_FILENO);
      dup(fd[0]);
      close(fd[0]);
      execlp("wc", "wc", NULL);
      perror("Error en el exec");
  }

  return 0;
}
