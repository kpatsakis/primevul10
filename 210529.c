void RunAndQuit(const base::Closure& closure,
                const base::Closure& quit,
                base::MessageLoopProxy* original_message_loop) {
  closure.Run();
  original_message_loop->PostTask(FROM_HERE, quit);
}
