/*
  Plastic Logic SDK - configuration file utility functions

  Copyright (C) 2013 Plastic Logic Limited

      Michele Marchetti <michele.marchetti@plasticlogic.com>
      Guillaume Tucker <guillaume.tucker@plasticlogic.com>

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <plsdk/plconfig.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#define LOG_TAG "plconfig"
#include <plsdk/log.h>

union plconfig_numeric {
	int int_value;
	unsigned long ulong_value;
};

struct plconfig_kv {
	char *key;
	char *str_value;
	union plconfig_numeric numeric_value;
};

struct plconfig {
	GKeyFile *key_file;
	GSList *cache;
	char *group;
	char *file_path;
};

static struct plconfig_kv *get_cached_kv(struct plconfig *p, const char *key);
static gint compare_kv(const struct plconfig_kv *kv, const char *key);
static struct plconfig_kv *add_kv(struct plconfig *p, const char *key);

struct plconfig *plconfig_init(const char *file, const char *group)
{
	struct plconfig *p;
	GError *err = NULL;

	assert(group != NULL);

	p = g_malloc(sizeof(struct plconfig));

	if (!file) {
		const char *home = getenv("HOME");

		if (home == NULL)
			goto err_free_plconfig;

		p->file_path = g_strdup_printf("%s/.plsdk.ini", home);
	} else {
		p->file_path = g_strdup(file);
	}

	p->cache = NULL;
	p->group = g_strdup(group);
	p->key_file = g_key_file_new();
	g_key_file_load_from_file(p->key_file, p->file_path, 0, &err);

	if (err != NULL)
		goto err_free_all;

	return p;

err_free_all:
	g_key_file_free(p->key_file);
	g_free(p->group);
	g_free(p->file_path);
err_free_plconfig:
	g_free(p);

	return NULL;
}

void plconfig_free(struct plconfig *p)
{
	GSList *it;

	for (it = p->cache; it != NULL; it = g_slist_next(it)) {
		struct plconfig_kv *kv = it->data;

		g_free(kv->key);

		if (kv->str_value != NULL)
			g_free(kv->str_value);
	}

	g_slist_free(p->cache);
	g_key_file_free(p->key_file);
	g_free(p->group);
	g_free(p->file_path);
	g_free(p);
}

const char *plconfig_get_file_path(struct plconfig *p)
{
	assert(p != NULL);

	return p->file_path;
}

const char *plconfig_get_group(struct plconfig *p)
{
	assert(p != NULL);

	return p->group;
}

const char *plconfig_get_str(struct plconfig *p, const char *key,
			     const char *def)
{
	struct plconfig_kv *kv;
	char *str_value;
	GError *err = NULL;

	assert(p != NULL);

	kv = get_cached_kv(p, key);

	if (kv != NULL)
		return kv->str_value;

	str_value = g_key_file_get_string(p->key_file, p->group, key, &err);

	if (err != NULL)
		return def;

	kv = add_kv(p, key);
	kv->str_value = str_value;

	return str_value;
}

int plconfig_get_int(struct plconfig *p, const char *key, int def)
{
	struct plconfig_kv *kv;
	int int_value;
	GError *err = NULL;

	assert(p != NULL);

	kv = get_cached_kv(p, key);

	if (kv != NULL)
		return kv->numeric_value.int_value;

	int_value = g_key_file_get_integer(p->key_file, p->group, key, &err);

	if (err != NULL)
		return def;

	kv = add_kv(p, key);
	kv->str_value = NULL;
	kv->numeric_value.int_value = int_value;

	return int_value;
}

unsigned long plconfig_get_hex(struct plconfig *p, const char *key,
			       unsigned long def)
{
	struct plconfig_kv *kv;
	unsigned ulong;
	char *str;
	GError *err = NULL;

	assert(p != NULL);

	kv = get_cached_kv(p, key);

	if (kv != NULL)
		return kv->numeric_value.ulong_value;

	str = g_key_file_get_string(p->key_file, p->group, key, &err);

	if (err != NULL)
		return def;

	errno = 0;
	ulong = strtoul(str, NULL, 16);

	if (errno) {
		LOG("warning: failed to parse hex number for %s", key);
		return def;
	}

	kv = add_kv(p, key);
	kv->str_value = str;
	kv->numeric_value.ulong_value = ulong;

	return kv->numeric_value.ulong_value;
}

/* ----------------------------------------------------------------------------
 * static functions
 */

static struct plconfig_kv *get_cached_kv(struct plconfig *p, const char *key)
{
	GSList *cached;

	cached = g_slist_find_custom(p->cache, key, (GCompareFunc)compare_kv);

	return (cached == NULL) ? NULL : cached->data;
}

static gint compare_kv(const struct plconfig_kv *kv, const char *key)
{
	return strcmp(kv->key, key);
}

static struct plconfig_kv *add_kv(struct plconfig *p, const char *key)
{
	struct plconfig_kv *kv;

	kv = g_malloc(sizeof(struct plconfig_kv));
	kv->key = g_strdup(key);
	p->cache = g_slist_prepend(p->cache, kv);

	return kv;
}
