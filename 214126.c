static void write_filter_data(struct rar5* rar, uint32_t offset,
    uint32_t value)
{
	archive_le32enc(&rar->cstate.filtered_buf[offset], value);
}