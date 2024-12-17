#include "cli_commands.h"
#include "cli.h"
#include "led_module.h"
#include "flash_module.h"
#include <stdlib.h>
#include <string.h>


void cli_rgb_command(char* input, char* output);
static cli_command_t rgb_command = 
{
    .name = "rgb",
    .short_help = "sets led rgb",
    .help = "set led2 rgb\r\nusage: rgb 255 0 0\r\nresult: led2 is red",
    .handler = cli_rgb_command,
};

void cli_hsv_command(char* input, char* output);
static cli_command_t hsv_command =
{
    .name = "hsv",
    .short_help = "sets led hsv",
    .help = "set led2 hsv\r\nusage: rgb 255 100 100\r\nresult: led2 is red",
    .handler = cli_hsv_command,
};

void cli_save_color_command(char* input, char* output);
static cli_command_t save_color_command =
{
    .name = "save",
    .short_help = "saves current color",
    .help = "save current color to flash memory",
    .handler = cli_save_color_command,
};

void cli_commands_init()
{
    cli_init_command(&rgb_command);
    cli_init_command(&hsv_command);
    cli_init_command(&save_color_command);
}

void cli_rgb_command(char* input, char* output)
{
    rgb_t rgb;
    rgb.red = atoi(cli_get_positioned_argument(input, 1));
    rgb.green = atoi(cli_get_positioned_argument(input, 2));
    rgb.blue = atoi(cli_get_positioned_argument(input, 3));

    NRF_LOG_INFO("red parsed: %d", rgb.red);
    NRF_LOG_INFO("green parsed: %d", rgb.green);
    NRF_LOG_INFO("blue parsed: %d", rgb.blue);

    hsv_t hsv;
    rgb_to_hsv(&rgb, &hsv);
    set_hsv(&hsv);

    strcat(output, "Set rgb successfully");
}

void cli_hsv_command(char* input, char* output)
{
    hsv_t hsv;
    hsv.hue = atoi(cli_get_positioned_argument(input, 1));
    hsv.saturation = atoi(cli_get_positioned_argument(input, 2));
    hsv.value = atoi(cli_get_positioned_argument(input, 3));

    NRF_LOG_INFO("hue parsed: %d", hsv.hue);
    NRF_LOG_INFO("saturation parsed: %d", hsv.saturation);
    NRF_LOG_INFO("value parsed: %d", hsv.value);


    set_hsv(&hsv);

    strcat(output, "Set hsv successfully");
}

void cli_save_color_command(char* input, char* output)
{
    save_color(get_current_color());

    strcat(output, "Color saved successfully");
}
