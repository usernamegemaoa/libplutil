/*
  Configuration Utility functions - plconfig

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

#ifndef INCLUDE_PLCONFIG_H
#define INCLUDE_PLCONFIG_H 1

/**
   @file plconfig.h

   This file provides an API to access a PLSDK configuration file.
*/

/** Opaque structure used by public interface */
struct plconfig;

/** Create an initialised plconfig instance
    @param[in] file path to the config file, NULL to use default ~/.plsdk.ini
    @param[in] group configuration group name
    @return pointer to new plconfig instance or NULL if error
 */
extern struct plconfig *plconfig_init(const char *file, const char *group);

/** Free a plconfig instance
    @param[in] plconfig plconfig instance as created by plconfig_init
 */
extern void plconfig_free(struct plconfig *plconfig);

/** Get the path to the configuration file
    @param[in] plconfig plconfig instance
    @return path to the configuration file
 */
extern const char *plconfig_get_file_path(struct plconfig *plconfig);

/** Get the configuration group name
    @param[in] plconfig plconfig instance
    @return configuration group name
 */
extern const char *plconfig_get_group(struct plconfig *plconfig);

/** Get a configuration string value

    The memory associated with this string is allocated by plconfig and remains
    in the plconfig instance.  So the pointer remains valid until the plconfig
    instance gets destroyed by @ref plconfig_free.

    @param[in] plconfig plconfig instance
    @param[in] key name of the configuration entry
    @param[in] def default value
    @return configuration string value or default value if key not found
 */
extern const char *plconfig_get_str(struct plconfig *plconfig,
				    const char *key, const char *def);

/** Get a configuration integer value
    @param[in] plconfig plconfig instance
    @param[in] key name of the configuration entry
    @param[in] def default value
    @return configuration integer value or default value if key not found
 */
extern int plconfig_get_int(struct plconfig *plconfig, const char *key,
			    int def);

/** Get a configuration hex value

    The configuration value in the file is read as a hexadecimal number, which
    may contain a 0x prefix or h suffix for improved readability.

    @param[in] plconfig plconfig instance
    @param[in] key name of the configuration entry
    @param[in] def default value
    @return configuration hex value or default value if key not found
 */
extern unsigned long plconfig_get_hex(struct plconfig *plconfig,
				      const char *key, unsigned long def);

#endif /* INCLUDE_PLCONFIG_H */
