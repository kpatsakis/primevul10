static uint16_t dummy_section(struct uc_struct *uc, PhysPageMap *map, FlatView *fv, MemoryRegion *mr)
{
    assert(fv);
    MemoryRegionSection section = {
        .fv = fv,
        .mr = mr,
        .offset_within_address_space = 0,
        .offset_within_region = 0,
        .size = int128_2_64(),
    };

    return phys_section_add(uc, map, &section);
}