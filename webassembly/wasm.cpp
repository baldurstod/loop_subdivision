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
	void destroy_buffer(uint8_t* p);
}

int result[2];

EMSCRIPTEN_KEEPALIVE
Mesh *create_mesh(uint32_t *indices, int indices_count, float *vertices, int vertices_count) {
	Mesh *mesh = new Mesh();
	int  vid = 0;
	int  fid = 0;
	//int  nid = 1;
	log_string("Creating mesh indices: " + std::to_string((unsigned int)indices_count) + " vertices: " + std::to_string((unsigned int)vertices_count));

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

	return mesh;
}

Mesh *subdivide_mesh(Mesh *oldMesh) {
	FormTrait traits(oldMesh); // add temporary variables
	Mesh *new_mesh = new Mesh();
	LOOP loop(oldMesh, new_mesh);
	loop.subdivide();

	return new_mesh;
}

EMSCRIPTEN_KEEPALIVE
uint32_t *subdivide(uint32_t * indices, int indices_count, float *vertices, int vertices_count, unsigned int count, float tolerance) {
	log_string("Subdividing " + std::to_string(count) + ", " + std::to_string(tolerance));

	Mesh *mesh = create_mesh(indices, indices_count, vertices, vertices_count);
	if (tolerance >= 0) {
		mesh->merge(tolerance);
	}

	Mesh *new_mesh = mesh;

	for (unsigned int i = 0; i < count; i++) {
		new_mesh = subdivide_mesh(new_mesh);
	}

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
			new_indices[face_index++] = he->target()->id();
			he = he->he_next();
			//TODO: check if we actually have 3 vertices
		}
	}

	uint32_t *ret = new uint32_t[4];
	ret[0] = reinterpret_cast<uint32_t>(new_indices);
	ret[1] = new_indices_count;
	ret[2] = reinterpret_cast<uint32_t>(new_vertices);
	ret[3] = new_vertices_count;


	return ret;
}

EMSCRIPTEN_KEEPALIVE
uint8_t* create_buffer(int size) {
	return new uint8_t[size];
}

EMSCRIPTEN_KEEPALIVE
void destroy_buffer(uint8_t* p) {
	delete p;
}

EMSCRIPTEN_KEEPALIVE
int get_result_pointer() {
	return result[0];
}

EMSCRIPTEN_KEEPALIVE
int get_result_size() {
	return result[1];
}

EMSCRIPTEN_KEEPALIVE
int get_detex_error_message_pointer() {
	return (int)0;//detexGetErrorMessage();
}
