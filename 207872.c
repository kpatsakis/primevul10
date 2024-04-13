void loongarch_cpu_list(void)
{
    GSList *list;
    list = object_class_get_list_sorted(TYPE_LOONGARCH_CPU, false);
    g_slist_foreach(list, loongarch_cpu_list_entry, NULL);
    g_slist_free(list);
}