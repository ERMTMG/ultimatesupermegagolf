#include "3dgeometry.h"

Vertex::Vertex() : pos{Vector3{0.,0.,0.}}, id{0}
{}

Vertex::Vertex(const Vector3 &pos, IDType id){
    this->pos = pos;
    this->id = id;
}

Vertex::Vertex(float x, float y, float z, IDType id){
    this->pos = {x, y, z};
    this->id = id;
}

Vertex::Vertex(const Vertex& other){
    this->pos = other.pos;
    this->id = other.id;
}

Vertex& Vertex::operator=(const Vertex& rhs){
    this->pos = rhs.pos;
    this->id = rhs.id;
    return *this;
}

inline bool Vertex::operator==(const Vertex& rhs){
    return (this->id == rhs.id);
}

inline bool Vertex::operator>(const Vertex& rhs){
    return (this->id > rhs.id);
}

inline bool Vertex::operator<(const Vertex& rhs){
    return (this->id < rhs.id);
}

inline bool Vertex::operator>=(const Vertex& rhs){
    return (this->id >= rhs.id);
}

inline bool Vertex::operator<=(const Vertex& rhs){
    return (this->id <= rhs.id);
}

inline bool Vertex::operator!=(const Vertex& rhs){
    return (this->id != rhs.id);
}

Edge::Edge(): vtx1{nullptr}, vtx2{nullptr}
{}

Edge::Edge(Vertex* vtx1, Vertex* vtx2){
    bool null = (vtx1 == nullptr || vtx2 == nullptr);
    this->vtx1 = !null ? vtx1 : nullptr;
    this->vtx2 = !null ? vtx2 : nullptr;
}

Edge::~Edge(){
    if(vtx1 != nullptr) delete vtx1;
    if(vtx2 != nullptr) delete vtx2;
}

Edge::Edge(const Edge& other){
    this->vtx1 = other.vtx1;
    this->vtx2 = other.vtx2;
}

Edge& Edge::operator=(const Edge& rhs){
    this->vtx1 = rhs.vtx1;
    this->vtx2 = rhs.vtx2;
    
    return *this;
}

bool Edge::operator==(const Edge& rhs){
    if(this->vtx1 == nullptr || this->vtx2 == nullptr){
        return (rhs.vtx1 == nullptr || rhs.vtx2 == nullptr);
    } else {
        return (rhs.vtx1 && rhs.vtx2 //both pointers must be non-null in both edge 
            && this->vtx1 == rhs.vtx1 && this->vtx2 == rhs.vtx2);
    }
}

Mesh3D::Mesh3D(){
    vertices = std::unordered_map<IDType, Vertex*>();
    edges = std::vector<Edge*>();
    currentMaxID = UNINITIALIZED_ID;
}

void Mesh3D::_copy_data(const Mesh3D& other){
    const auto& copyVertices = other.vertices;
    const auto& copyEdges = other.edges;

    for(auto itr = copyVertices.begin(); itr != copyVertices.end(); ++itr){ // copy all (non-null) vertices
        if(itr->second != nullptr){
            Vertex* currVtx = new Vertex(*(itr->second));
            this->vertices.insert({currVtx->id, currVtx});
        }
    }

    for(auto itr = copyEdges.begin(); itr != copyEdges.end(); ++itr){ // make new edges connecting the already-copied vertices
        const Edge* currEdge = *itr;
        if(currEdge != nullptr){ // just assume the vertices aren't null either
            IDType id1 = currEdge->vtx1->id; // vertices have same relative IDs in `other` and in `this`.
            IDType id2 = currEdge->vtx2->id;
            Vertex* vtx1 = this->vertex(id1);
            Vertex* vtx2 = this->vertex(id2);
            this->edges.push_back(new Edge(vtx1, vtx2));
        }
    }
    
    this->currentMaxID = other.currentMaxID;
}

void Mesh3D::clear(){
    for(auto itr = vertices.begin(); itr != vertices.end(); ++itr){
        Vertex* vtx = itr->second;
        if(vtx != nullptr) delete vtx;
    }
    vertices.clear();
    for(auto itr = edges.begin(); itr != edges.end(); ++itr){
        Edge* edge = *itr;
        if(edge != nullptr) delete edge;
    }
    edges.clear();
}

