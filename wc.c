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
#include <stdio.h>

int main(int argc, const char *argv[]) {
	if (argc < 3)
		return -1;
	const char *wc = argv[1];
	const char *test = argv[2];
	if (wc_matches(test, wc)) {
		printf("Matched\n");
	} else {
		printf("Not Matched\n");
	}
	return 0;
}
