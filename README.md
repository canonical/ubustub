# Ubustub

A minimal alternative to [systemd-stub(7)](https://manpages.ubuntu.com/manpages/plucky/man7/systemd-stub.7.html)
that serves a single purpose:

**Automatically loading machine specific device trees embedded within the
kernel image.**

ubustub is compatible with [systemd-stub(7)](https://manpages.ubuntu.com/manpages/plucky/man7/systemd-stub.7.html)
and [ukify(1)](https://manpages.ubuntu.com/manpages/plucky/man1/ukify.1.html).
It is designed to seamlessly integrate with Ubuntu's current bootloader and
boot security model. The resulting kernel image can be signed and verified
and loaded by grub like any other kernel.

Before loading the kernel, the stub generates
[hwids](https://github.com/fwupd/fwupd/blob/main/docs/hwids.md) of the
running machine derived from smbios and compares them to an embedded
lookup table in the .hwids section of the kernel image.
If a match is found it loads the corresponding device tree from the
.dtbauto section before jumping tothe bundled kernel.

## Dependencies

```
# apt install python3-pyelftools systemd-ukify
```

## Building

Build the stub:

```
$ make
```

For a simple combined kernel+ubustub image bundling a single DTB you can run:

```
$ ukify build --linux=/boot/vmlinuz --stub=ubustub.efi --hwids=hwids/json --dtbauto=/boot/dtb --output=vmlinuz.efi
```

## HWIDs

The `.txt` files in hwids/txt have been generated with `fwupdtool hwids`.
The can be converted to `.json` files by running `hwid2json.py` from the
`hwids` directory. The `compatible` field of the resulting JSON files has
to be filled in manually.

# Acknowledgements

This project is originally based on
[systemd-stub](https://manpages.ubuntu.com/manpages/plucky/man7/systemd-stub.7.html)
from the systemd project.
The `.dtbauto` feature in systemd was contributed by
[anonymix007](https://github.com/anonymix007/).
It is inspired by the [dtbloader](https://github.com/TravMurav/dtbloader)
project by Nikita Travkin and
[DtbLoader.efi](https://github.com/aarch64-laptops/edk2/tree/dtbloader-app)
from the aarch64-laptops project.
