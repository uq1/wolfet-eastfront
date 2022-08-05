#include <unistd.h>
int main(int argc, char **argv) {
	char *params[1024];
	char *etpub_param[5] = {
		"+set", "fs_game", "etpub",
		"+exec", "server.cfg"
	};
	int i;

	params[0] = *argv;

	params[1] = etpub_param[0];
	params[2] = etpub_param[1];
	params[3] = etpub_param[2];
	params[4] = etpub_param[3];
	params[5] = etpub_param[4];

	i = 6;
	while(*++argv && i < 1024) { 
		params[i] = *argv;
		i++;
	}
	
	chdir("/Applications/Wolfenstein ET/etpub/");
	execv("/Applications/Wolfenstein ET/rtcw_et_server", params);
	return 0;
}
