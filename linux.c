/* SPDX-License-Identifier: LGPL-2.1-or-later */

/*
 * Generic Linux boot protocol using the EFI/PE entry point of the kernel. Passes
 * initrd with the LINUX_INITRD_MEDIA_GUID DevicePath and cmdline with
 * EFI LoadedImageProtocol.
 *
 * This method works for Linux 5.8 and newer on ARM/Aarch64, x86/x68_64 and RISC-V.
 */

#include "efi-log.h"
#include "initrd.h"
#include "linux.h"
#include "pe.h"
#include "proto/device-path.h"
#include "proto/loaded-image.h"
#include "secure-boot.h"
#include "util.h"

#define STUB_PAYLOAD_GUID \
        { 0x55c5d1f8, 0x04cd, 0x46b5, { 0x8a, 0x20, 0xe5, 0x6c, 0xbb, 0x30, 0x52, 0xd0 } }

typedef struct {
        const void *addr;
        size_t len;
        const EFI_DEVICE_PATH *device_path;
} ValidationContext;

static bool validate_payload(
                const void *ctx, const EFI_DEVICE_PATH *device_path, const void *file_buffer, size_t file_size) {

        const ValidationContext *payload = ASSERT_PTR(ctx);

        if (device_path != payload->device_path)
                return false;

        /* Security arch (1) protocol does not provide a file buffer. Instead we are supposed to fetch the payload
         * ourselves, which is not needed as we already have everything in memory and the device paths match. */
        if (file_buffer && (file_buffer != payload->addr || file_size != payload->len))
                return false;

        return true;
}

static EFI_STATUS load_image(EFI_HANDLE parent, const void *source, size_t len, EFI_HANDLE *ret_image) {
        assert(parent);
        assert(source);
        assert(ret_image);

        /* We could pass a NULL device path, but it's nicer to provide something and it allows us to identify
         * the loaded image from within the security hooks. */
        struct {
                VENDOR_DEVICE_PATH payload;
                EFI_DEVICE_PATH end;
        } _packed_ payload_device_path = {
                .payload = {
                        .Header = {
                                .Type = MEDIA_DEVICE_PATH,
                                .SubType = MEDIA_VENDOR_DP,
                                .Length = sizeof(payload_device_path.payload),
                        },
                        .Guid = STUB_PAYLOAD_GUID,
                },
                .end = {
                        .Type = END_DEVICE_PATH_TYPE,
                        .SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE,
                        .Length = sizeof(payload_device_path.end),
                },
        };

        /* We want to support unsigned kernel images as payload, which is safe to do under secure boot
         * because it is embedded in this stub loader (and since it is already running it must be trusted). */
        install_security_override(
                        validate_payload,
                        &(ValidationContext) {
                                .addr = source,
                                .len = len,
                                .device_path = &payload_device_path.payload.Header,
                        });

        EFI_STATUS ret = BS->LoadImage(
                        /*BootPolicy=*/false,
                        parent,
                        &payload_device_path.payload.Header,
                        (void *) source,
                        len,
                        ret_image);

        uninstall_security_override();

        return ret;
}

EFI_STATUS linux_exec(
                EFI_HANDLE parent,
                const char16_t *cmdline,
                const struct iovec *kernel,
                const struct iovec *initrd) {

        size_t kernel_size_in_memory = 0;
        uint32_t compat_entry_point, entry_point;
        uint64_t image_base;
        EFI_STATUS err;

        assert(parent);
        assert(iovec_is_set(kernel));
        assert(iovec_is_valid(initrd));

        err = pe_kernel_info(kernel->iov_base, &entry_point, &compat_entry_point, &image_base, &kernel_size_in_memory);
        if (err == EFI_UNSUPPORTED)
                return log_error_status(err, "EFI_UNSUPPORTED: %m");
        if (err != EFI_SUCCESS)
                return log_error_status(err, "Bad kernel image: %m");

        _cleanup_(unload_imagep) EFI_HANDLE kernel_image = NULL;
        err = load_image(parent, kernel->iov_base, kernel->iov_len, &kernel_image);
        if (err != EFI_SUCCESS)
                return log_error_status(err, "Error loading kernel image: %m");

        EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
        err = BS->HandleProtocol(
                        kernel_image, MAKE_GUID_PTR(EFI_LOADED_IMAGE_PROTOCOL), (void **) &loaded_image);
        if (err != EFI_SUCCESS)
                return log_error_status(err, "Error getting kernel loaded image protocol: %m");

        if (cmdline) {
                loaded_image->LoadOptions = (void *) cmdline;
                loaded_image->LoadOptionsSize = strsize16(loaded_image->LoadOptions);
        }

        _cleanup_(cleanup_initrd) EFI_HANDLE initrd_handle = NULL;
        err = initrd_register(initrd->iov_base, initrd->iov_len, &initrd_handle);
        if (err != EFI_SUCCESS)
                return log_error_status(err, "Error registering initrd: %m");

        log_wait();
        err = BS->StartImage(kernel_image, NULL, NULL);

        /* Try calling the kernel compat entry point if one exists. */
        if (err == EFI_UNSUPPORTED && compat_entry_point > 0) {
                EFI_IMAGE_ENTRY_POINT compat_entry =
                                (EFI_IMAGE_ENTRY_POINT) ((uint8_t *) loaded_image->ImageBase + compat_entry_point);
                err = compat_entry(kernel_image, ST);
        }

        return log_error_status(err, "Error starting kernel image: %m");
}
