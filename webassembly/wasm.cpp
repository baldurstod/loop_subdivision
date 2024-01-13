#include "emscripten.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "../src/Mesh.h"
#include "../src/FormTrait.h"
#include "../src/LOOP.h"
#include "../src/javascript.h"

using namespace MeshLib;

extern "C" {
	uint32_t *subdivide(uint32_t *indices, int indices_count, float *vertices, int vertices_count, unsigned int count, float tolerance);
	uint8_t* create_buffer(int size);
	void delete_buffer(uint8_t *ptr);
	void cleanup();
}

uint32_t subdivide_ret[4];

EMSCRIPTEN_KEEPALIVE
std::unique_ptr<Mesh> create_mesh(uint32_t *indices, int indices_count, float *vertices, int vertices_count) {
	std::unique_ptr<Mesh> ptr = std::unique_ptr<Mesh>(new Mesh());
	Mesh *mesh = ptr.get();

	int  vid = 0;
	int  fid = 0;
	//int  nid = 1;

#ifdef LOG_TO_JAVASCRIPT
	log_string("Creating mesh indices: " + std::to_string((unsigned int)indices_count) + " vertices: " + std::to_string((unsigned int)vertices_count));
#endif

	for (int i = 0; i < vertices_count; i+=3) {
		Point p;
		for (int j = 0; j < 3; j++) {
			p[j] = vertices[i + j];
		}

		Vertex * v = mesh->create_vertex(vid++);
		v->point() = p;
	}


	for (int i = 0; i < indices_count; i+=3) {
		Vertex* v[3];
		for (int j = 0; j < 3; j++) {
			v[j] = mesh->id_vertex(indices[i + j]);
		}
		mesh->create_face(v, fid++);
	}

	mesh->refine_halfedge_structure();

	return ptr;
}

std::unique_ptr<Mesh> subdivide_mesh(std::shared_ptr<Mesh> old_mesh_ptr) {
	Mesh *old_mesh = old_mesh_ptr.get();
	FormTrait traits(old_mesh); // add temporary variables
	std::unique_ptr<Mesh> ptr = std::unique_ptr<Mesh>(new Mesh());
	Mesh *new_mesh = ptr.get();

	LOOP loop(old_mesh, new_mesh);
	loop.subdivide();

	return ptr;
}

EMSCRIPTEN_KEEPALIVE
uint32_t *subdivide(uint32_t *indices, int indices_count, float *vertices, int vertices_count, unsigned int count, float tolerance) {
#ifdef LOG_TO_JAVASCRIPT
	log_string("Subdividing " + std::to_string(count) + ", " + std::to_string(tolerance));
#endif

	std::shared_ptr<Mesh> mesh_ptr = create_mesh(indices, indices_count, vertices, vertices_count);

	Mesh *mesh = mesh_ptr.get();
	if (tolerance >= 0) {
		mesh->merge(tolerance);
	}

	for (unsigned int i = 0; i < count; i++) {
		mesh_ptr = subdivide_mesh(mesh_ptr);
	}


	Mesh *new_mesh = mesh_ptr.get();

	std::list<Vertex*> &new_vertices_list = new_mesh->vertices();
	uint32_t new_vertices_count = new_vertices_list.size() * 3;
	float *new_vertices = new float[new_vertices_count];

	int vertex_index = 0;
	for (std::list<Vertex*>::iterator viter = new_vertices_list.begin(); viter != new_vertices_list.end(); viter++) {
		Vertex *v = *viter;

		for (int i = 0; i < 3; i++) {
			new_vertices[vertex_index++] = v->point()[i];
		}
	}

	std::list<Face*> &new_indices_list = new_mesh->faces();
	uint32_t new_indices_count = new_indices_list.size() * 3;
	uint32_t *new_indices = new uint32_t[new_indices_count];

	int face_index = 0;
	for (std::list<Face*>::iterator fiter = new_indices_list.begin(); fiter != new_indices_list.end(); fiter++) {
		Face *f = *fiter;
		HalfEdge *he = f->halfedge();

		for (int i = 0; i < 3; i++) {
			new_indices[face_index++] = he->target()->id() - 1;
			he = he->he_next();
			//TODO: check if we actually have 3 vertices
		}
	}

	subdivide_ret[0] = reinterpret_cast<uint32_t>(new_indices);
	subdivide_ret[1] = new_indices_count;
	subdivide_ret[2] = reinterpret_cast<uint32_t>(new_vertices);
	subdivide_ret[3] = new_vertices_count;

	return subdivide_ret;
}

EMSCRIPTEN_KEEPALIVE
uint8_t *create_buffer(int size) {
	return new uint8_t[size];
}

EMSCRIPTEN_KEEPALIVE
void delete_buffer(uint8_t *ptr) {
	delete[] ptr;
}

EMSCRIPTEN_KEEPALIVE void cleanup() {
	float *indices = reinterpret_cast<float *>(subdivide_ret[0]);
	uint32_t *vertices = reinterpret_cast<uint32_t *>(subdivide_ret[2]);

	delete[] indices;
	delete[] vertices;

	subdivide_ret[0] = NULL;
	subdivide_ret[1] = 0;
	subdivide_ret[2] = NULL;
	subdivide_ret[3] = 0;
}
