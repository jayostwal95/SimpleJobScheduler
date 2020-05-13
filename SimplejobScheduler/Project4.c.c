/*
* gcc Project4.c queue.c -lpthread
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include "queue.h"
#include <semaphore.h>

queue *q;
typedef struct _shared {
    char **cmd;
    int tid;
    int strLen;
    pthread_t ptid;
} shared;

sem_t mutex;
int i=0;
char pid_out[100];
char pid_err[100];
int fdout, fderr;
char *readInput(void){
  static char input[100];
  printf("blazersh>");
  fgets(input, 256, stdin);
  //blazerlog(0,input);
  fflush(stdin);
  return input;
}
void *compute(void *arg) {
  char status_job[10];
  shared *s = (shared *)arg;
  char **cmd1 = s->cmd;
  int ttid=s->tid;
  pid_t pid;
  int status;
  time_t t1, t2;
  char buf[BUFSIZ];
  char log_s[BUFSIZ];
  //printf("%d\n", );
  FILE *fp2 = fopen("output.log","a");
  if (fp2 == NULL) {
    printf("Error opening file output.log for writing\n");
    exit(-1);
  }
  int cmd_cnt=0;
  while(cmd_cnt<(s->strLen-2)){
    strcat(buf, cmd1[cmd_cnt]); /* save line read */
    strcat(buf, " ");
    cmd_cnt++;
  }
  char cmd_cpy[100];
  sprintf(pid_out,"%d", s->tid);
  sprintf(pid_err, "%d", s->tid);
  strcat(cmd_cpy,buf);
  strcat(log_s,pid_out);
  strcat(log_s,"\t");
  strcat(log_s,cmd_cpy);
  strcat(log_s,"\t");
  strcat(log_s,"\t");
  int end_t = queue_insert(q, ttid, cmd_cpy);
  sem_wait(&mutex);
  queue_update(q, end_t);
  time(&t1);
  //pid = fork();
  //if (pid == 0) { /* this is child process */
    //printf("%d\n", ttid);


  strcat(pid_out,".out");
  strcat(pid_err,".err");
  strcat(buf,">");
  strcat(buf,pid_out);
  strcat(buf," 2>");
  strcat(buf,pid_err);
  strcat(buf,"\0");
  strcpy(status_job,"Success");
  if(system(buf)){
    strcpy(status_job , "Fail");
  }
  time(&t2);
  log_s[strlen(log_s) - 1] = '\t'; /* replace \n included by fgets with \t */
  strcat(log_s, ctime(&t1)); /* append start time to command with arguments */
  log_s[strlen(log_s) - 1] = '\t'; /* replace \n added by ctime at the end with \t */
  strcat(log_s, ctime(&t2)); /* append end time */
  //printf("%s\n", buf);
  log_s[strlen(log_s) - 1] = '\t'; /* replace \n added by ctime at the end with \t */
  //strcat(log_s, "\t");
  strcat(log_s, status_job);
  strcat(log_s, "\n");
  fprintf(fp2, "%s", log_s);
  fflush(fp2);
  queue_delete(q);
  sem_post(&mutex);
  //printf("job completed");
}

void create_process(){
  //char line[] = "uname -a";
  int status;
  int    N, size=10;
  shared *s;
  int j=0;
  s = (shared *)malloc(sizeof(shared)*size);
	q = queue_init(10);
  //tid = (pthread_t *)malloc(sizeof(pthread_t)*size);

  while(1){
    //printf("%d\n", j);
    char *line = readInput();
    if(line[0]=='\n'){
      continue;
    }
    size_t len = strlen(line);
    char *array[10];
    int i=0;

    if (len > 0 && line[len-1] == '\n') {
      line[--len] = '\0';
    }
    char line_copy[100];
    strcpy(line_copy, line);
    while ((array[i++] = strtok_r(line, " ", &line)));
    if(strcmp(array[0], "showjobs") == 0){
      printf("jobid \t command \t status\n");
      queue_display(q);
    }else if(strcmp(array[0], "submit") == 0) {
      s[j].strLen = i;
      s[j].tid = j;
      s[j].cmd = array+1;
      pthread_create(&s[j].ptid, NULL, compute, (void *)&s[j]);
      printf("Job %d added to queue\n",j);
      j++;
    }else if(strcmp(array[0], "submithistory")==0){
      printf("Jobid\tCommand\t\t\tStarttime\t\t\tEndtime\t\t\t\tStatus\n");
      FILE *fp1 = fopen("output.log","r");
      char line_read[BUFSIZ];
      if (fp1 == NULL) {
        printf("Error opening file %s for reading\n", "output.log");
        exit(-1);
      }
      while (fgets(line_read, BUFSIZ, fp1) != NULL) {
        printf("%s\n", line_read);
      }
    }
  }
}

int main(int argc, char *argv[]){
  if (argc < 2) {
      printf("Usage: %s <commands file>\n", argv[0]);
      exit(-1);
  }
  sem_init(&mutex, 0, atoi(argv[1]));
  create_process();
  sem_destroy(&mutex);
}
