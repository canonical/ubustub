/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#define EINVAL 22
#include "string-util-fundamental.h"

/* Features of the loader, i.e. systemd-boot */
#define EFI_LOADER_FEATURE_CONFIG_TIMEOUT          (UINT64_C(1) << 0)
#define EFI_LOADER_FEATURE_CONFIG_TIMEOUT_ONE_SHOT (UINT64_C(1) << 1)
#define EFI_LOADER_FEATURE_ENTRY_DEFAULT           (UINT64_C(1) << 2)
#define EFI_LOADER_FEATURE_ENTRY_ONESHOT           (UINT64_C(1) << 3)
#define EFI_LOADER_FEATURE_BOOT_COUNTING           (UINT64_C(1) << 4)
#define EFI_LOADER_FEATURE_XBOOTLDR                (UINT64_C(1) << 5)
#define EFI_LOADER_FEATURE_RANDOM_SEED             (UINT64_C(1) << 6)
#define EFI_LOADER_FEATURE_LOAD_DRIVER             (UINT64_C(1) << 7)
#define EFI_LOADER_FEATURE_SORT_KEY                (UINT64_C(1) << 8)
#define EFI_LOADER_FEATURE_SAVED_ENTRY             (UINT64_C(1) << 9)
#define EFI_LOADER_FEATURE_DEVICETREE              (UINT64_C(1) << 10)
#define EFI_LOADER_FEATURE_SECUREBOOT_ENROLL       (UINT64_C(1) << 11)
#define EFI_LOADER_FEATURE_RETAIN_SHIM             (UINT64_C(1) << 12)
#define EFI_LOADER_FEATURE_MENU_DISABLE            (UINT64_C(1) << 13)
#define EFI_LOADER_FEATURE_MULTI_PROFILE_UKI       (UINT64_C(1) << 14)
#define EFI_LOADER_FEATURE_REPORT_URL              (UINT64_C(1) << 15)
#define EFI_LOADER_FEATURE_TYPE1_UKI               (UINT64_C(1) << 16)
#define EFI_LOADER_FEATURE_TYPE1_UKI_URL           (UINT64_C(1) << 17)

/* Features of the stub, i.e. systemd-stub */
#define EFI_STUB_FEATURE_REPORT_BOOT_PARTITION     (UINT64_C(1) << 0)
#define EFI_STUB_FEATURE_PICK_UP_CREDENTIALS       (UINT64_C(1) << 1)
#define EFI_STUB_FEATURE_PICK_UP_SYSEXTS           (UINT64_C(1) << 2)
#define EFI_STUB_FEATURE_THREE_PCRS                (UINT64_C(1) << 3)
#define EFI_STUB_FEATURE_RANDOM_SEED               (UINT64_C(1) << 4)
#define EFI_STUB_FEATURE_CMDLINE_ADDONS            (UINT64_C(1) << 5)
#define EFI_STUB_FEATURE_CMDLINE_SMBIOS            (UINT64_C(1) << 6)
#define EFI_STUB_FEATURE_DEVICETREE_ADDONS         (UINT64_C(1) << 7)
#define EFI_STUB_FEATURE_PICK_UP_CONFEXTS          (UINT64_C(1) << 8)
#define EFI_STUB_FEATURE_MULTI_PROFILE_UKI         (UINT64_C(1) << 9)
#define EFI_STUB_FEATURE_REPORT_STUB_PARTITION     (UINT64_C(1) << 10)
#define EFI_STUB_FEATURE_REPORT_URL                (UINT64_C(1) << 11)
