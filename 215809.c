GF_Err stbl_RemoveSubSample(GF_SampleTableBox *stbl, u32 SampleNumber)
{
	u32 i, count, j, subs_count, prev_sample, delta=0;

	if (! stbl->sub_samples) return GF_OK;
	subs_count = gf_list_count(stbl->sub_samples);
	for (j=0; j<subs_count; j++) {
		GF_SubSampleInformationBox *subs = gf_list_get(stbl->sub_samples, j);
		if (! subs->Samples) continue;

		prev_sample = 0;
		count = gf_list_count(subs->Samples);
		for (i=0; i<count; i++) {
			GF_SubSampleInfoEntry *e = gf_list_get(subs->Samples, i);
			prev_sample += e->sample_delta;
			if (prev_sample==SampleNumber) {
				gf_list_rem(subs->Samples, i);
				while (gf_list_count(e->SubSamples)) {
					GF_SubSampleEntry *pSubSamp = (GF_SubSampleEntry*) gf_list_get(e->SubSamples, 0);
					gf_free(pSubSamp);
					gf_list_rem(e->SubSamples, 0);
				}
				gf_list_del(e->SubSamples);
				gf_free(e);
				i--;
				count--;
				delta=1;
				continue;
			}
			e->sample_delta+=delta;
		}
	}
	return GF_OK;
}