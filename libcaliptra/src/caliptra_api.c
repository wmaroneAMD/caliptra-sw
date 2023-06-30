// Licensed under the Apache-2.0 license
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <caliptra_top_reg.h>
#include "caliptra_api.h"
#include "caliptra_if.h"
#include "caliptra_fuses.h"
#include "caliptra_mbox.h"

static bool caliptra_init_complete = false;

int caliptra_init(struct caliptra_init_params *params)
{
    caliptra_init_complete = caliptra_if_init(params);

    return caliptra_init_complete;
}

int caliptra_init_fuses(struct caliptra_fuses *fuses)
{
    // Parameter check
    if (!caliptra_init_complete || !fuses) {
        return -EINVAL;
    }

    // Check whether caliptra is ready for fuses
    if (!caliptra_ready_for_fuses())
        return -EPERM;

    // Write Fuses
    caliptra_fuse_array_write(GENERIC_AND_FUSE_REG_FUSE_UDS_SEED_0, fuses->uds_seed, sizeof(fuses->uds_seed));
    caliptra_fuse_array_write(GENERIC_AND_FUSE_REG_FUSE_FIELD_ENTROPY_0, fuses->field_entropy, sizeof(fuses->field_entropy));
    caliptra_fuse_array_write(GENERIC_AND_FUSE_REG_FUSE_KEY_MANIFEST_PK_HASH_0, fuses->key_manifest_pk_hash, sizeof(fuses->key_manifest_pk_hash));
    caliptra_fuse_write(GENERIC_AND_FUSE_REG_FUSE_KEY_MANIFEST_PK_HASH_MASK, fuses->key_manifest_pk_hash_mask);
    caliptra_fuse_array_write(GENERIC_AND_FUSE_REG_FUSE_OWNER_PK_HASH_0, fuses->owner_pk_hash, sizeof(fuses->owner_pk_hash));
    caliptra_fuse_write(GENERIC_AND_FUSE_REG_FUSE_FMC_KEY_MANIFEST_SVN, fuses->fmc_key_manifest_svn);
    caliptra_fuse_array_write(GENERIC_AND_FUSE_REG_FUSE_FMC_KEY_MANIFEST_SVN, fuses->runtime_svn, sizeof(fuses->runtime_svn));
    caliptra_fuse_write(GENERIC_AND_FUSE_REG_FUSE_ANTI_ROLLBACK_DISABLE, (uint32_t)fuses->anti_rollback_disable);
    caliptra_fuse_array_write(GENERIC_AND_FUSE_REG_FUSE_IDEVID_CERT_ATTR_0, fuses->idevid_cert_attr, sizeof(fuses->idevid_cert_attr));
    caliptra_fuse_array_write(GENERIC_AND_FUSE_REG_FUSE_IDEVID_MANUF_HSM_ID_0, fuses->idevid_manuf_hsm_id, sizeof(fuses->idevid_manuf_hsm_id));
    caliptra_fuse_write(GENERIC_AND_FUSE_REG_FUSE_LIFE_CYCLE, (uint32_t)fuses->life_cycle);

    // Write to Caliptra Fuse Done
    caliptra_write_u32(CALIPTRA_TOP_REG_GENERIC_AND_FUSE_REG_CPTRA_FUSE_WR_DONE, 1);

    // It shouldn`t be longer ready for fuses
    if (caliptra_ready_for_fuses())
        return -EIO;

    return 0;
}

int caliptra_bootfsm_go()
{
    if (!caliptra_init_complete) {
        return -EINVAL;
    }

    // Write BOOTFSM_GO Register
    caliptra_write_u32(CALIPTRA_TOP_REG_GENERIC_AND_FUSE_REG_CPTRA_BOOTFSM_GO, 1);

    return 0;
}

