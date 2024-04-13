static void print_memslots(RedMemSlotInfo *info)
{
    int i;
    int x;

    for (i = 0; i < info->num_memslots_groups; ++i) {
        for (x = 0; x < info->num_memslots; ++x) {
            if (!info->mem_slots[i][x].virt_start_addr &&
                !info->mem_slots[i][x].virt_end_addr) {
                continue;
            }
            printf("id %d, group %d, virt start %lx, virt end %lx, generation %u, delta %lx\n",
                   x, i, info->mem_slots[i][x].virt_start_addr,
                   info->mem_slots[i][x].virt_end_addr, info->mem_slots[i][x].generation,
                   info->mem_slots[i][x].address_delta);
            }
    }
}