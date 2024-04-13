process_cmd_reselectdist(CMD_Request *msg, char *line)
{
  double dist;
  int ok;
  msg->command = htons(REQ_RESELECTDISTANCE);
  if (sscanf(line, "%lf", &dist) == 1) {
    msg->data.reselect_distance.distance = UTI_FloatHostToNetwork(dist);
    ok = 1;
  } else {
    ok = 0;
  }
  return ok;
}