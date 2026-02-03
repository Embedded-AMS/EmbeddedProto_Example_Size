#!/bin/bash
# Minimal script to flash nucleo-f446re using OpenOCD

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ELF_FILE="${SCRIPT_DIR}/build/bin/nucleo_f446re_app.elf"

if [ ! -f "$ELF_FILE" ]; then
    echo "Error: ELF file not found: $ELF_FILE"
    echo "Please build the project first."
    exit 1
fi

openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
    -c "program ${ELF_FILE} verify reset exit"
