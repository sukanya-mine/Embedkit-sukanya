# EmbedKit_Shreenag

**Author:** Shreenag  
**Assessment:** Embedded Developer Assignment | Fresher | Embed Square Solutions

## Modules

| File | Description |
|------|-------------|
| `ringbuf.c` | Circular FIFO buffer (8-byte, uint8_t) — models a UART receive buffer with ISR-safe write and main-loop read semantics |

## Build & Run

```bash
gcc -Wall -std=c99 ringbuf.c -o ringbuf
./ringbuf
```

Tested on Linux with GCC. Compiles with zero warnings and zero errors.

## Design Notes

- All indices use `uint8_t`; all data widths use fixed-width types from `<stdint.h>` — no bare `int` or `char`.
- **Bonus implemented:** wrap-around uses `& (BUFFER_SIZE - 1)` instead of `% BUFFER_SIZE`.  
  This is a single-cycle bitwise AND vs. a multi-cycle division on MCUs without a hardware divider (e.g. Cortex-M0/M0+). It is valid only when `BUFFER_SIZE` is a power of 2, because `2^N - 1` in binary is N ones, which masks any index into `[0, BUFFER_SIZE-1]` — equivalent to modulo, but without division.
