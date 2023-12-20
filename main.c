
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

//variaveis que serão utilizadas.

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1
#define EXIT "exit"

//cores para o console.
#define COLOR_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0;0m"  

void lb_0890(char *argv[], int argc);
int callFork(char command[], char *argv[]);
int callPipeDup(int argc, char** argv);
int callTok(char command[]);

int callTok(char command[]) {
    char *argv[256];
    int argc = 0;
    //char line[] = "procA | procB | procC";
    //printf("%s\n", line);
    char * token = strtok(command, "|");
    //char * token = strtok(command, "");
    //argv[0] = token;
    while (token != NULL) {
        argv[argc++] = token;
        token = strtok(NULL, "|");

    }
    argv[argc] = NULL;
    /*printf("strtok():\n");
    for (int i = 0; i < argc; i++) {
        printf("token[%d] = %s\n", i, argv[i]);
    }
    
    printf("\nArgc %d\n", argc);*/
    lb_0890(argv, argc);
    return 0;
}

int callFork(char command[], char *argv[]) {
    pid_t result;
    /* fork a child process */
    strcpy(command, argv[0]);
    int argc = 0;
    char * token = strtok(command, " ");
    while (token != NULL) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    argv[argc] = NULL;

    /*int i = 0;
    while (argv[i] != NULL) {
        printf("token[%d] = %s\n", i, argv[i]);
        i++;
    }

    printf("\nComando: %s\n", command);*/
    result = fork(); //Upon successful completion, fork() shall return 0 to the child process and shall return the process ID of the child process to the parent process.
    if (result < 0) { /* error occurred */
        fprintf(stderr, "Falha na realizacao do fork()\n");
        return 1;
    } else if (result == 0) { /* child process */
        if (execvp(command, argv) == -1) {
            printf("\n---Comando não reconhecido----\n");
            exit(1);
        }
        execvp(command, argv);
    } else /* if (result > 0) */ { /* parent process */
        /* parent will wait for the child to complete */
        wait(NULL); // espera QUALQUER filho terminar
        //waitpid(result, NULL, NULL);
        printf("\nO processo o pai terminou, após aguardar pelo processo filho.\n");
    }
    return 0;
}

int callPipeDup(int argc, char** argv) {
    printf("Número de comandos: %d\n", argc);

    int vetPipe[argc - 1][2]; //vetor com os descritores
    for (int i = 0; i < argc - 1; i++) {//determina quantos pipes serão necessários
        if (pipe(vetPipe[i]) == -1) {//realiza a criação do pipe
            perror("Falha na realização do pipe");
            exit(1);
        }
    }

    for (int i = 0; i < argc; i++) { // separa por espaços
        int cont = 0;
        char *token2[256];
        char *token2Prog = strtok(argv[i], " ");
        while (token2Prog != NULL) {
            token2[cont++] = token2Prog;
            token2Prog = strtok(NULL, " ");
        }
        token2[cont] = NULL;

        pid_t result = fork(); //realiza o fork

        if (result < 0) {
            fprintf(stderr, "Falha na realizacao do fork()\n");
            return 1;
        }
        if (result == 0) { //processos filhos
            
            if (i == argc - 1) {
                dup2(vetPipe[i - 1][READ_END], STDIN_FILENO);
            } else if (i == 0) {
                dup2(vetPipe[i][WRITE_END], STDOUT_FILENO);
            } else if (i > 0 && i < argc - 1) {
                dup2(vetPipe[i - 1][READ_END], STDIN_FILENO);
                dup2(vetPipe[i][WRITE_END], STDOUT_FILENO);
            }
            if (execvp(token2[0], token2) == -1) {
                printf("\n---Comando não reconhecido----\n");
                printf("\nPipe não realizado.\n");
                exit(1);
            }
            execvp(token2[0], token2);
            return 0;
        }
        if (result > 0) {//Processo pai
            if (i < argc - 1) {
                close(vetPipe[i][WRITE_END]);
            }
            waitpid(result, NULL, 0);
            
        }
    }
    return 0;
}

void lb_0890(char *argv[], int argc) {
    char command[256];
    //    strcpy(command, a)
    if (argv[0] == NULL) return;
    //printf("\nArgc lb : %d\n", argc);
    if (argc > 1) {
        callPipeDup(argc, argv);
    } else {
        //printf("\nFork\n");
        callFork(command, argv);
    }
}

/*
 * 
 */
int main(int argc, char** argv) {
    char command[256];
    while (1) {
        printf(COLOR_GREEN "\n$" COLOR_RESET ": ");
        fgets(command, 256, stdin);
        setbuf(stdin, NULL);
        command[strcspn(command, "\n")] = 0;
        if (strcmp(EXIT, command) == 0) exit(1);
        callTok(command);
    }
    return (EXIT_SUCCESS);
}
