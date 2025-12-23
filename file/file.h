#ifndef CUTIL_FILE_H
#define CUTIL_FILE_H

#include "../cutil/type.h"
#include <time.h>

typedef enum {
	FILEFLAG_NONE = 0
} fileflag_e;

typedef struct {
	char *path, *content;
	fileflag_e flags;
	uint64_t size;
	// last modification timestamp
	time_t lastmod;
} file_t;

bool file_create(file_t *file, const char *path, fileflag_e flags);
bool file_changed(file_t file);
bool file_reload(file_t *file);
void file_destroy(file_t *file);

#endif // CUTIL_FILE_H