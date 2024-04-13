GF_Err gf_isom_make_interleave_ex(GF_ISOFile *file, GF_Fraction *fTimeInSec)
{
	GF_Err e;
	u64 itime;
	if (!file || !fTimeInSec->den || (fTimeInSec->num<=0)) return GF_BAD_PARAM;

	itime = (u64) fTimeInSec->num;
	itime *= gf_isom_get_timescale(file);
	itime /= fTimeInSec->den;
	if (file->storageMode==GF_ISOM_STORE_FASTSTART) {
		return gf_isom_set_interleave_time(file, (u32) itime);
	}
	if (gf_isom_get_mode(file) < GF_ISOM_OPEN_EDIT) return GF_BAD_PARAM;
	e = gf_isom_set_storage_mode(file, GF_ISOM_STORE_DRIFT_INTERLEAVED);
	if (e) return e;
	return gf_isom_set_interleave_time(file, (u32) itime);
}