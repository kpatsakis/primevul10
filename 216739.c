is_exit_restart(int sig)
{
    return (sig == SIGUSR1 || sig == SIGTERM || sig == SIGHUP || sig == SIGINT);
}