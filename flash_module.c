#include "flash_module.h"
#include "nrfx_nvmc.h"
#include <stdint.h>
#include <stdbool.h>


static uint32_t* pWrite;
static uint32_t* pRead;
bool is_read_available = true;


static void reset_memory()
{
    pWrite = (uint32_t*) (READ_START_ADDRESS + 4);
    pRead = pWrite;
    nrfx_nvmc_page_erase(READ_START_ADDRESS);
    is_read_available = false;
}

static void add_zeroes_to_config_words(uint32_t offset)
{
    uint32_t control_word = 0;
    uint32_t last_bits_read = 0;
    uint32_t* last_address;

    for (uint32_t i = 0; i < 32 * CONFIG_WORDS_USED; i++, last_bits_read++)
    {
        if (i % 32)
        {
            last_bits_read = 0;
            last_address = (uint32_t*)(READ_START_ADDRESS + 0x4 * (i / 32));
            control_word = *last_address;
        }

        if (control_word & (1 << (i % 32)))
            break;
    }

    NRF_LOG_INFO("pWrite: %X", pWrite);
    NRF_LOG_INFO("pRead: %X", pRead);
    NRF_LOG_INFO("offset: %d", offset);

    while (offset)
    {
        uint32_t last_bits_unset = 32 - last_bits_read;
        uint32_t shift = (offset > last_bits_unset) ? last_bits_unset : offset;
        control_word = *last_address;
        nrfx_nvmc_word_write((uint32_t)last_address, control_word << shift);
        while(!nrfx_nvmc_write_done_check()) {}
        offset -= shift;
        last_address++;
    }
    NRF_LOG_INFO("base: %X", *((uint32_t*)READ_START_ADDRESS));
}

void save_color(hsv_t* hsv)
{
    uint32_t word_to_save = 0;
    uint32_t offset = 1;
    word_to_save = (hsv->value << (VALUE_POS)) + (hsv->saturation << (SATURATION_POS)) + (hsv->hue << (HUE_POS));
    NRF_LOG_INFO("SAVING: %X", word_to_save);
    NRF_LOG_INFO("hue: %d", hsv->hue);   
    NRF_LOG_INFO("saturation: %d", hsv->saturation); 
    NRF_LOG_INFO("value: %d", hsv->value);

    while (!nrfx_nvmc_word_writable_check((uint32_t)pWrite, word_to_save))
    {
        ++pWrite;
        ++offset;
        if (pWrite > (uint32_t*)(READ_END_ADDRESS) || ((uint32_t)pWrite - READ_START_ADDRESS - CONFIG_WORDS_USED > CONFIG_WORDS_USED * 128))
        {
            reset_memory();
            offset = 0;
        }
    }
    if ((uint32_t)pWrite - READ_START_ADDRESS - CONFIG_WORDS_USED > CONFIG_WORDS_USED * 128)
    {
        reset_memory();
        save_color(hsv);
        return;
    }

    NRF_LOG_INFO("%d", (uint32_t)pWrite - READ_START_ADDRESS - CONFIG_WORDS_USED);

    nrfx_nvmc_word_write((uint32_t)pWrite, word_to_save);
    while(!nrfx_nvmc_write_done_check()) {}

    add_zeroes_to_config_words(offset);

    pRead = pWrite++;
}

void load_color(hsv_t* hsv)
{
    uint32_t data = *pRead;
    hsv->hue = (data & HUE_MASK) >> (HUE_POS);
    hsv->saturation = (data & SATURATION_MASK) >> (SATURATION_POS);
    hsv->value = (data & VALUE_MASK) >> (VALUE_POS);
    NRF_LOG_INFO("val read: %X", data);   
    NRF_LOG_INFO("hue: %d", hsv->hue);   
    NRF_LOG_INFO("saturation: %d", hsv->saturation); 
    NRF_LOG_INFO("value: %d", hsv->value);


    if (hsv->hue > 360 || hsv->saturation > 100 || hsv->saturation > 100 || !is_read_available)
    {
        hsv->hue = 360 * 0.80;
        hsv->saturation = 100;
        hsv->value = 100;
    }
}

void init_flash(void)
{
#ifdef RESET_MEMORY
    reset_memory();
    return;
#endif
    uint32_t control_word = 0;
    uint32_t offset = 0;
    for (uint32_t i = 0; i < 32 * CONFIG_WORDS_USED; i++)
    {
        if (i % 32)
        {
            if (i / 32 >= CONFIG_WORDS_USED)
            {
                is_read_available = false;
                break;
            }
            control_word = *((uint32_t*)READ_START_ADDRESS + (i / 32));
        }

        if (control_word & (1 << (i % 32)))
            break;

        ++offset;
    }

    pWrite = (uint32_t*)(READ_START_ADDRESS + CONFIG_WORDS_USED * 0x4 + offset * 0x4);
    pRead = pWrite - 1;

    // cant happen -> memory corrupted -> erase
    if (!is_read_available)
    {
       reset_memory();
       return;
    }

    is_read_available = true;
}
