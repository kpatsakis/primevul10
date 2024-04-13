static inline unsigned int alignDiff(rpm_tagtype_t type, unsigned int alignsize)
{
    int typesize = typeSizes[type];

    if (typesize > 1) {
	unsigned int diff = typesize - (alignsize % typesize);
	if (diff != typesize)
	    return diff;
    }
    return 0;
}