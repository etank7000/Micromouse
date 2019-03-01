#!/bin/bash
make -f mac-makefile && st-flash write ./build/chok_boi.bin 0x8000000
