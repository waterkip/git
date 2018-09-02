#include "hooks.h"
#include "cache.h"
#include "run-command.h"
#include "string-list.h"
#include "config.h"
#include <string.h>

//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/sysmacros.h>

static char *_get_hook(struct strbuf *path)
{
/* Windows file handling, add .exe */
#ifdef STRIP_EXTENSION
	strbuf_addstr(path, STRIP_EXTENSION);
#endif
	char *name = path->buf;
	int err;
	if (access(name, X_OK) >= 0) {
		return name;
	}
	err = errno;

	if (err == EACCES && advice_ignored_hook) {
		static struct string_list advise_given = STRING_LIST_INIT_DUP;

		if (!string_list_lookup(&advise_given, name)) {
			string_list_insert(&advise_given, name);
			advise(_("The '%s' hook was ignored because "
				 "it's not set as executable.\n"
				 "You can disable this warning with "
				 "`git config advice.ignoredHook false`."),
			       name);
		}
	}
	return NULL;
}

static int _is_hook_in_hooksd(const char *file)
{
	const char *hook_ext =
#ifdef STRIP_EXTENSION
	".hook.exe";
#else
	".hook";
#endif
	char *extn = strrchr(file, '.');
	if (strcmp(extn, hook_ext) == 0) {
		return 0;
	}
	return 1;
}

static void get_hooks_from_directory(const char *name, struct string_list *list)
{

	struct strbuf path = STRBUF_INIT;
	struct dirent *de;
	DIR *hooksd_dir;
	strbuf_git_path(&path, "hooks/%s.d", name);

	if ((hooksd_dir = opendir(path.buf)) == NULL) {
		return;
	}


	while ((de = readdir(hooksd_dir)) != NULL) {
		struct stat stbuf;
		struct strbuf hooksd_script = STRBUF_INIT;
		char *hook;

		strbuf_addf(&hooksd_script, "%s/%s", path.buf, de->d_name);

		if (stat(hooksd_script.buf, &stbuf) == -1) {
			continue;
		}
		else if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
			continue;
		}

		hook = _get_hook(&hooksd_script);
		if (hook && _is_hook_in_hooksd(hook) == 0) {
			string_list_insert(list, hook);
		}

	}

	return;
}

const char *find_hook(const char *name)
{
	const struct string_list *list;
	const char *rv;
	list = find_hooks(name);
	if (list->nr > 0) {
		rv = list->items[0].string;
	}
	else {
		rv = NULL;
	}
	return rv;
}

int has_hook(const char *name)
{
	const struct string_list *list;
	list = find_hooks(name);
	if (list->nr > 0)
		return 0;
	return 1;
}

const struct string_list *find_hooks(const char *name)
{

	static struct string_list advise_given = STRING_LIST_INIT_DUP;
	struct string_list tmp = STRING_LIST_INIT_NODUP;

	static struct string_list list = STRING_LIST_INIT_NODUP;
	const char *hook_path;
	const struct string_list *rv;

	struct strbuf path = STRBUF_INIT;
	strbuf_git_path(&path, "hooks/%s", name);

	string_list_clear(&list, sizeof(list));

	hook_path = _get_hook(&path);
	rv = &list;

	if (hook_path) {
		string_list_insert(&list, hook_path);
	}

	if (hookd_enabled == 0) {
		return rv;
	}

	if (hookd_enabled == 1) {
		get_hooks_from_directory(name, &list);
		return rv;
	}

	get_hooks_from_directory(name, &tmp);
	if (   hook_path && tmp.nr > 0
	    && !string_list_lookup(&advise_given, name)) {
		string_list_insert(&advise_given, name);
		advise(_("You have a hook plus hook.d dir for %s. This"
			 " behaviour is now supported by git.\nYou can"
			 " disable this warning with `git config"
			 " hooks.multiHook false` to disable reading\n"
			 "the hook.d directory or run `git config"
			 " hooks.multiHook true` to enable the\nhook.d"
			 " directory. Ignoring the hook.d directory"
			 " for now"), name);
	}
	return rv;
}

static int _run_hook_by_name_ve(const char *path, const char *const *env,
	va_list args)
{
	struct child_process hook = CHILD_PROCESS_INIT;
	const char *foo;

	argv_array_push(&hook.args, path);
	while ((foo = va_arg(args, const char *)))
	{
		argv_array_push(&hook.args, foo);
	}
	hook.env = env;
	hook.no_stdin = 1;
	hook.stdout_to_stderr = 1;

	return run_command(&hook);
}

int run_hooks_by_name_ve(const char *name, const char *const *env, va_list args)
{
	const struct string_list *p;
	int ret = 0;

	p = find_hooks(name);
	if (p->nr == 0) {
		return 0;
	}
	else {
		for (int i = 0; i < p->nr; i++) {
			const char *path;
			path = p->items[i].string;
			ret = _run_hook_by_name_ve(path, env, args);
			printf("Hook %s ran: %d\n", path, ret);
			if (ret != 0) {
				printf("Hook %s ran with error %d\n", path, ret);
				return ret;
			}
		}
	}
	return 0;
}