Mesh3D::Mesh3D(const Mesh3D& other){
    _copy_data(other);
}

Mesh3D::~Mesh3D(){
    clear();
}

Mesh3D& Mesh3D::operator=(const Mesh3D& rhs){
    if(this != &rhs){
        clear();
        _copy_data(rhs);
    }
    return *this;
}

Mesh3D& Mesh3D::shallow_copy(const Mesh3D& rhs){
    this->vertices = rhs.vertices;
    this->edges = rhs.edges;
    this->currentMaxID = rhs.currentMaxID;
    return *this;
}

void Mesh3D::add_vertex(Vertex* vtx){
    if(vtx != nullptr){
        IDType vtxId = vtx->id;
        if(vertices.find(vtxId) == vertices.end()){ // else there's already a vertex with that ID
            vertices.insert({vtxId, vtx});
            if(currentMaxID < vtxId || currentMaxID == UNINITIALIZED_ID){
                currentMaxID = vtxId;
            }
        }
    }
}

IDType Mesh3D::add_vertex_at_pos(Vector3 pos){
    Vertex* vtx = new Vertex(pos, currentMaxID + 1);
    currentMaxID++;
    vertices.insert({currentMaxID, vtx});
    return currentMaxID;
}

IDType Mesh3D::add_vertex_at_pos(float x, float y, float z){
    Vertex* vtx = new Vertex(x, y, z, currentMaxID + 1);
    currentMaxID++;
    vertices.insert({currentMaxID, vtx});
    return currentMaxID;
}

void Mesh3D::remove_vertex(IDType id){
    vertices.erase(id);
}

inline int Mesh3D::get_number_of_vertices() const{
    return vertices.size();
}

Vector3 Mesh3D::get_pos_of_vertex(IDType id) const{
    auto itr = vertices.find(id);
    if(itr == vertices.end()){
        return VECTOR3_NULL;
    } else if(itr->second == nullptr){
        return VECTOR3_NULL;
    } else {
        return itr->second->pos;
    }
}

const Vertex* Mesh3D::vertex(IDType id) const{
    auto itr = vertices.find(id);
    if(itr == vertices.end()){
        return nullptr;
    } else {
        return itr->second;
    }
}

Vertex* Mesh3D::vertex(IDType id){
    auto itr = vertices.find(id);
    if(itr == vertices.end()){
        return nullptr;
    } else {
        return itr->second;
    }
}

inline bool Mesh3D::is_vertex_id_taken(IDType id) const{
    return (vertices.find(id) == vertices.end());
}

inline IDType Mesh3D::new_free_vertex_id() const{
    return 1 + currentMaxID;
}

void Mesh3D::connect_vertices(IDType id1, IDType id2){
    Vertex* vtx1 = vertex(id1);
    Vertex* vtx2 = vertex(id2);
    if(vtx1 != nullptr && vtx2 != nullptr){
        Edge* edge = new Edge(vtx1, vtx2);
        edges.push_back(edge);
    }
}

void Mesh3D::add_edge(Edge* e){
    if(e != nullptr && e->vtx1 != nullptr && e->vtx2 != nullptr){
        IDType id1 = e->vtx1->id;
        IDType id2 = e->vtx2->id;
        if(vertex(id1) == e->vtx1 && vertex(id2) == e->vtx2){
            edges.push_back(e);
        }
    }
}

void Mesh3D::disconnect_vertices(IDType id1, IDType id2){
    Vertex* vtx1 = vertex(id1);
    Vertex* vtx2 = vertex(id2);
    bool foundAndErased = false;
    for(auto itr = edges.begin(); !foundAndErased && itr != edges.end(); ++itr){
        Edge* currEdge = *itr;
        if(currEdge != nullptr){
            if((currEdge->vtx1 == vtx1 && currEdge->vtx2 == vtx2) ||
               (currEdge->vtx1 == vtx2 && currEdge->vtx2 == vtx1)){ // vertices could also be switched up
                itr = edges.erase(itr);
                foundAndErased = true;
            }
        }
    }
}

