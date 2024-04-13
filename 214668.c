

static void print_config_hash(GF_List *xps_array, char *szName)
{
	u32 i, j;
	u8 hash[20];
	for (i=0; i<gf_list_count(xps_array); i++) {
		GF_AVCConfigSlot *slc = gf_list_get(xps_array, i);
		gf_sha1_csum((u8 *) slc->data, slc->size, hash);
		fprintf(stderr, "\t%s#%d hash: ", szName, i+1);
		for (j=0; j<20; j++) fprintf(stderr, "%02X", hash[j]);
		fprintf(stderr, "\n");