# Boot-Stack-Size modification
The OCU program uses more memory than originally allocated. The lack of memory of the STM32F303K8 therefore requires the boot-stack-size to be lowered.

1. Navigate to mbed-os/targets/targets.json
2. Modify "boot-stack-size" under "Target","config" from 0x1000 to 0x200

After modification it should look like,
"boot-stack-size": {
    "help": "Define the boot stack size in bytes. This value must be a multiple of 8",
    "value": "0x200"
}

This needs to be done for every update/new download of mbed-os since mbed studio overwrites the whole folder.

Tested on mbed-os 6.9