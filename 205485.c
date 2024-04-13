unsuppress_quant_table (JPEGState* sp, int tblno)
{
	JQUANT_TBL* qtbl;

	if ((qtbl = sp->cinfo.c.quant_tbl_ptrs[tblno]) != NULL)
		qtbl->sent_table = FALSE;
}