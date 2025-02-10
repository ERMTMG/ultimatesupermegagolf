//
// Created by usuario on 1/19/25.
//

#ifndef ULTIMATESUPERMEGAGOLF_3DGEOMETRY_H
#define ULTIMATESUPERMEGAGOLF_3DGEOMETRY_H
#include "raylib.h"
#include "vector3_utilities.h" //defines operators on Raylib's Vector3 class
#include <unordered_map>
#include <vector>
#include <iostream>

typedef unsigned short IDType;
struct Vertex;
struct Edge;
class Mesh3D;

/*  
    Basic vertex struct: position of the vertex and ID in the mesh (ID is an unsigned int).
    Comparison and equality operators compare based on the vertices' IDs. No two vertices
    should have the same ID value, at least within the same mesh.
*/
struct Vertex{
    Vector3 pos;
    IDType id;

    Vertex();
    Vertex(const Vector3& pos, IDType id);
    Vertex(float x, float y, float z, IDType id);
    Vertex(const Vertex& other);
    Vertex& operator=(const Vertex& rhs);

    inline bool operator==(const Vertex& rhs);
    inline bool operator<(const Vertex& rhs);
    inline bool operator>(const Vertex& rhs);
    inline bool operator<=(const Vertex& rhs);
    inline bool operator>=(const Vertex& rhs);
    inline bool operator!=(const Vertex& rhs);
};

/*
    Basic edge struct: declares a connection between two vertices (handled by pointers to 
    dynamically-allocated vertices). Constructor by IDs needs a reference to the mesh from
    which the IDs should be considered.
*/
struct Edge{
    Vertex* vtx1;
    Vertex* vtx2;

    Edge();
    Edge(Vertex* v1, Vertex* v2);
    Edge(IDType id1, IDType id2, const Mesh3D& m);
    ~Edge();
    Edge(const Edge& other);
    Edge& operator=(const Edge& rhs);
    bool operator==(const Edge& rhs);
};

/*
    Class that holds a set of vertices and edges joining those vertices. Uses pointers to 
    dynamically-allocated vertices and edges, so the destructor calls `delete` on all the elements
    of the vertex and edge containers. Do not use with pointers to Vertex/Edge variables in the heap. Do not.
*/
class Mesh3D{
private:
    std::unordered_map<IDType, Vertex*> vertices;
    std::vector<Edge*> edges;
    IDType currentMaxID; // for fast access to the smallest free ID
    static const IDType UNINITIALIZED_ID = -1; // as a consequence, there can't be exactly 65535 vertices in the mesh. Oops.

    void _copy_data(const Mesh3D& other);
public:
    Mesh3D();
    Mesh3D(const Mesh3D& other);
    ~Mesh3D();
    Mesh3D& operator=(const Mesh3D& rhs); // does a deep copy of all the vertices and edges

    Mesh3D& shallow_copy(const Mesh3D& rhs);
    void clear();

    void add_vertex(Vertex* vtx); // for stuff like mesh.add_vertex(new Vertex(x, y, z, mesh.new_free_vertex_id()));
    IDType add_vertex_at_pos(Vector3 pos);
    IDType add_vertex_at_pos(float x, float y, float z);
    void remove_vertex(IDType id);
    inline int get_number_of_vertices() const;
    Vector3 get_pos_of_vertex(IDType id) const;
    const Vertex* vertex(IDType id) const;
    Vertex* vertex(IDType id);
    inline const Vertex* operator[](IDType id) const{  return vertex(id);  }; // alias for vertex()
    inline Vertex* operator[](IDType id){  return vertex(id);  };
    inline bool is_vertex_id_taken(IDType id) const;
    inline IDType new_free_vertex_id() const;

    void connect_vertices(IDType id1, IDType id2);
    void add_edge(Edge* e);
    void disconnect_vertices(IDType id1, IDType id2);
    inline int get_number_of_edges() const;
    bool are_vertices_connected(IDType id1, IDType id2) const;

    void erase_null_vertices_and_edges(); // this should probably never be called in the first place but it's in here just in case

    class Iterator; class ConstIterator;
    /*
    Non-const iterator that iterates over all the vertices in the mesh.
    */
    class Iterator{
      private:
        std::unordered_map<IDType, Vertex*>::iterator iter;
      public:
        Iterator();
        Iterator(const Iterator& other);
        Iterator& operator=(const Iterator& rhs);
        inline bool operator==(const Iterator& rhs) const;
        inline bool operator!=(const Iterator& rhs) const;
        Iterator& operator++();
        inline Iterator& operator++(int){  return this->operator++();  };
        Vertex* operator*();
        IDType id() const;

        friend class Mesh3D;
        friend class Mesh3D::ConstIterator;
    };
    using Iterator = typename Mesh3D::Iterator;
    Iterator nc_begin();
    Iterator nc_end();

    class ConstIterator{
      private:
        std::unordered_map<IDType, Vertex*>::const_iterator iter;
      public:
        ConstIterator();
        ConstIterator(const ConstIterator& other);
        ConstIterator(const Iterator& other);
        ConstIterator& operator=(const ConstIterator& rhs);
        inline bool operator==(const ConstIterator& rhs) const;
        inline bool operator!=(const ConstIterator& rhs) const;
        ConstIterator& operator++();
        inline ConstIterator& operator++(int){  return this->operator++();  };
        const Vertex* operator*() const;
        IDType id() const;

        friend class Mesh3D;
    };
    using ConstIterator = typename Mesh3D::ConstIterator;
    ConstIterator begin();
    ConstIterator end();
};

#endif
