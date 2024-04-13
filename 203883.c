start_initial_timeout(NCR_Instance inst)
{
  if (!inst->timer_running) {
    /* This will be the first transmission after mode change */

    /* Mark source active */
    SRC_SetActive(inst->source);
  }

  restart_timeout(inst, INITIAL_DELAY);
}