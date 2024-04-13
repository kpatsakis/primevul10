void timer_output(void)
{
  if (timer_file)
  {
    char buf[32], *end;
    ulonglong timer= timer_now() - timer_start;
    end= longlong10_to_str(timer, buf, 10);
    str_to_file(timer_file,buf, (int) (end-buf));
    /* Timer has been written to the file, don't use it anymore */
    timer_file= 0;
  }
}