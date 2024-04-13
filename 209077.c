void add_subst_list(char list[][NAME_LENGTH], char value[][NAME_LENGTH], char *item, char *str, int *i) {
   strlcpy(list[*i], item, NAME_LENGTH);
   strlcpy(value[(*i)++], str, NAME_LENGTH);
}