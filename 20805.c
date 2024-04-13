int memslot_validate_virt(RedMemSlotInfo *info, unsigned long virt, int slot_id,
                          uint32_t add_size, uint32_t group_id)
{
    MemSlot *slot;

    slot = &info->mem_slots[group_id][slot_id];
    if ((virt + add_size) < virt) {
        spice_critical("virtual address overlap");
        return 0;
    }

    if (virt < slot->virt_start_addr || (virt + add_size) > slot->virt_end_addr) {
        print_memslots(info);
        spice_warning("virtual address out of range"
              "    virt=0x%lx+0x%x slot_id=%d group_id=%d\n"
              "    slot=0x%lx-0x%lx delta=0x%lx",
              virt, add_size, slot_id, group_id,
              slot->virt_start_addr, slot->virt_end_addr, slot->address_delta);
        return 0;
    }
    return 1;
}