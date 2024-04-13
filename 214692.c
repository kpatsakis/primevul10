

static void on_m2ts_dump_event(GF_M2TS_Demuxer *ts, u32 evt_type, void *par)
{
	u32 i, count;
	GF_M2TS_Program *prog;
	GF_M2TS_PES_PCK *pck;
	GF_M2TS_Dump *dumper = (GF_M2TS_Dump *)ts->user;

	switch (evt_type) {
	case GF_M2TS_EVT_PAT_FOUND:
		if (dumper->timestamps_info_file) {
			fprintf(dumper->timestamps_info_file, "%u\t%d\n", ts->pck_number, 0);
		}
		break;
	case GF_M2TS_EVT_PAT_UPDATE:
		if (dumper->timestamps_info_file) {
			fprintf(dumper->timestamps_info_file, "%u\t%d\n", ts->pck_number, 0);
		}
		break;
	case GF_M2TS_EVT_PAT_REPEAT:
		/* WARNING: We detect the pat on a repetition, probably to ensure that we also have seen all the PMT
		   To be checked */
		dumper->has_seen_pat = 1;
		if (dumper->timestamps_info_file) {
			fprintf(dumper->timestamps_info_file, "%u\t%d\n", ts->pck_number, 0);
		}
//		fprintf(stderr, "Repeated PAT found - %d programs\n", gf_list_count(ts->programs) );
		break;
	case GF_M2TS_EVT_CAT_FOUND:
		if (dumper->timestamps_info_file) {
			fprintf(dumper->timestamps_info_file, "%u\t%d\n", ts->pck_number, 0);
		}
		break;
	case GF_M2TS_EVT_CAT_UPDATE:
		if (dumper->timestamps_info_file) {
			fprintf(dumper->timestamps_info_file, "%u\t%d\n", ts->pck_number, 0);
		}
		break;
	case GF_M2TS_EVT_CAT_REPEAT:
		if (dumper->timestamps_info_file) {
			fprintf(dumper->timestamps_info_file, "%u\t%d\n", ts->pck_number, 0);
		}
		break;
	case GF_M2TS_EVT_PMT_FOUND:
		prog = (GF_M2TS_Program*)par;
		if (gf_list_count(ts->programs)>1 && prog->number!=dumper->prog_number)
			break;

		count = gf_list_count(prog->streams);

		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("Program number %d found - %d streams:\n", prog->number, count));
		for (i=0; i<count; i++) {
			GF_M2TS_ES *es = gf_list_get(prog->streams, i);
			if (es->pid == prog->pmt_pid) {
				GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("\tPID %d: Program Map Table\n", es->pid));
			} else {
				GF_M2TS_PES *pes = (GF_M2TS_PES *)es;
				gf_m2ts_set_pes_framing(pes, dumper->pes_out ? GF_M2TS_PES_FRAMING_RAW : GF_M2TS_PES_FRAMING_DEFAULT);
				GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("\tPID %d: %s ", pes->pid, gf_m2ts_get_stream_name(pes->stream_type) ));
				if (pes->mpeg4_es_id) GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, (" - MPEG-4 ES ID %d", pes->mpeg4_es_id));
				GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("\n"));
			}
		}
		if (dumper->timestamps_info_file) {
			fprintf(dumper->timestamps_info_file, "%u\t%d\n", ts->pck_number, prog->pmt_pid);
		}
		break;
	case GF_M2TS_EVT_PMT_UPDATE:
		prog = (GF_M2TS_Program*)par;
		if (gf_list_count(ts->programs)>1 && prog->number!=dumper->prog_number)
			break;
		if (dumper->timestamps_info_file) {
			fprintf(dumper->timestamps_info_file, "%u\t%d\n", ts->pck_number, prog->pmt_pid);
		}
		break;
	case GF_M2TS_EVT_PMT_REPEAT:
		prog = (GF_M2TS_Program*)par;
		if (gf_list_count(ts->programs)>1 && prog->number!=dumper->prog_number)
			break;
		if (dumper->timestamps_info_file) {
			fprintf(dumper->timestamps_info_file, "%u\t%d\n", ts->pck_number, prog->pmt_pid);
		}
		break;
	case GF_M2TS_EVT_SDT_FOUND:
#ifndef GPAC_DISABLE_LOG
		count = gf_list_count(ts->SDTs) ;
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("Program Description found - %d desc:\n", count));
		for (i=0; i<count; i++) {
			GF_M2TS_SDT *sdt = gf_list_get(ts->SDTs, i);
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("\tServiceID %d - Provider %s - Name %s\n", sdt->service_id, sdt->provider, sdt->service));
		}
#endif
		break;
	case GF_M2TS_EVT_SDT_UPDATE:
#ifndef GPAC_DISABLE_LOG
		count = gf_list_count(ts->SDTs) ;
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("Program Description updated - %d desc\n", count));
		for (i=0; i<count; i++) {
			GF_M2TS_SDT *sdt = gf_list_get(ts->SDTs, i);
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("\tServiceID %d - Provider %s - Name %s\n", sdt->service_id, sdt->provider, sdt->service));
		}
