unsigned long memslot_max_size_virt(RedMemSlotInfo *info,
                                    unsigned long virt, int slot_id,
                                    uint32_t group_id)
{
    MemSlot *slot;

    slot = &info->mem_slots[group_id][slot_id];

    if (virt < slot->virt_start_addr || virt > slot->virt_end_addr) {
        return 0;
    }
    return slot->virt_end_addr - virt;
}