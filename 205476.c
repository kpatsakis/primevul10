unsuppress_huff_table (JPEGState* sp, int tblno)
{
	JHUFF_TBL* htbl;

	if ((htbl = sp->cinfo.c.dc_huff_tbl_ptrs[tblno]) != NULL)
		htbl->sent_table = FALSE;
	if ((htbl = sp->cinfo.c.ac_huff_tbl_ptrs[tblno]) != NULL)
		htbl->sent_table = FALSE;
}