#endif
		break;
	case GF_M2TS_EVT_SDT_REPEAT:
		break;
	case GF_M2TS_EVT_PES_TIMING:
		pck = par;
		if (gf_list_count(ts->programs)>1 && pck->stream->program->number != dumper->prog_number)
			break;

		break;
	case GF_M2TS_EVT_PES_PCK:
		pck = par;
		if (gf_list_count(ts->programs)>1 && pck->stream->program->number != dumper->prog_number)
			break;
		if (dumper->has_seen_pat) {

			/*We need the interpolated PCR for the pcrb, hence moved this calculus out, and saving the calculated value in index_info to put it in the pcrb*/
			GF_M2TS_PES *pes = pck->stream;
			/*FIXME : not used GF_M2TS_Program *prog = pes->program; */
			/* Interpolated PCR value for the TS packet containing the PES header start */
			u64 interpolated_pcr_value = 0;
			if (pes->last_pcr_value && pes->before_last_pcr_value_pck_number && pes->last_pcr_value > pes->before_last_pcr_value) {
				u32 delta_pcr_pck_num = pes->last_pcr_value_pck_number - pes->before_last_pcr_value_pck_number;
				u32 delta_pts_pcr_pck_num = pes->pes_start_packet_number - pes->last_pcr_value_pck_number;
				u64 delta_pcr_value = pes->last_pcr_value - pes->before_last_pcr_value;
				if ((pes->pes_start_packet_number > pes->last_pcr_value_pck_number)
				        && (pes->last_pcr_value > pes->before_last_pcr_value)) {

					pes->last_pcr_value = pes->before_last_pcr_value;
				}
				/* we can compute the interpolated pcr value for the packet containing the PES header */
				interpolated_pcr_value = pes->last_pcr_value + (u64)((delta_pcr_value*delta_pts_pcr_pck_num*1.0)/delta_pcr_pck_num);
			}

			if (dumper->timestamps_info_file) {
				Double diff;
				fprintf(dumper->timestamps_info_file, "%u\t%d\t", pck->stream->pes_start_packet_number, pck->stream->pid);
				if (interpolated_pcr_value) fprintf(dumper->timestamps_info_file, "%f", interpolated_pcr_value/(300.0 * 90000));
				fprintf(dumper->timestamps_info_file, "\t");
				if (pck->DTS) fprintf(dumper->timestamps_info_file, "%f", (pck->DTS / 90000.0));
				fprintf(dumper->timestamps_info_file, "\t%f\t%d\t%d", pck->PTS / 90000.0, (pck->flags & GF_M2TS_PES_PCK_RAP) ? 1 : 0, (pck->flags & GF_M2TS_PES_PCK_DISCONTINUITY) ? 1 : 0);
				if (interpolated_pcr_value) {
					diff = (pck->DTS ? pck->DTS : pck->PTS) / 90000.0;
					diff -= pes->last_pcr_value / (300.0 * 90000);
					fprintf(dumper->timestamps_info_file, "\t%f\n", diff);
					if (diff<0) fprintf(stderr, "Warning: detected PTS/DTS value less than current PCR of %g sec\n", diff);
				} else {
					fprintf(dumper->timestamps_info_file, "\t\n");
				}
			}
		}

		if (dumper->has_seen_pat && dumper->pes_out && (dumper->dump_pid == pck->stream->pid)) {
			gf_fwrite(pck->data, pck->data_len, dumper->pes_out);
		}
		break;
	case GF_M2TS_EVT_PES_PCR:
		pck = par;
		if (gf_list_count(ts->programs)>1 && pck->stream->program->number != dumper->prog_number)
			break;
		if (dumper->timestamps_info_file) {
			fprintf(dumper->timestamps_info_file, "%u\t%d\t%f\t\t\t\t%d\n", pck->stream->program->last_pcr_value_pck_number, pck->stream->pid, pck->PTS / (300*90000.0), (pck->flags & GF_M2TS_PES_PCK_DISCONTINUITY) ? 1 : 0);
		}
		break;
	case GF_M2TS_EVT_SL_PCK:
#if 0
	{
		GF_M2TS_SL_PCK *sl_pck = par;
		if (dumper->pes_out && (dumper->dump_pid == sl_pck->stream->pid)) {
			GF_SLHeader header;
			u32 header_len;
			if (sl_pck->stream->mpeg4_es_id) {
				GF_ESD *esd = ((GF_M2TS_PES*)sl_pck->stream)->esd;
				if (!dumper->is_info_dumped) {
					if (esd->decoderConfig->decoderSpecificInfo) gf_fwrite(esd->decoderConfig->decoderSpecificInfo->data, esd->decoderConfig->decoderSpecificInfo->dataLength, dumper->pes_out_info);
					dumper->is_info_dumped = 1;
					fprintf(dumper->pes_out_nhml, "<NHNTStream version=\"1.0\" ");
					fprintf(dumper->pes_out_nhml, "timeScale=\"%d\" ", esd->slConfig->timestampResolution);
					fprintf(dumper->pes_out_nhml, "streamType=\"%d\" ", esd->decoderConfig->streamType);
					fprintf(dumper->pes_out_nhml, "objectTypeIndication=\"%d\" ", esd->decoderConfig->objectTypeIndication);
					if (esd->decoderConfig->decoderSpecificInfo) fprintf(dumper->pes_out_nhml, "specificInfoFile=\"%s\" ", dumper->info);
					fprintf(dumper->pes_out_nhml, "baseMediaFile=\"%s\" ", dumper->dump);
					fprintf(dumper->pes_out_nhml, "inRootOD=\"yes\">\n");
				}
				gf_sl_depacketize(esd->slConfig, &header, sl_pck->data, sl_pck->data_len, &header_len);
				gf_fwrite(sl_pck->data+header_len, sl_pck->data_len-header_len, dumper->pes_out);
				fprintf(dumper->pes_out_nhml, "<NHNTSample DTS=\""LLD"\" dataLength=\"%d\" isRAP=\"%s\"/>\n", header.decodingTimeStamp, sl_pck->data_len-header_len, (header.randomAccessPointFlag?"yes":"no"));
			}
		}
	}
#endif
	break;