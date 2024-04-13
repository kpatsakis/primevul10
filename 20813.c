void memslot_info_add_slot(RedMemSlotInfo *info, uint32_t slot_group_id, uint32_t slot_id,
                           uint64_t addr_delta, unsigned long virt_start, unsigned long virt_end,
                           uint32_t generation)
{
    spice_assert(info->num_memslots_groups > slot_group_id);
    spice_assert(info->num_memslots > slot_id);

    info->mem_slots[slot_group_id][slot_id].address_delta = addr_delta;
    info->mem_slots[slot_group_id][slot_id].virt_start_addr = virt_start;
    info->mem_slots[slot_group_id][slot_id].virt_end_addr = virt_end;
    info->mem_slots[slot_group_id][slot_id].generation = generation;
}