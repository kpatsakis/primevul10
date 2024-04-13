void memslot_info_del_slot(RedMemSlotInfo *info, uint32_t slot_group_id, uint32_t slot_id)
{
    spice_return_if_fail(info->num_memslots_groups > slot_group_id);
    spice_return_if_fail(info->num_memslots > slot_id);

    info->mem_slots[slot_group_id][slot_id].virt_start_addr = 0;
    info->mem_slots[slot_group_id][slot_id].virt_end_addr = 0;
}