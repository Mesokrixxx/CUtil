#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define FILE_ERROR(_path, _fn)                                                 \
	fprintf(stderr, "%s: ", (_path));                                          \
	perror(_fn)

static inline bool file_load(const char *path, uint64_t *sbuf, char **buf);
static inline bool file_lastmod(const char *path, time_t *buf);

bool file_create(file_t *file, const char *path, fileflag_e flags) {
	*file = (file_t){0};
	file->flags = flags;
	if (!file_load(path, &file->size, &file->content)) {
		*file = (file_t){0};
		return false;
	}
	file->path = strdup(path);
	if (!file->path) {
		FILE_ERROR(path, "malloc");
		free(file->content);
		*file = (file_t){0};
		return false;
	}
	if (!file_lastmod(path, &file->lastmod)) {
		free(file->content);
		free(file->path);
		*file = (file_t){0};
		return false;
	}
	return true;
}

bool file_changed(file_t file) {
	time_t buf;

	if (!file_lastmod(file.path, &buf))
		return false;
	return (buf != file.lastmod);
}

bool file_reload(file_t *file) {
	char *ncontent;
	uint64_t nsize;
	time_t nlm;

	if (!file_load(file->path, &nsize, &ncontent)) {
		return false;
	}
	if (!file_lastmod(file->path, &nlm)) {
		free(ncontent);
		return false;
	}
	free(file->content);
	file->content = ncontent;
	file->size = nsize;
	file->lastmod = nlm;
	return true;
}

void file_destroy(file_t *file) {
	free(file->path);
	free(file->content);
	*file = (file_t){0};
}

static inline bool file_load(const char *path, uint64_t *sbuf, char **buf) {
	FILE *f = fopen(path, "r");
	int64_t tmp;

	if (!f) {
		FILE_ERROR(path, "fopen");
		return false;
	}
	if (fseek(f, 0, SEEK_END)) {
		FILE_ERROR(path, "fseek");
		return false;
	}
	tmp = ftell(f);
	if (tmp < 0) {
		FILE_ERROR(path, "ftell");
		return false;
	}
	*sbuf = tmp;
	if (fseek(f, 0, SEEK_SET)) {
		FILE_ERROR(path, "fseek");
		return false;
	}
	*buf = malloc(sizeof(char) * (*sbuf + 1));
	if (!*buf) {
		FILE_ERROR(path, "malloc");
		return false;
	}
	fread(*buf, sizeof(char), *sbuf, f);
	*buf[*sbuf] = 0;
	return true;
}

static inline bool file_lastmod(const char *path, time_t *buf) {
	struct stat st;

	if (stat(path, &st)) {
		FILE_ERROR(path, "stat");
		return false;
	}
	*buf = st.st_mtime;
	return true;
}
