#include "cli.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct cli_command_list {
    cli_command_t* command;
    struct cli_command_list* next;
} cli_command_list_t;

void cli_help_command(char* input, char* output);

static cli_command_t help_command = 
{
    .name = "help",
    .short_help = "prints help for commands",
    .help = "lists all command and prints additional information",
    .handler = cli_help_command,
};


static cli_command_list_t command_head = 
{
    .command = &help_command,
    .next = NULL,
};

void cli_init_command(cli_command_t* command)
{
    static cli_command_list_t* command_tail = &command_head;

    command_tail->next = (cli_command_list_t*) malloc(sizeof(cli_command_list_t));
    command_tail = command_tail->next;

    command_tail->command = command;
}

static bool is_white_space(char character)
{
    return character == ' ' || character == '\r' || character == '\n' || character == '\t';
}

static void trim_whitespaces(char* input)
{
    while (is_white_space(*input))
    {
        input++;
    }
}

static cli_command_list_t* find_command(char* command_name)
{
    cli_command_list_t* current_command = &command_head;

    while (current_command != NULL)
    {
        if (strstr(command_name, current_command->command->name))
            return current_command;
        current_command = current_command->next;
    }

    return NULL;
}

void cli_execute_command(char* input, char* output)
{
    trim_whitespaces(input);
    cli_command_list_t* command = find_command(input);

    if (command == NULL)
    {
        strcat(output, "Command not found! Type help to list commands.");
        return;
    }

    command->command->handler(input, output);
}

char* cli_get_positioned_argument(char* input, uint32_t position)
{
    uint32_t length = strlen(input);
    uint32_t current_argument = 0;

    for(uint32_t i = 0; i < length; i++)
    {
        if (current_argument == position)
            return input;

        if (*input++ == ' ')
            ++current_argument;
    }
    return input;
}

void cli_help_command(char* input, char* output)
{
    cli_command_list_t* command = &command_head;

    while(command != NULL)
    {
        strcat(output, HELP_NAME_ACCENT_COLOR);
        strcat(output, command->command->name);
        strcat(output, ANSI_RESET);
        strcat(output, "\r\n\t");
        strcat(output, command->command->short_help);
        strcat(output, "\r\n");
        command = command->next;
    }
}
