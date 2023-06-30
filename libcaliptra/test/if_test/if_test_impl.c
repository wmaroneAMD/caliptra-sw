#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "caliptra_api.h"
#include "caliptra_if.h"

int caliptra_if_init(struct caliptra_init_params *params)
{
    return CALIPTRA_STATUS_OK;
}
void caliptra_destroy(void)
{
}

bool caliptra_exit_requested(void)
{
    return false;
}

struct caliptra_buffer* caliptra_output_peek(void)
{
    return NULL;
}

// Memory
int caliptra_write_u32(uint32_t offset, uint32_t data)
{
    return CALIPTRA_STATUS_OK;
}

int caliptra_read_u32(uint32_t offset, uint32_t *data)
{
    return 0x0ACEFACE;
}

// Fuses
bool caliptra_ready_for_fuses(void)
{
    return true;
}

// Firmware
bool caliptra_ready_for_firmware(void)
{
    return true;
}

// Simulator Control
int caliptra_step(void)
{
    return CALIPTRA_STATUS_OK;
}
