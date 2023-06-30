#include <stdint.h>
#include <stdbool.h>

#define CALIPTRA_STATUS_OK 0

// Interface
int caliptra_if_init(struct caliptra_init_params *params);
void caliptra_destroy(void);
bool caliptra_exit_requested(void);
struct caliptra_buffer* caliptra_output_peek(void);

// Memory
int caliptra_write_u32(uint32_t offset, uint32_t data);
int caliptra_read_u32(uint32_t offset, uint32_t *data);

// Fuses
bool caliptra_ready_for_fuses(void);

// Firmware
bool caliptra_ready_for_firmware(void);

// Simulator Control
int caliptra_step(void);
