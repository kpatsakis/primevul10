process_cmd_dfreq(CMD_Request *msg, char *line)
{
  double dfreq;
  msg->command = htons(REQ_DFREQ);
  if (sscanf(line, "%lf", &dfreq) == 1) {
    msg->data.dfreq.dfreq = UTI_FloatHostToNetwork(dfreq);
  } else {
    msg->data.dfreq.dfreq = UTI_FloatHostToNetwork(0.0);
  }
}