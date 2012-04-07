#define _POSIX_SOURCE

#define ABC_START 65
#define ABC_END 91

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

volatile sig_atomic_t signaled = 0;
volatile sig_atomic_t reset = 0;

void sig_usr1(int sig){
    signaled = 1;
}

void sig_usr2(int sig){
    reset = 1;
}

int main(void)
{
    pid_t pid,ppid;
    struct sigaction sigact1;
    struct sigaction sigact2;
    sigset_t sigusr2_set;
    int char_num = ABC_START;
    int c;

    ppid = getpid();
    sigemptyset(&sigusr2_set);
    sigaddset(&sigusr2_set,SIGUSR2);

    sigact1.sa_handler = sig_usr1;
    sigact1.sa_flags = 0;
    sigact2.sa_handler = sig_usr2;
    sigact2.sa_flags = 0;

    if (sigaction(SIGUSR1,&sigact1,NULL)) {
		perror("sigaction()");
		return(1);
	}
    if (sigaction(SIGUSR2,&sigact2,NULL)) {
		perror("sigaction()");
		return(1);
	}

    /* fork */
    if((pid = fork()) < 0) {
        perror("fork error");
    }

    /* parrent process */
    if(pid > 0){
        while (1){
            if(reset) {
                sigprocmask(SIG_BLOCK, &sigusr2_set,NULL);
                char_num = ABC_START;
                reset = 0;
                signaled = 1;/* pokracuj na getchar() */
                sigprocmask(SIG_UNBLOCK, &sigusr2_set,NULL);
            }else{
                if(char_num == ABC_END) char_num = ABC_START;
                printf("Parent (%d): '%c'\n",ppid,char_num++);
                kill(pid, SIGUSR1);
                printf("Press enter...");
            }
            /* pockaj na singal USR1 a blokuj USR2 */
            if(!signaled) sigsuspend(&sigusr2_set);
            signaled = 0;

            /* treba pockat na enter */
            c = getchar();
            while (c != '\n' && c != EOF) c=getchar();
        }
    }


    /* child process */
    if(pid == 0) {
        while (1){

            /* pockaj na singal USR1 a blokuj USR2 */
            if(!signaled) sigsuspend(&sigusr2_set);
            signaled = 0;

            if(reset){
                sigprocmask(SIG_BLOCK, &sigusr2_set,NULL);
                reset = 0;
                char_num = ABC_START;
                sigprocmask(SIG_UNBLOCK, &sigusr2_set,NULL);
            }

            if(char_num == ABC_END) char_num = ABC_START;
            printf("Child (%d): '%c'\n",getpid(),char_num++);
            kill(ppid, SIGUSR1);
        }
    }

	return 0;
}
