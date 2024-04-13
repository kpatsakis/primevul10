GF_Err gf_isom_make_interleave(GF_ISOFile *file, Double TimeInSec)
{
	GF_Fraction f;
	f.num = (s32) (TimeInSec * 1000);
	f.den = 1000;
	return gf_isom_make_interleave_ex(file, &f);

}