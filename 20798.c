void memslot_info_init(RedMemSlotInfo *info,
                       uint32_t num_groups, uint32_t num_slots,
                       uint8_t generation_bits,
                       uint8_t id_bits,
                       uint8_t internal_groupslot_id)
{
    uint32_t i;

    spice_assert(num_slots > 0);
    spice_assert(num_groups > 0);

    info->num_memslots_groups = num_groups;
    info->num_memslots = num_slots;
    info->generation_bits = generation_bits;
    info->mem_slot_bits = id_bits;
    info->internal_groupslot_id = internal_groupslot_id;

    info->mem_slots = g_new(MemSlot *, num_groups);

    for (i = 0; i < num_groups; ++i) {
        info->mem_slots[i] = g_new0(MemSlot, num_slots);
    }

    /* TODO: use QXLPHYSICAL_BITS */
    info->memslot_id_shift = 64 - info->mem_slot_bits;
    info->memslot_gen_shift = 64 - (info->mem_slot_bits + info->generation_bits);
    info->memslot_gen_mask = ~((QXLPHYSICAL)-1 << info->generation_bits);
    info->memslot_clean_virt_mask = (((QXLPHYSICAL)(-1)) >>
                                       (info->mem_slot_bits + info->generation_bits));
}