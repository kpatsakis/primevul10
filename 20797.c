static unsigned long __get_clean_virt(RedMemSlotInfo *info, QXLPHYSICAL addr)
{
    return addr & info->memslot_clean_virt_mask;
}