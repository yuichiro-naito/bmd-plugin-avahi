#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/unistd.h>

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <bmd_plugin.h>

#define AVAHI_PUBLISH LOCALBASE "/bin/avahi-publish"

static int avahi_enable = 0;

static int
avahi_initialize()
{
	if (access(AVAHI_PUBLISH, R_OK | X_OK) == 0)
		avahi_enable = 1;

	return 0;
}

static int on_process_exit(int id, void *data);

static int
exec_avahi_publish(nvlist_t *config)
{
	pid_t pid;
	char  *args[6];
	sigset_t mask;

	args[0] = AVAHI_PUBLISH;
	args[1] = "-s";
	args[2] = (char *)nvlist_get_string(config, "name");
	args[3] = "_rfb._tcp";
	args[4] = (char *)nvlist_get_string(config, "port");
	args[5] = NULL;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);

	pid = fork();
	if (pid == 0) {
		sigprocmask(SIG_UNBLOCK, &mask, NULL);
		execv(args[0], args);
		exit(1);
	}

	if (pid > 0)
		plugin_wait_for_process(pid, on_process_exit, config);
	return pid;
}

static int
on_timer(int id, void *data)
{
	nvlist_t *config = data;
	pid_t pid = exec_avahi_publish(config);
	if (pid > 0) {
		if (nvlist_exists_number(config, "pid"))
			nvlist_free_number(config, "pid");
		nvlist_add_number(config, "pid", pid);
	}
	return 0;
}


static int
on_process_exit(int id, void *data)
{
	int status;
	if (waitpid(id, &status, WNOHANG) < 0)
		return -1;

	/* If avahi-publish exit on error, retry */
	if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
		plugin_set_timer(5, on_timer, data);

	return 0;
}

void
set_params(nvlist_t *config, struct vm_conf *conf)
{
	char num[16];

	if (nvlist_exists_string(config, "name"))
		nvlist_free_string(config, "name");
	if (nvlist_exists_string(config, "port"))
		nvlist_free_string(config, "port");

	nvlist_add_string(config, "name", get_name(conf));
	snprintf(num, sizeof(num), "%d", get_fbuf_port(conf));
	nvlist_add_string(config, "port", num);
}


static void
avahi_status_change(struct vm *vm, nvlist_t *config)
{
	pid_t pid;
	struct vm_conf *conf = vm_get_conf(vm);

	if (avahi_enable == 0 || is_fbuf_enable(conf) == false)
		return;

	pid = nvlist_exists_number(config, "pid") ?
		nvlist_take_number(config, "pid") : 0;

	switch (get_state(vm)) {
	case LOAD:
	case RUN:
		if (pid == 0) {
			set_params(config, conf);
			pid = exec_avahi_publish(config);
		}
		/* FALLTHROUGH */
	default:
		if (pid > 0)
			nvlist_add_number(config, "pid", pid);
		break;
	case TERMINATE:
		if (pid > 0)
			kill(pid, SIGINT);
		break;
	}
}

void
avahi_reload_config(nvlist_t *new_conf, nvlist_t *old_conf)
{
#define copy_conf(type, key)				\
	if (nvlist_exists_##type(old_conf, (key)))	\
		nvlist_add_##type(new_conf, (key),	\
				  nvlist_get_##type(old_conf, (key)))
	copy_conf(string, "name");
	copy_conf(number, "port");
	copy_conf(number, "pid");
}

PLUGIN_DESC plugin_desc = {
	.version = PLUGIN_VERSION,
	.name = "avahi",
	.initialize = avahi_initialize,
	.finalize =  NULL,
	.on_status_change = avahi_status_change,
	.parse_config = NULL,
	.method = NULL,
	.on_reload_config = avahi_reload_config,
};
