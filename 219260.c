    yaffsfs_is_spare_valid(YAFFSFS_INFO * /*yfs*/, YaffsSpare *spare)
{
    if (spare == NULL) {
        return 1;
    }

    if ((spare->object_id > YAFFS_MAX_OBJECT_ID) ||
        (spare->seq_number < YAFFS_LOWEST_SEQUENCE_NUMBER) ||
        (spare->seq_number > YAFFS_HIGHEST_SEQUENCE_NUMBER)) {
            return 1;
    }

    return 0;
}