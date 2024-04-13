static void adjust_endianness(MemoryRegion *mr, uint64_t *data, MemOp op)
{
    if ((op & MO_BSWAP) != devend_memop(mr->ops->endianness)) {
        switch (op & MO_SIZE) {
        case MO_8:
            break;
        case MO_16:
            *data = bswap16(*data);
            break;
        case MO_32:
            *data = bswap32(*data);
            break;
        case MO_64:
            *data = bswap64(*data);
            break;
        default:
            g_assert_not_reached();
        }
    }
}