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

#include "wildcard.h"
#include <string.h>

bool wc_matches(const char *what, const char *wc)
{
	size_t in_len = strlen(what);
	size_t wc_len = strlen(wc);

	size_t wc_begin = 0;
	size_t idx = 0;
	size_t match = 0;

	bool result = false;

	if (in_len < 1 || wc_len < 1)
		return false;

	/* Find the beginning in the wildcard */
	while (wc[wc_begin] != '\0' && wc[wc_begin] == '*')
		wc_begin++;

	while (idx < in_len) {
		match = 0;
		/* Find the beginning of the wildcard in the string */
		while (what[idx] != '\0' && what[idx] != wc[wc_begin])
			idx++;

		/* Match from the beginning through */
		while (idx + match < in_len && wc_begin + match < wc_len) {
			if (what[idx + match] != wc[wc_begin + match] &&
			        wc[wc_begin + match] != '*')
			{
				break;
			}
			match++;
		}

		if (match == wc_len - wc_begin) {
			result = true;
			break;
		}
		idx++;
	}
	return result;
}
