static void fdctrl_to_command_phase(FDCtrl *fdctrl)
{
    fdctrl->phase = FD_PHASE_COMMAND;
    fdctrl->data_dir = FD_DIR_WRITE;
    fdctrl->data_pos = 0;
    fdctrl->data_len = 1; /* Accept command byte, adjust for params later */
    fdctrl->msr &= ~(FD_MSR_CMDBUSY | FD_MSR_DIO);
    fdctrl->msr |= FD_MSR_RQM;
}