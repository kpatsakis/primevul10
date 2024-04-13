GF_Err stbl_AppendDegradation(GF_SampleTableBox *stbl, u16 DegradationPriority)
{
	if (!stbl->DegradationPriority) {
		stbl->DegradationPriority = (GF_DegradationPriorityBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STDP);
		if (!stbl->DegradationPriority) return GF_OUT_OF_MEM;
	}

	stbl->DegradationPriority->priorities = (u16 *)gf_realloc(stbl->DegradationPriority->priorities, sizeof(u16) * stbl->SampleSize->sampleCount);
	if (!stbl->DegradationPriority->priorities) return GF_OUT_OF_MEM;
	stbl->DegradationPriority->priorities[stbl->SampleSize->sampleCount-1] = DegradationPriority;
	stbl->DegradationPriority->nb_entries = stbl->SampleSize->sampleCount;
	return GF_OK;
}