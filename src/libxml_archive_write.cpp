#include "libxml_archive_write.h"
#include <stdio.h>
#include <string.h>
#include <libxml/xmlIO.h>
#include <fnmatch.h>
#include <archive.h>
#include <archive_entry.h>
#include <string>

static const int NUMARCHIVES = 4;
static const char * ARCHIVE_FILTER_EXTENSIONS[] = {"tar", "zip", "tgz", "cpio", "gz", "bz2", 0};

//static char* pdata = 0;
static int size = 0;
static int pos = 0;
static struct archive *wa;
static struct archive_entry *wentry;
static char root_filename[512] = { 0 };
static char filename[512] = { 0 };

static std::string data;

// check if archive matches the protocol on the URI
int archiveWriteMatch(const char * URI) {

  //  fprintf(stderr, "MATCH: %s %s\n", URI, root_filename);

  if (URI == NULL)
      return 0;

  if (strcmp(URI, "-") == 0)
    return  0;

  if (root_filename[0])
     return 1;

  for(const char ** pos = ARCHIVE_FILTER_EXTENSIONS; *pos != 0; ++pos )
    {
      char pattern[10] = { 0 } ;
      strcpy(pattern, "*.");
      strcat(pattern, *pos);
      if(int match = fnmatch(pattern, URI, 0) == 0)
	return match;
     }

  return 0;
}

// setup archive for this URI
void* archiveWriteRootOpen(const char * URI) {
  // fprintf(stderr, "ARCHIVE_WRITE_ROOT_OPEN: %s\n", URI);
  strcpy(root_filename, URI);
}

// setup archive for this URI
void* archiveWriteOpen(const char * URI) {

  // fprintf(stderr, "ARCHIVE_WRITE_OPEN: %s\n", URI);

  if (!wa) {
    wa = archive_write_new();

    // setup the desired compression
    // TODO:  Extract into method, and make more general
    if (!fnmatch("*.gz", root_filename, 0))
      archive_write_set_compression_gzip(wa);
    else if (!fnmatch("*.bz2", root_filename, 0))
      archive_write_set_compression_bzip2(wa);

    // setup the desired format
    // TODO:  Extract into method, and make more general
#if ARCHIVE_VERSION_STAMP >= 2008000
    if (!fnmatch("*.zip", root_filename, 0) || !fnmatch("*.zip.*", root_filename, 0))
      archive_write_set_format_zip(wa);
#else
    if (false)
      ;
#endif
    else if (!fnmatch("*.cpio", root_filename, 0) || !fnmatch("*.cpio.*", root_filename, 0))
      archive_write_set_format_cpio(wa);
    else
      archive_write_set_format_ustar(wa);

    fprintf(stderr, "ROOT: %s %s %s\n", root_filename, archive_compression_name(wa),
    archive_format_name(wa));

    archive_write_open_filename(wa, root_filename);
  }
  //  pos = 0;
  strcpy(filename, URI);

  data.clear();
  // fprintf(stderr, "FILE: %s\n", URI);

  return wa;
}

// read from the URI
int archiveWrite(void * context, const char * buffer, int len) {

  // fprintf(stderr, "ARCHIVE_WRITE_WRITE: %d\n", len);
  /*
  // make sure we have room
  if (pos + len >= size) {
    size = (pos + len) * 2;
    pdata = (char*) realloc(pdata, size);
  }

  memcpy(pdata + pos, buffer, len);
  pos += len;
  */
  data.append(buffer, len);
  //  data.append(buffer, buffer + len);

  return len;
}

// close the open file
int archiveWriteClose(void * context) {

  // fprintf(stderr, "ARCHIVE_WRITE_CLOSE: %d\n", pos);

  wentry = archive_entry_new();
  archive_entry_set_pathname(wentry, filename);
  archive_entry_set_size(wentry, data.size());
  archive_entry_set_filetype(wentry, AE_IFREG);
  archive_entry_set_perm(wentry, 0644);
  archive_write_header(wa, wentry);
  archive_write_data(wa, data.c_str(), data.size());
  //  archive_write_data(wa, pdata, pos);
  archive_entry_free(wentry);
  wentry = 0;

  return 1;
}

int archiveWriteRootClose(void * context) {

  // fprintf(stderr, "ARCHIVE_WRITE_ROOT_CLOSE\n");

  if (wa) {
    // fprintf(stderr, "FINISHING\n");
    archive_write_close(wa);
    archive_write_finish(wa);
  }
  //  if (pdata)
  //    free(pdata);

  wa = 0;
  strcpy(root_filename, "");
  strcpy(filename, "");

  return 1;
}
