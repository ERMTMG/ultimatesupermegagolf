//
// Created by usuario on 1/19/25.
//

#ifndef ULTIMATESUPERMEGAGOLF_3DGEOMETRY_H
#define ULTIMATESUPERMEGAGOLF_3DGEOMETRY_H
#include "raylib.h"
#include "utility.h" //defines operators on Raylib's Vector3 class
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>

typedef unsigned short IDType;
struct Vertex;
struct Edge;
class Mesh3D;

const IDType UNINITIALIZED_ID = -1;

/*  
    Basic vertex struct: position of the vertex and ID in the mesh (ID is an unsigned int).
    Comparison and equality operators compare based on the vertices' IDs. No two vertices
    should have the same ID value, at least within the same mesh.
*/
struct Vertex{
    Vector3 pos;
    IDType id;

    Vertex();
    ~Vertex();
    Vertex(const Vector3& pos, IDType id);
    Vertex(float x, float y, float z, IDType id);
    Vertex(const Vertex& other);
    Vertex& operator=(const Vertex& rhs);
    Vertex& operator=(const Vector3& pos);

    bool operator==(const Vertex& rhs);
    bool operator<(const Vertex& rhs);
    bool operator>(const Vertex& rhs);
    bool operator<=(const Vertex& rhs);
    bool operator>=(const Vertex& rhs);
    bool operator!=(const Vertex& rhs);
};

const Vertex VERTEX_NULL (VECTOR3_NULL, UNINITIALIZED_ID);

/*
    Basic edge struct: declares a connection between two vertices (handled by those vertices' IDs). 
    Should not exist outside of a mesh.
*/
struct Edge{
    IDType vtx1;
    IDType vtx2;

    Edge();
    Edge(std::unique_ptr<Vertex> v1, std::unique_ptr<Vertex> v2);
    Edge(IDType id1, IDType id2);
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
    std::unordered_map<IDType, std::unique_ptr<Vertex>> vertices;
    std::vector<std::unique_ptr<Edge>> edges;
    IDType currentMaxID; // for fast access to the smallest free ID
   // as a consequence, there can't be exactly 65535 vertices in the mesh. Oops.

    void _copy_data(const Mesh3D& other);
    void read_vertices_implicit_ids(std::istream& in);
    void read_vertices(std::istream& in);
    void read_edges(std::istream& in);
public:
    Mesh3D();
    Mesh3D(const Mesh3D& other);
    ~Mesh3D();
    Mesh3D& operator=(const Mesh3D& rhs); // does a deep copy of all the vertices and edges

    void clear();

    void add_vertex(Vertex* vtx); // not recommended. Creating shared_ptr from a C-style pointer is a bit slower
    IDType add_vertex_at_pos(Vector3 pos); 
    IDType add_vertex_at_pos(float x, float y, float z);
    void remove_vertex(IDType id);
    unsigned short get_number_of_vertices() const;
    const IDType MAX_NUMBER_OF_VERTICES() const;
    Vector3 get_pos_of_vertex(IDType id) const;
    const Vertex& vertex(IDType id) const;
    Vertex& vertex(IDType id); //non-const version throws an exception if ID is not in mesh
                               //TODO: maybe make it add that vertex in? probably at position (0,0,0)? 
    inline const Vertex& operator[](IDType id) const{  return vertex(id);  }; // alias for vertex()
    inline Vertex& operator[](IDType id){  return vertex(id);  };
    bool is_vertex_id_taken(IDType id) const;
    IDType new_free_vertex_id() const;

    void connect_vertices(IDType id1, IDType id2);
    void disconnect_vertices(IDType id1, IDType id2);
    unsigned int get_number_of_edges() const;
    bool are_vertices_connected(IDType id1, IDType id2) const;

    void erase_null_vertices_and_edges(); // this should probably never be called in the first place but it's in here just in case

    class Iterator; class ConstIterator;

    /*
    Non-const iterator that iterates over all the vertices in the mesh.
    */
    class Iterator{
      private:
        std::unordered_map<IDType, std::unique_ptr<Vertex>>::iterator iter;
      public:
        Iterator();
        Iterator(const Iterator& other);
        Iterator& operator=(const Iterator& rhs);
        bool operator==(const Iterator& rhs) const;
        bool operator!=(const Iterator& rhs) const;
        Iterator& operator++();
        inline Iterator& operator++(int){  return this->operator++();  };
        Vertex& operator*();
        Vertex* operator->();
        IDType id() const;

        friend class Mesh3D;
        friend class Mesh3D::ConstIterator;
    };
    //using Iterator = typename Mesh3D::Iterator;
    Iterator nc_begin();
    Iterator nc_end();

    /*
    Const iterator that iterates over all the vertices in the mesh.
    */
    class ConstIterator{
      private:
        std::unordered_map<IDType, std::unique_ptr<Vertex>>::const_iterator iter;
      public:
        ConstIterator();
        ConstIterator(const ConstIterator& other);
        ConstIterator(const Iterator& other);
        ConstIterator& operator=(const ConstIterator& rhs);
        bool operator==(const ConstIterator& rhs) const;
        bool operator!=(const ConstIterator& rhs) const;
        ConstIterator& operator++();
        inline ConstIterator& operator++(int){  return this->operator++();  };
        const Vertex& operator*() const;
        const Vertex* operator->() const;
        IDType id() const;

        friend class Mesh3D;
    };
    //using ConstIterator = typename Mesh3D::ConstIterator;
    ConstIterator begin() const;
    ConstIterator end() const;

    class EdgeIterator; class ConstEdgeIterator;
    
    /*
    Non-const iterator that iterates over all the edges in the mesh.
    */
    class EdgeIterator{
      private:
        std::vector<std::unique_ptr<Edge>>::iterator iter;
      public:
        EdgeIterator();
        EdgeIterator(const EdgeIterator& other);
        EdgeIterator& operator=(const EdgeIterator& rhs);
        bool operator==(const EdgeIterator& rhs) const;
        bool operator!=(const EdgeIterator& rhs) const;
        EdgeIterator& operator++();
        inline EdgeIterator& operator++(int){  return this->operator++();  };
        Edge& operator*();
        Edge* operator->();

        friend class Mesh3D;
    };
    //using EdgeIterator = typename Mesh3D::EdgeIterator;
    EdgeIterator nc_begin_e();
    EdgeIterator nc_end_e();

    /*
    Const iterator that iterates over all the edges in the mesh.
    */
    class ConstEdgeIterator{
      private:
        std::vector<std::unique_ptr<Edge>>::const_iterator iter;
      public:
        ConstEdgeIterator();
        ConstEdgeIterator(const ConstEdgeIterator& other);
        ConstEdgeIterator(const EdgeIterator& other);
        ConstEdgeIterator& operator=(const ConstEdgeIterator& rhs);
        bool operator==(const ConstEdgeIterator& rhs) const;
        bool operator!=(const ConstEdgeIterator& rhs) const;
        ConstEdgeIterator& operator++();
        inline ConstEdgeIterator& operator++(int){  return this->operator++();  };
        const Edge& operator*() const;
        const Edge* operator->() const;

        friend class Mesh3D;
    };
    //using EdgeIterator = typename Mesh3D::EdgeIterator;
    ConstEdgeIterator begin_e() const;
    ConstEdgeIterator end_e() const;

    void print_data(std::ostream& out) const;
    void read_from_input(std::istream& in);
    void read_from_file(const std::string& filename);
    void write_to_file(const std::string& filename) const;
};


#endif
