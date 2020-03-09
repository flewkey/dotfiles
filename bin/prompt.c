#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

char *get_user()
{
	return getenv("USER");
}

char *get_host()
{
	char *host;
	host = malloc(_POSIX_HOST_NAME_MAX);
	gethostname(host, _POSIX_HOST_NAME_MAX);
	return host;
}

char *get_wd()
{
	char *home;
	char wd_raw[PATH_MAX];
	char *wd;
	home = getenv("HOME");
	getcwd(wd_raw, sizeof(wd_raw));
	wd = malloc(PATH_MAX);
	if (memcmp(home, wd_raw, strlen(home)) == 0)
		sprintf(wd, "~%s", wd_raw + strlen(home));
	else
		sprintf(wd, "%s", wd_raw);
	return wd;
}

int main()
{
	printf("%s@%s %s", get_user(), get_host(), get_wd());
	return 0;
}

