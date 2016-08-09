/* Header file for vos Dialogs */

int msgConfirm(const char *str, char **opcs, int nOpcs);
void msgBox(const char *str, const char *strBtn);
void progress(const char *str, int *progress);
char *getStrBox(const char *str, char *ret, int nCars);