static int caliptra_mailbox_write_fifo(struct caliptra_buffer *buffer)
{

    // Check against max size
    const uint32_t MBOX_SIZE = (128u * 1024u);
    if (buffer->len > MBOX_SIZE) {
        return -EINVAL;
    }

    // Write DLEN
    caliptra_mbox_write_dlen(buffer->len);

    uint32_t remaining_len = buffer->len;
    uint32_t *data_dw = (uint32_t *)buffer->data;

    // Copy DWord multiples
    while (remaining_len > sizeof(uint32_t)) {
        caliptra_mbox_write(MBOX_CSR_MBOX_DATAIN, *data_dw++);
        remaining_len -= sizeof(uint32_t);
    }

    // if un-aligned dword reminder...
    if (remaining_len) {
        uint32_t data = 0;
        memcpy(&data, data_dw, remaining_len);
        caliptra_mbox_write(MBOX_CSR_MBOX_DATAIN, data);
    }

    return 0;
}

static int caliptra_mailbox_read_buffer(struct caliptra_buffer *buffer)
{

    // Check we have enough room in the buffer
    if (buffer->len < caliptra_mbox_read_dlen() || !buffer->data)
       return -EINVAL;

    uint32_t remaining_len = caliptra_mbox_read_dlen();
    uint32_t *data_dw = (uint32_t *)buffer->data;

    // Copy DWord multiples
    while (remaining_len > sizeof(uint32_t)) {
        *data_dw++ = caliptra_mbox_read(MBOX_CSR_MBOX_DATAOUT);
        remaining_len -= sizeof(uint32_t);
    }

    // if un-aligned dword reminder...
    if (remaining_len) {
        uint32_t data = caliptra_mbox_read(MBOX_CSR_MBOX_DATAOUT);
        memcpy(data_dw, &data, remaining_len);
    }

    return 0;
}

int caliptra_mailbox_execute(uint32_t cmd, struct caliptra_buffer *mbox_tx_buffer, struct caliptra_buffer *mbox_rx_buffer)
{
    if (!caliptra_init_complete) {
        return -EINVAL;
    }

    // If mbox already locked return
    if (caliptra_mbox_is_lock()) {
        return -EBUSY;
    }

    // Write Cmd and Tx Buffer
    caliptra_mbox_write_cmd(cmd);
    caliptra_mailbox_write_fifo(mbox_tx_buffer);

    // Set Execute bit
    caliptra_mbox_write_execute(true);

    // Keep stepping until mbox status is busy
    while(caliptra_mbox_read_status() == CALIPTRA_MBOX_STATUS_BUSY)
        caliptra_step();

    // Check the Mailbox Status
    uint32_t status = caliptra_mbox_read_status();
    if (status == CALIPTRA_MBOX_STATUS_CMD_FAILURE) {
        caliptra_mbox_write_execute(false);
        return -EIO;
    } else if(status == CALIPTRA_MBOX_STATUS_CMD_COMPLETE) {
        caliptra_mbox_write_execute(false);
        return 0;
    } else if (status != CALIPTRA_MBOX_STATUS_DATA_READY) {
        return -EIO;
    }

    // Read Mbox out Data Len
    uint32_t dlen = caliptra_mbox_read_dlen();

    // Read Buffer
    caliptra_mailbox_read_buffer(mbox_rx_buffer);

    // Execute False
    caliptra_mbox_write_execute(false);

    // mbox_fsm_ps isn't updated immediately after execute is cleared (!?),
    // so step an extra clock cycle to wait for fm_ps to update
    caliptra_step();

    if (caliptra_mbox_read_status_fsm() != CALIPTRA_MBOX_STATUS_FSM_IDLE)
        return -EIO;

    return 0;
}

int caliptra_upload_fw(struct caliptra_buffer *fw_buffer)
{
    const uint32_t FW_LOAD_CMD_OPCODE = 0x46574C44u;
    return caliptra_mailbox_execute(FW_LOAD_CMD_OPCODE, fw_buffer, NULL);
}
