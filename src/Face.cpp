#include "Face.h"
#include "HalfEdge.h"
#include "Vertex.h"
#include "Point.h"

using namespace MeshLib;

bool Face::include_edge(Edge *e) {
	HalfEdge *he = m_halfedge;
	if (he->edge() == e || he->he_next()->edge() == e || he->he_prev()->edge() == e)
		return true;
	return false;
}

bool Face::include_vertex(Vertex *v) {
	HalfEdge *he = m_halfedge;
	if (he->target() == v || he->source() == v || he->he_next()->target() == v)
		return true;
	return false;
}

Point Face::normal() {
	HalfEdge *he = m_halfedge;
	Point p1 = he->target()->point() - he->source()->point();
	Point p2 = he->he_next()->target()->point() - he->target()->point();
	Point n = p1^p2;
	n /= n.norm();
	return n;
}

FaceKey::FaceKey(Vertex * v1, Vertex* v2, Vertex* v3) {
	assert((v1->id() != v2->id()) && (v2->id() != v3->id()) && (v3->id() != v1->id()));

	if (v1->id() < v2->id()) {
		if (v2->id() < v3->id()) {
			m_vertices[0] = v1;
			m_vertices[1] = v2;
			m_vertices[2] = v3;
		} else {
			if (v1->id() < v3->id()) {
				m_vertices[0] = v1;
				m_vertices[1] = v3;
				m_vertices[2] = v2;
			} else {
				m_vertices[0] = v3;
				m_vertices[1] = v1;
				m_vertices[2] = v2;
			}
		}
	}
	else {
		if (v2->id() < v3->id()) {
			if (v1->id() < v3->id()) {
				m_vertices[0] = v2;
				m_vertices[1] = v1;
				m_vertices[2] = v3;
			} else {
				m_vertices[0] = v2;
				m_vertices[1] = v3;
				m_vertices[2] = v1;
			}
		} else {
			m_vertices[0] = v3;
			m_vertices[1] = v2;
			m_vertices[2] = v1;
		}
	}
}

bool FaceKey::operator<(const FaceKey & key) const {
	if (m_vertices[0]->id() < key.m_vertices[0]->id()) return true;
	if (m_vertices[0]->id() > key.m_vertices[0]->id()) return false;

	if (m_vertices[1]->id() < key.m_vertices[1]->id()) return true;
	if (m_vertices[1]->id() > key.m_vertices[1]->id()) return false;

	if (m_vertices[2]->id() < key.m_vertices[2]->id()) return true;
	if (m_vertices[2]->id() > key.m_vertices[2]->id()) return false;

	return false;
}

bool FaceKey::operator==(const FaceKey & key) const {
	if (m_vertices[0]->id() != key.m_vertices[0]->id()) return false;
	if (m_vertices[1]->id() != key.m_vertices[1]->id()) return false;
	if (m_vertices[2]->id() != key.m_vertices[2]->id()) return false;

	return true;
}