inline int Mesh3D::get_number_of_edges() const{
    return edges.size();
}

bool Mesh3D::are_vertices_connected(IDType id1, IDType id2) const{
    const Vertex* vtx1 = vertex(id1);
    const Vertex* vtx2 = vertex(id2);
    bool found = false;
    for(auto itr = edges.begin(); !found && itr != edges.end(); ++itr){
        Edge* currEdge = *itr;
        if(currEdge != nullptr){
            if((currEdge->vtx1 == vtx1 && currEdge->vtx2 == vtx2) ||
               (currEdge->vtx1 == vtx2 && currEdge->vtx2 == vtx1)){ // vertices could also be switched up
                found = true;
            }
        }
    }
    return found;
}

void Mesh3D::erase_null_vertices_and_edges(){
    auto vtxItr = vertices.begin();
    while(vtxItr != vertices.end()){
        if(vtxItr->second == nullptr){
            vtxItr = vertices.erase(vtxItr);
        } else {
            ++vtxItr;
        }
    }

    auto edgItr = edges.begin();
    while(edgItr != edges.end()){
        if(*edgItr == nullptr){
            edgItr = edges.erase(edgItr);
        } else if((*edgItr)->vtx1 == nullptr || (*edgItr)->vtx2 == nullptr){
            edgItr = edges.erase(edgItr);
        } else {
            ++edgItr;
        }
    }
}

Edge::Edge(IDType id1, IDType id2, const Mesh3D& m){
    const Vertex* vtx1 = m.vertex(id1);
    const Vertex* vtx2 = m.vertex(id2);
    this->vtx1 = const_cast<Vertex*>(vtx1);
    this->vtx2 = const_cast<Vertex*>(vtx2);
}

Mesh3D::Iterator::Iterator(){}
Mesh3D::Iterator::Iterator(const Mesh3D::Iterator& other){
    this->iter = other.iter;
}
Mesh3D::Iterator& Mesh3D::Iterator::operator=(const Mesh3D::Iterator& rhs){
    this->iter = rhs.iter;
    return *this;
}
inline bool Mesh3D::Iterator::operator==(const Mesh3D::Iterator& rhs) const{
    return (this->iter == rhs.iter);
}
inline bool Mesh3D::Iterator::operator!=(const Mesh3D::Iterator& rhs) const{
    return (this->iter != rhs.iter);
}
Mesh3D::Iterator& Mesh3D::Iterator::operator++(){
    ++iter;
    return *this;
}
Vertex* Mesh3D::Iterator::operator*(){
    return iter->second;
}
IDType Mesh3D::Iterator::id() const{
    return iter->first;
}

Mesh3D::Iterator Mesh3D::nc_begin(){
    Mesh3D::Iterator output;
    output.iter = vertices.begin();
    return output;
}

Mesh3D::Iterator Mesh3D::nc_end(){
    Mesh3D::Iterator output;
    output.iter = vertices.end();
    return output;
}

Mesh3D::ConstIterator::ConstIterator(){}
Mesh3D::ConstIterator::ConstIterator(const Mesh3D::ConstIterator& other){
    this->iter = other.iter;
}
Mesh3D::ConstIterator::ConstIterator(const Mesh3D::Iterator& other){
    this->iter = other.iter;
}
Mesh3D::ConstIterator& Mesh3D::ConstIterator::operator=(const Mesh3D::ConstIterator& rhs){
    this->iter = rhs.iter;
    return *this;
}
inline bool Mesh3D::ConstIterator::operator==(const Mesh3D::ConstIterator& rhs) const{
    return (this->iter == rhs.iter);
}
inline bool Mesh3D::ConstIterator::operator!=(const Mesh3D::ConstIterator& rhs) const{
    return (this->iter != rhs.iter);
}
Mesh3D::ConstIterator& Mesh3D::ConstIterator::operator++(){
    ++iter;
    return *this;
}
const Vertex* Mesh3D::ConstIterator::operator*() const{
    return iter->second;
}
IDType Mesh3D::ConstIterator::id() const{
    return iter->first;
}