my_suspend_hook(my_bool suspend, void *data)
{
  struct my_hook_data *hook_data= (struct my_hook_data *)data;
  if (suspend)
  {
    hook_data->orig_vio= hook_data->orig_mysql->net.vio;
    hook_data->orig_mysql->net.vio= hook_data->new_mysql->net.vio;
  }
  else
    hook_data->orig_mysql->net.vio= hook_data->orig_vio;
}