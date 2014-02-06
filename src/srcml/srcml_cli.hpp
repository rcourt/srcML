/*
  srcml_cli.hpp

  Copyright (C) 2004-2014  SDML (www.srcML.org)

  This file is part of the srcML Toolkit.

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
  srcml_cli handles parsing for CLI options for srcml
*/

#ifndef SRCML_CLI_HPP
#define SRCML_CLI_HPP

#include <srcml.h>
#include <string>
#include <vector>
#include <iostream>

/* These are internal to srcml */
#define SRCML_COMMAND_LONGINFO          1<<0
#define SRCML_COMMAND_INFO              1<<1
#define SRCML_COMMAND_INFO_FILENAME     1<<2

#define SRCML_COMMAND_CPP_TEXT_IF0      1<<4
#define SRCML_COMMAND_CPP_MARKUP_ELSE   1<<5
#define SRCML_COMMAND_QUIET             1<<6
#define SRCML_COMMAND_VERBOSE           1<<7
#define SRCML_COMMAND_VERSION           1<<8

#define SRCML_COMMAND_EXPRESSION        1<<10
#define SRCML_COMMAND_INTERACTIVE       1<<11
#define SRCML_COMMAND_XML               1<<12
#define SRCML_COMMAND_LIST              1<<13
#define SRCML_COMMAND_UNITS             1<<14
#define SRCML_COMMAND_INFO_DIRECTORY    1<<15
#define SRCML_COMMAND_INFO_ENCODING     1<<16
#define SRCML_COMMAND_INFO_LANGUAGE     1<<17
#define SRCML_COMMAND_INFO_SRC_VERSION  1<<18
#define SRCML_COMMAND_TO_DIRECTORY      1<<19

struct srcml_request_t {
  int command;
  int markup_options;
  std::string filename;
  std::string output;
  std::string src_encoding;
  std::string encoding;
  std::string files_from;
  std::string language;
  std::vector<std::string> register_ext;
  int tabs;
  std::string directory;
  std::string src_versions;
  std::string prefix;
  std::string xmlns_uri;
  std::vector<std::string> xmlns_prefix;
  std::string relaxng;
  std::string xpath;
  std::vector<std::string> xpathparam;
  std::string xslt;
  int unit;
  int max_threads;
  std::vector<std::string> positional_args;
  bool help_set;
  bool directory_set;
  bool filename_set;
  bool src_versions_set;
};

srcml_request_t parseCLI (int argc, char* argv[]);

#endif