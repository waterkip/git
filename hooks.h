#ifndef HOOKS_H
#define HOOKS_H

#include <stdarg.h>

/*
 * Add a generic mechanism to find and execute one or multiple hooks found
 * in $GIT_DIR/hooks/<hook> and/or $GIT_DIR/hooks/<hooks>.d/
 * The API is as follows:
 *
 *	#include "hooks.h"
 *
 *	array hooks   = find_hooks('pre-receive');
 *
 *	int hooks_ran = run_hooks_ve(hooks);
 *	int hooks_ran = run_hooks_le(hooks);
 *
 *	int hooks_ran = run_hooks_by_name_ve(name, args);
 *	int hooks_ran = run_hooks_by_name_le(name, args);
 *
 *
 * The implemented behaviour is:
 * * If we find a hooks/<hook>.d directory and the hooks.multiHook flag isn't
 *   set we make use of that directory.
 * * If we find a hooks/<hook>.d and we also have hooks/<hook> and the
 *   hooks.multiHook isn't set or set to false we don't use the hook.d
 *   directory. If the hook isn't set we issue a warning to the user
 *   telling him/her that we support multiple hooks via the .d directory
 *   structure
 * * If the hooks.multiHook is set to true we use the hooks/<hook> and all
 *   the entries found in hooks/<hook>.d
 * * All the scripts are executed and fail on the first error
*/

/*
 * Returns all the hooks found in either
 * $GIT_DIR/hooks/$hook and/or $GIT_DIR/hooks/$hook.d/
 *
 * Note that this points to static storage that will be
 * overwritten by further calls to find_hooks and run_hook_*.
 */

extern const struct string_list *find_hooks(const char *name);

extern const char *find_hook(const char *name);
extern int has_hook(const char *name);

//LAST_ARG_MUST_BE_NULL

//extern int run_hooks_by_name_le(
//	const char *name,
//	const char *const *env,
//	...
//);

extern int run_hooks_by_name_ve(const char *name, const char *const *env, va_list args);
/*
 * Run all the runnable hooks found in
 * $GIT_DIR/hooks/$hook and/or $GIT_DIR/hooks/$hook.d/
 *
 */

/*
extern int run_hooks_le(
	const struct string_list,
	const char *const *env,
	...
);

extern int run_hooks_ve(
	const struct string_list,
	const char *const *env,
	va_list args
);

extern int run_hooks_ve(
	const struct string_list,
	const char *const *env,
	va_list args
);


*/

#endif
