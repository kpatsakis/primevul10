HashOneWord(
    Jsi_Hash *tablePtr,
    const void *key)
{
    uint64_t a0, a1;
    uint64_t y0, y1;
    uint64_t y2, y3;
    uint64_t p1, p2;
    uint64_t result;
    /* Compute key * GOLDEN_RATIO in 128-bit arithmetic */
    a0 = (uint64_t)key & 0x00000000FFFFFFFF;
    a1 = (uint64_t)key >> 32;

    y0 = a0 * 0x000000007f4a7c13;
    y1 = a0 * 0x000000009e3779b9;
    y2 = a1 * 0x000000007f4a7c13;
    y3 = a1 * 0x000000009e3779b9;
    y1 += y0 >> 32;     /* Can't carry */
    y1 += y2;           /* Might carry */
    if (y1 < y2) {
        y3 += (1LL << 32);  /* Propagate */
    }

    /* 128-bit product: p1 = loword, p2 = hiword */
    p1 = ((y1 & 0x00000000FFFFFFFF) << 32) + (y0 & 0x00000000FFFFFFFF);
    p2 = y3 + (y1 >> 32);

    /* Left shift the value downward by the size of the table */
    if (tablePtr->downShift > 0) {
        if (tablePtr->downShift < 64) {
            result = ((p2 << (64 - tablePtr->downShift)) |
                      (p1 >> (tablePtr->downShift & 63)));
        } else {
            result = p2 >> (tablePtr->downShift & 63);
        }
    } else {
        result = p1;
    }
    /* Finally mask off the high bits */
    return (jsi_Hash)(result & tablePtr->mask);
}