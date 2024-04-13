adjust_poll(NCR_Instance inst, double adj)
{
  inst->poll_score += adj;

  if (inst->poll_score >= 1.0) {
    inst->local_poll += (int)inst->poll_score;
    inst->poll_score -= (int)inst->poll_score;
  }

  if (inst->poll_score < 0.0) {
    inst->local_poll += (int)(inst->poll_score - 1.0);
    inst->poll_score -= (int)(inst->poll_score - 1.0);
  }
  
  /* Clamp polling interval to defined range */
  if (inst->local_poll < inst->minpoll) {
    inst->local_poll = inst->minpoll;
    inst->poll_score = 0;
  } else if (inst->local_poll > inst->maxpoll) {
    inst->local_poll = inst->maxpoll;
    inst->poll_score = 1.0;
  }
}