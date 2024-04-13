void RunOnIOThread(const base::Closure& closure) {
  base::RunLoop run_loop;
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&RunAndQuit, closure, run_loop.QuitClosure(),
                 base::MessageLoopProxy::current()));
  run_loop.Run();
}
