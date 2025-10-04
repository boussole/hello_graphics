#pragma once

// https://en.wikipedia.org/wiki/Wavefront_obj_file

struct wf_vertex {
	float x, y, z;
};

struct wf_obj {
	struct wf_vertex *vertices;
	unsigned int nr_vertices;
};

#ifdef __cplusplus
extern "C" {
#endif

// init obj file
void wf_obj_init(struct wf_obj *o);

// load from obj file
int wf_obj_load(const char *filename, struct wf_obj *o);

void wf_obj_clean(struct wf_obj *o);

void wf_obj_dump(struct wf_obj *o);

#ifdef __cplusplus
}
#endif
