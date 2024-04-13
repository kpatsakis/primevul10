static void jsi_InteractiveSignal(int sig){
    if (jsi_interactiveInterp)
        jsi_interactiveInterp->interrupted = 1;
}