#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* BUFFER_SIZE must be a power of 2 for the bitwise AND trick to work */
#define BUFFER_SIZE     8u
#define BUFFER_MASK     (BUFFER_SIZE - 1u)

#define RB_OK           0
#define RB_ERR_FULL    -1
#define RB_ERR_EMPTY   -2

typedef struct {
    uint8_t  data[BUFFER_SIZE];
    uint8_t  head;   /* next write index */
    uint8_t  tail;   /* next read index  */
    uint8_t  count;  /* bytes currently stored */
} RingBuf;

/* ---------- API ---------- */

void rb_init(RingBuf *rb) {
    memset(rb->data, 0, sizeof(rb->data));
    rb->head  = 0u;
    rb->tail  = 0u;
    rb->count = 0u;
}

uint8_t rb_is_full(const RingBuf *rb) {
    return rb->count == BUFFER_SIZE;
}

uint8_t rb_is_empty(const RingBuf *rb) {
    return rb->count == 0u;
}

uint8_t rb_count(const RingBuf *rb) {
    return rb->count;
}

int rb_write(RingBuf *rb, uint8_t byte) {
    if (rb_is_full(rb)) {
        return RB_ERR_FULL;
    }
    rb->data[rb->head] = byte;
    rb->head = (uint8_t)((rb->head + 1u) & BUFFER_MASK); /* power-of-2 wrap */
    rb->count++;
    return RB_OK;
}

int rb_read(RingBuf *rb, uint8_t *out) {
    if (rb_is_empty(rb)) {
        return RB_ERR_EMPTY;
    }
    *out = rb->data[rb->tail];
    rb->tail = (uint8_t)((rb->tail + 1u) & BUFFER_MASK); /* power-of-2 wrap */
    rb->count--;
    return RB_OK;
}

/* ---------- Demo ---------- */

static void do_write(RingBuf *rb, uint8_t byte) {
    int ret = rb_write(rb, byte);
    if (ret == RB_OK) {
        printf("[WRITE] 0x%02X -> OK (count=%u)%s\n",
               byte, rb_count(rb),
               rb_is_full(rb) ? " FULL" : "");
    } else {
        printf("[WRITE] 0x%02X -> FAIL (buffer full)\n", byte);
    }
}

static void do_read(RingBuf *rb) {
    uint8_t val = 0u;
    int ret = rb_read(rb, &val);
    if (ret == RB_OK) {
        printf("[READ]  -> 0x%02X (count=%u)%s\n",
               val, rb_count(rb),
               rb_is_empty(rb) ? " EMPTY" : "");
    } else {
        printf("[READ]  (empty) -> FAIL (buffer empty)\n");
    }
}

int main(void) {
    RingBuf rb;
    rb_init(&rb);

    printf("=== Step 1: Write 8 bytes ===\n");
    uint8_t fill[] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
    for (int i = 0; i < 8; i++) {
        do_write(&rb, fill[i]);
    }

    printf("\n=== Step 2: Write past full ===\n");
    do_write(&rb, 0x99);

    printf("\n=== Step 3: Read 3 bytes ===\n");
    do_read(&rb);
    do_read(&rb);
    do_read(&rb);
    printf("count after 3 reads: %u\n", rb_count(&rb));

    printf("\n=== Step 4: Write 3 new bytes (reuse freed slots) ===\n");
    do_write(&rb, 0x49);
    do_write(&rb, 0x4A);
    do_write(&rb, 0x4B);
    printf("count after 3 writes: %u\n", rb_count(&rb));

    printf("\n=== Step 5: Read all 8 remaining bytes ===\n");
    for (int i = 0; i < 8; i++) {
        do_read(&rb);
    }

    printf("\n=== Step 6: Read from empty buffer ===\n");
    do_read(&rb);

    return 0;
}
