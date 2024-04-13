handle_manual_list(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int n_samples;
  int i;
  RPY_ManualListSample *sample;
  RPT_ManualSamplesReport report[MAX_MANUAL_LIST_SAMPLES];

  tx_message->status = htons(STT_SUCCESS);
  tx_message->reply = htons(RPY_MANUAL_LIST);
  
  MNL_ReportSamples(report, MAX_MANUAL_LIST_SAMPLES, &n_samples);
  tx_message->data.manual_list.n_samples = htonl(n_samples);
  for (i=0; i<n_samples; i++) {
    sample = &tx_message->data.manual_list.samples[i];
    UTI_TimevalHostToNetwork(&report[i].when, &sample->when);
    sample->slewed_offset = UTI_FloatHostToNetwork(report[i].slewed_offset);
    sample->orig_offset = UTI_FloatHostToNetwork(report[i].orig_offset);
    sample->residual = UTI_FloatHostToNetwork(report[i].residual);
  }
}