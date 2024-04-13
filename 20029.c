int ram_block_discard_range(struct uc_struct *uc, RAMBlock *rb, uint64_t start, size_t length)
{
    int ret = -1;

    uint8_t *host_startaddr = rb->host + start;

    if (!QEMU_PTR_IS_ALIGNED(host_startaddr, rb->page_size)) {
        //error_report("ram_block_discard_range: Unaligned start address: %p",
        //             host_startaddr);
        goto err;
    }

    if ((start + length) <= rb->used_length) {
        bool need_madvise;
        if (!QEMU_IS_ALIGNED(length, rb->page_size)) {
            //error_report("ram_block_discard_range: Unaligned length: %zx",
            //             length);
            goto err;
        }

        errno = ENOTSUP; /* If we are missing MADVISE etc */

        /* The logic here is messy;
         *    madvise DONTNEED fails for hugepages
         *    fallocate works on hugepages and shmem
         */
        need_madvise = (rb->page_size == uc->qemu_host_page_size);
        if (need_madvise) {
            /* For normal RAM this causes it to be unmapped,
             * for shared memory it causes the local mapping to disappear
             * and to fall back on the file contents (which we just
             * fallocate'd away).
             */
#if defined(CONFIG_MADVISE)
            ret =  madvise(host_startaddr, length, MADV_DONTNEED);
            if (ret) {
                ret = -errno;
                //error_report("ram_block_discard_range: Failed to discard range "
                //             "%s:%" PRIx64 " +%zx (%d)",
                //             rb->idstr, start, length, ret);
                goto err;
            }
#else
            ret = -ENOSYS;
            //error_report("ram_block_discard_range: MADVISE not available"
            //             "%s:%" PRIx64 " +%zx (%d)",
            //             rb->idstr, start, length, ret);
            goto err;
#endif
        }
    } else {
        //error_report("ram_block_discard_range: Overrun block '%s' (%" PRIu64
        //             "/%zx/" RAM_ADDR_FMT")",
        //             rb->idstr, start, length, rb->used_length);
    }

err:
    return ret;
}