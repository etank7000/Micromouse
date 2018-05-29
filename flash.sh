#!/bin/bash
make && st-flash write ./build/chok_boi.bin 0x8000000
