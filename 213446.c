virtio_get_cap_id(uint64_t offset, int size)
{
	int i, rc = -1;

	for (i = 0; i < ARRAY_SIZE(cap_regions); i++) {
		if (offset >= cap_regions[i].cap_offset &&
			offset + size <= cap_regions[i].cap_offset +
			cap_regions[i].cap_size)
			return cap_regions[i].cap_id;
	}

	return rc;
}