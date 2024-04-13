GF_Err gf_isom_clone_pssh(GF_ISOFile *output, GF_ISOFile *input, Bool in_moof) {
	GF_Box *a;
	u32 i;
	i = 0;

	while ((a = (GF_Box *)gf_list_enum(input->moov->child_boxes, &i))) {
		if (a->type == GF_ISOM_BOX_TYPE_PSSH) {
			GF_List **child_boxes = &output->moov->child_boxes;
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
			if (in_moof)
				child_boxes = &output->moof->child_boxes;
#endif

			GF_ProtectionSystemHeaderBox *pssh = (GF_ProtectionSystemHeaderBox *)gf_isom_box_new_parent(child_boxes, GF_ISOM_BOX_TYPE_PSSH);
			if (!pssh) return GF_OUT_OF_MEM;
			memmove(pssh->SystemID, ((GF_ProtectionSystemHeaderBox *)a)->SystemID, 16);
			if (((GF_ProtectionSystemHeaderBox *)a)->KIDs && ((GF_ProtectionSystemHeaderBox *)a)->KID_count > 0) {
				pssh->KID_count = ((GF_ProtectionSystemHeaderBox *)a)->KID_count;
				pssh->KIDs = (bin128 *)gf_malloc(pssh->KID_count*sizeof(bin128));
				if (!pssh->KIDs) return GF_OUT_OF_MEM;
				memmove(pssh->KIDs, ((GF_ProtectionSystemHeaderBox *)a)->KIDs, pssh->KID_count*sizeof(bin128));
			}
			pssh->private_data_size = ((GF_ProtectionSystemHeaderBox *)a)->private_data_size;
			pssh->private_data = (u8 *)gf_malloc(pssh->private_data_size*sizeof(char));
			if (!pssh->private_data) return GF_OUT_OF_MEM;
			memmove(pssh->private_data, ((GF_ProtectionSystemHeaderBox *)a)->private_data, pssh->private_data_size);
		}
	}
	return GF_OK;
}