/*
  srcml_create_archive_memory.c

  Copyright (C) 2013  SDML (www.sdml.info)

  The srcML translator is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcML translator is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcML translator; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
  Example program of the use of the C API for srcML.

  Create an archive, file by file, with an output memory.
*/

#include "srcml.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    int i;
    struct srcml_archive* archive;
    struct srcml_unit* unit;
    char s[500];

    /* create a new srcml archive structure */
    archive = srcml_create_archive();

    /* open a srcML archive for output */
    srcml_write_open_memory(archive, s, 500);

    /* add all the files to the archive */
    for (i = 0; i < argc; ++i) {

        unit = srcml_create_unit();
        
        /* Translate to srcml and append to the archive */
        srcml_parse_unit_filename(unit, argv[i]);

        /* Translate to srcml and append to the archive */
        srcml_write_unit(archive, unit);

        srcml_free_unit(unit);
    }

    /* close the srcML archive */
    srcml_close_archive(archive);

    /* free the srcML archive data */
    srcml_free_archive(archive);

    /* now dump the contents of the archive */
    puts(s);

    return 0;
}
