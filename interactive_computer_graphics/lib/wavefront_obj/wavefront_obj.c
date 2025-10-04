#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "wavefront_obj.h"

// init obj file
void wf_obj_init(struct wf_obj *o)
{
	memset(o, 0, sizeof(*o));
}

// load from obj file
int wf_obj_load(const char *filename, struct wf_obj *o)
{
	int r = 0, l = 1, scan;
	char line[256];
	FILE *file;
	float a, b, c;
	void *p;

	file = fopen(filename, "r");
	if (!file) {
		r = errno;
		fprintf(stderr, "fopen('%s') fail: %s (%d)\n", filename, strerror(r), r);
		return r;
	}

	while (fgets(line, sizeof(line), file)) {
		if (strncmp(line, "v ", 2) == 0) {
			scan = sscanf(line, "v %f %f %f", &a, &b, &c);
			if (scan != 3) {
				fprintf(stderr, "wrong vertex format at line=%d\n", l);
				r = EINVAL;
				goto fail;
			}

			p = realloc(o->vertices, (o->nr_vertices + 1) * sizeof(*o->vertices));;
			if (!p) {
				fprintf(stderr, "realloc() fail\n");
				r = ENOMEM;
				goto fail;
			}

			o->vertices = p;
			o->vertices[o->nr_vertices].x = a;
			o->vertices[o->nr_vertices].y = b;
			o->vertices[o->nr_vertices++].z = c;
		}

		l++;
	}

	if (!feof(file) && ferror(file)) {
		fprintf(stderr, "fgets() fail\n");
		r = ferror(file);
		goto fail;
	}

	fclose(file);
	return 0;

fail:
	wf_obj_clean(o);
	fclose(file);
	return r;
}

void wf_obj_dump(struct wf_obj *o)
{
	for (unsigned int i = 0; i < o->nr_vertices; i++) {
		printf("v %f %f %f\n", o->vertices[i].x, o->vertices[i].y, o->vertices[i].z);
	}
}

void wf_obj_clean(struct wf_obj *o)
{
	if (o->vertices)
		free(o->vertices);

	memset(o, 0, sizeof(*o));
}
