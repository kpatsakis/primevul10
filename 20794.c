void memslot_info_destroy(RedMemSlotInfo *info)
{
    uint32_t i;

    for (i = 0; i < info->num_memslots_groups; ++i) {
        g_free(info->mem_slots[i]);
    }
    g_free(info->mem_slots);
}