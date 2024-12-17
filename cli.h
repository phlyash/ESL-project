#ifndef CLI_H
#define CLI_H

#include <stdint.h>

#define ANSI_COLOR_CYAN "\x1b[1;31m"
#define ANSI_RESET "\x1b[0m"
#define HELP_NAME_ACCENT_COLOR ANSI_COLOR_CYAN

typedef void (*command_handler_t)(char*, char*);

typedef struct {
    char* name;
    char* short_help;
    char* help;
    command_handler_t handler;
} cli_command_t;

void cli_init_command(cli_command_t* command);
void cli_execute_command(char* input, char* output);
char* cli_get_positioned_argument(char* input, uint32_t position);

#endif