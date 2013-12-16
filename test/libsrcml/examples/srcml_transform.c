/*
  srcml_transform.c

  Copyright (C) 2013  SDML (www.srcML.org)

  The srcML Toolkit is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcML Toolkit is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcML Toolkit; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
  Example program of the use of the C API for srcML.

  Use XPath, XSLT, and RelaxNG.
*/

#include <stdio.h>
#include <string.h>

#include "srcml.h"

int main(int argc, char * argv[]) {

  struct srcml_archive * iarchive = srcml_create_archive();
  struct srcml_archive * oarchive;
  srcml_read_open_filename(iarchive, "project.xml");
  oarchive = srcml_clone_archive(iarchive);
  srcml_write_open_filename(oarchive, "transform.xml");

  srcml_append_transform_xpath(iarchive, "//src:unit");
  srcml_append_transform_xslt(iarchive, "copy.xsl");
  srcml_append_transform_relaxng(iarchive, "schema.rng");

  srcml_apply_transforms(iarchive, oarchive);

  srcml_close_archive(iarchive);
  srcml_close_archive(oarchive);

  srcml_free_archive(iarchive);
  srcml_free_archive(oarchive);

  return 0;
}