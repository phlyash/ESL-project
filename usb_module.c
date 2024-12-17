#include "usb_module.h"
#include "cli.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"
#include "app_usbd.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_cdc_acm.h"
#include "nrf_error.h"

#define READ_SIZE 1
#define COMMAND_BUFFER 64
#define OUTPUT_BUFFER 128
#ifdef ESTC_USB_CLI_ENABLED

static char m_rx_buffer[READ_SIZE];
static char command_buffer[COMMAND_BUFFER];
static char output_buffer[OUTPUT_BUFFER];
static size_t command_length = 0;
static bool tx_done = false;

void usb_ev_handler(app_usbd_class_inst_t const *p_inst, app_usbd_cdc_acm_user_event_t event);

#define CDC_ACM_COMM_INTERFACE 2
#define CDC_ACM_COMM_EPIN NRF_DRV_USBD_EPIN3
#define CDC_ACM_DATA_INTERFACE 3
#define CDC_ACM_DATA_EPIN NRF_DRV_USBD_EPIN4
#define CDC_ACM_DATA_EPOUT NRF_DRV_USBD_EPOUT4
APP_USBD_CDC_ACM_GLOBAL_DEF(usb_cdc_acm,
                            usb_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_NONE);

static void send_response(const char *response)
{
    ret_code_t ret = app_usbd_cdc_acm_write(&usb_cdc_acm, response, strlen(response));
    tx_done = false;
    while (!tx_done)
    {
        while (app_usbd_event_queue_process())
            {
            }
    }
    if (ret != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Failed to send response: %d", ret);
    }
}

void usb_ev_handler(app_usbd_class_inst_t const *p_inst, app_usbd_cdc_acm_user_event_t event)
{
    switch (event)
    {
    case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
        app_usbd_cdc_acm_read(&usb_cdc_acm, m_rx_buffer, READ_SIZE);
        break;
    case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
        command_length = 0;
        break;
    case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
        for (size_t i = 0; i < app_usbd_cdc_acm_rx_size(&usb_cdc_acm); i++)
        {
            char c = m_rx_buffer[i];
            send_response(m_rx_buffer);
            if (c == '\r' || c == '\n')
            {
                send_response("\r\n");
                if (command_length > 0)
                {
                    command_buffer[command_length] = '\0';
                    NRF_LOG_INFO("Received command: %s", command_buffer);
                    cli_execute_command(command_buffer, output_buffer);
                    command_length = 0;
                }
                strcat(output_buffer, "\r\n");
                send_response(output_buffer);
                memset(output_buffer, 0x00, OUTPUT_BUFFER);
            }
            else if (c == 127)
            {
                NRF_LOG_INFO("backspace");
                send_response("\b  \b\b");
                command_buffer[command_length--] = 0x00;
            }
            else if (command_length < COMMAND_BUFFER - 1)
            {
                command_buffer[command_length++] = c;
            }
            else
            {
                send_response("Error: Command buffer overflow.\r\n");
                command_length = 0;
            }
        }
        app_usbd_cdc_acm_read(&usb_cdc_acm, m_rx_buffer, READ_SIZE);
        break;
    case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
        tx_done = true;
        break;
    default:
        break;
    }
}

void usb_module_init(void)
{
    app_usbd_class_inst_t const *class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);
}
#endif
