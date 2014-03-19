/**
 * @file srcml_read_archive_fd.c
 *
 * @copyright Copyright (C) 2013-2014 SDML (www.srcML.org)
 *
 * The srcML Toolkit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The srcML Toolkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
  Example program of the use of the C API for srcML.

  Take an archive and extract the invidual units and write to a filesystem.
*/

#include "srcml.h"
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _MSC_BUILD  
#include <io.h>
#include windows_macros.h
#else
#include <unistd.h>
#endif
  
int main(int argc, char* argv[]) {
    int srcml_input;
    int srcml_output;
    const char* language;
    const char* filename;
    struct srcml_archive* archive;
    struct srcml_unit* unit;

    /* create a new srcml archive structure */
    archive = srcml_create_archive();

    /* open a srcML archive for input */
    srcml_input = open("project.xml", O_RDONLY);
    srcml_read_open_fd(archive, srcml_input);

    /* add all the files to the archive */
    while ((unit = srcml_read_unit(archive))) {

        /* can inquire about the current unit */
        language = srcml_unit_get_language(unit);
        filename = srcml_unit_get_filename(unit);

        /* uparse and write to a file */
        srcml_output = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        srcml_unparse_unit_fd(unit, srcml_output);

        srcml_free_unit(unit);
        close(srcml_output);
    }


    /* close the srcML archive */
    srcml_close_archive(archive);
    close(srcml_input);

    /* free the srcML archive data */
    srcml_free_archive(archive);

    return 0;
}
