#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "strextra.h"
#include <unistd.h>
#include "builtin.h"
#include "command.h"
#include "tests/syscall_mock.h"

#include <sys/syscall.h>
#include <sys/types.h>
#define PATH_SIZE 1024
#define INTERNOS 3

bool exit_bash = false;

/*struct internals_info{
  char * internals_name;
  void (*function_pointer)(const char *);
};*/

char * commands[INTERNOS] = {"cd", "help", "exit"};





static void run_cd(scommand cmd) {


    unsigned int n = scommand_length(cmd);
    assert(cmd != NULL);
    if (n > 2) {    
        printf("Argumentos de mas\n"); 
    }
    else if (n == 2) { 
        scommand_pop_front(cmd);
        char* path = scommand_front(cmd);
        int err = syscall(SYS_chdir, path);
        if (err != 0) {
            printf("No se encontro directorio \n");
        }
    }
    else if (n == 1) { 
        char* home_path = getenv("HOME"); 
        int err = syscall(SYS_chdir, home_path);
        if (err != 0) {
            printf("No se como esto dio un error \n");
        }
    }
}

void run_exit(void){
    exit_bash = true;
}

void run_help(const char *aux){
    char * help_str = "\n"
                      "------------------------------"
                      "MyBash Wolovers 2023 - Creada por:\n"
                      "Nicolás Fernandez, Guadalupe Galindo, Felipe Jimenez y Fabrizio Longhi\n"
                      "\n"
                      "Comandos Internos:\n"
                      "cd: Permite moverse entre directorios del sistema.\n"
                      "help: Printea información sobre la shell y los comandos internos.\n"
                      "exit: Provoca la terminación del proceso.\n"
                      "------------------------------"
                      "\n";
    
    printf("%s", help_str);
}

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    unsigned int i = 0;
    bool result = false;
    char *comando = scommand_front(cmd);
    
    while (!result && i < INTERNOS) {
        result = (strcmp(comando, commands[i]) == 0);
        i++;
    }
    
    return result;
}


bool builtin_alone(pipeline p){
    assert(p != NULL);
    assert (pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p)));
    return (pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p)));
}

void builtin_run(scommand cmd) {
    assert(builtin_is_internal(cmd));
    char *comando = scommand_front(cmd);
    const char *aux = NULL;

    if (strcmp(comando, commands[0]) == 0) {
        run_cd(cmd);
    }
    if (strcmp(comando, commands[1]) == 0) {
        run_help(aux);
    }
    if (strcmp(comando, commands[2]) == 0) {
        run_exit();
    }
    comando = NULL;
}
