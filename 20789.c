void memslot_info_reset(RedMemSlotInfo *info)
{
        uint32_t i;
        for (i = 0; i < info->num_memslots_groups; ++i) {
            memset(info->mem_slots[i], 0, sizeof(MemSlot) * info->num_memslots);
        }
}