static void circular_memcpy(uint8_t* dst, uint8_t* window, const uint64_t mask,
    int64_t start, int64_t end)
{
	if((start & mask) > (end & mask)) {
		ssize_t len1 = mask + 1 - (start & mask);
		ssize_t len2 = end & mask;

		memcpy(dst, &window[start & mask], len1);
		memcpy(dst + len1, window, len2);
	} else {
		memcpy(dst, &window[start & mask], (size_t) (end - start));
	}
}