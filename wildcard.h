/*************************************************************************
 * This file is part of jackmono
 * github.con/univrsal/jackmono
 * Copyright 2020 univrsal <universailp@web.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#ifndef WC_H
#define WC_H
#include <stdbool.h>

/**
 * \brief Matches a string against another one
 * containing wildcards (*)
 * \param what the string to check
 * \param wc the wildcard string
 * \return true if the wildcard matches the string
 */
bool wc_matches(const char *what, const char *wc);

#endif
