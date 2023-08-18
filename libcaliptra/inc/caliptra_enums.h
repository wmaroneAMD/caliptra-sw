// Licensed under the Apache-2.0 license
#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * device_lifecycle
 *
 * Device life cycle states
 */
enum device_lifecycle {
    Unprovisioned = 0,
    Manufacturing = 1,
    Reserved2 = 2,
    Production = 3,
};

/**
 * fips_status
 *
 * All valid FIPS status codes.
 */
enum fips_status {
    FIPS_STATUS_APPROVED = 0,
};

enum toc_entry_id {
    FMC     = 0x00000001,
    Runtime = 0x00000002,
    MAX     = 0xFFFFFFFF,
};

// The below enums are placeholders to set up the baseline
// required for communication of DPE commands to Caliptra
// firmware.

enum dpe_commands {
    DPE_GET_PROFILE        = 0x0,
    DPE_INITIALIZE_CONTEXT = 0x1,
    DPE_DERIVE_CHILD       = 0x2,
    DPE_CERTIFY_KEY        = 0x3,
    DPE_SIGN               = 0x4,
    DPE_ROTATE_CTX_HANDLE  = 0x5,
    DPE_DESTROY_CTX        = 0x6,
    DPE_GET_CERT_CHAIN     = 0x7,
    DPE_EXTEND_TCI         = 0x8,
    DPE_TAG_TCI            = 0x9,
    DPE_GET_TAGGED_TCI     = 0xA,
};
