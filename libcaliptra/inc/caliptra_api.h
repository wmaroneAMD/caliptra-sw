// Licensed under the Apache-2.0 license
#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct caliptra_buffer {
  const uint8_t *data;
  uintptr_t len;
} caliptra_buffer;

typedef struct caliptra_init_params {
  struct caliptra_buffer rom;
  struct caliptra_buffer dccm;
  struct caliptra_buffer iccm;
} caliptra_init_params;

enum DeviceLifecycle {
    Unprovisioned = 0,
    Manufacturing = 1,
    Reserved2 = 2,
    Production = 3,
};

struct caliptra_fuses {
    uint32_t uds_seed[12];
    uint32_t field_entropy[8];
    uint32_t key_manifest_pk_hash[12];
    uint32_t key_manifest_pk_hash_mask : 4;
    uint32_t rsvd : 28;
    uint32_t owner_pk_hash[12];
    uint32_t fmc_key_manifest_svn;
    uint32_t runtime_svn[4];
    bool anti_rollback_disable;
    uint32_t idevid_cert_attr[24];
    uint32_t idevid_manuf_hsm_id[4];
    enum DeviceLifecycle life_cycle;
};

int caliptra_init(struct caliptra_init_params *params);

// Initialize Caliptra fuses prior to boot
int caliptra_init_fuses(struct caliptra_fuses *fuses);

// Write into Caliptra BootFSM Go Register
int caliptra_bootfsm_go();

// Upload Caliptra Firmware
int caliptra_upload_fw(struct caliptra_buffer *fw_buffer);

// Execute Mailbox Command
int caliptra_mailbox_execute(uint32_t cmd, struct caliptra_buffer *mbox_tx_buffer, struct caliptra_buffer *mbox_rx_buffer);
