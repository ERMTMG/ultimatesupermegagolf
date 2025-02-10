#include "3dgeometry.h"
#include <fstream>

using std::unique_ptr;
using std::make_unique;
using std::make_pair;
using std::ws;

Vertex::Vertex() : pos{Vector3{0.,0.,0.}}, id{0}
{}

Vertex::~Vertex(){};

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

Vertex& Vertex::operator=(const Vector3& pos){
    this->pos = pos;
    return *this;
}

bool Vertex::operator==(const Vertex& rhs){
    return (this->id == rhs.id);
}

bool Vertex::operator>(const Vertex& rhs){
    return (this->id > rhs.id);
}

bool Vertex::operator<(const Vertex& rhs){
    return (this->id < rhs.id);
}

bool Vertex::operator>=(const Vertex& rhs){
    return (this->id >= rhs.id);
}

bool Vertex::operator<=(const Vertex& rhs){
    return (this->id <= rhs.id);
}

bool Vertex::operator!=(const Vertex& rhs){
    return (this->id != rhs.id);
}

Edge::Edge(): vtx1{UNINITIALIZED_ID}, vtx2{UNINITIALIZED_ID}
{}

Edge::Edge(std::unique_ptr<Vertex> vtx1, std::unique_ptr<Vertex> vtx2){
    bool null = (vtx1 == nullptr || vtx2 == nullptr);
    this->vtx1 = !null ? vtx1->id : UNINITIALIZED_ID;
    this->vtx2 = !null ? vtx2->id : UNINITIALIZED_ID;
}

Edge::Edge(IDType id1, IDType id2): vtx1{id1}, vtx2{id2}
{}

Edge::~Edge(){}

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
    return (this->vtx1 == rhs.vtx1 && this->vtx2 == rhs.vtx2);
}

Mesh3D::Mesh3D(){
    vertices = std::unordered_map<IDType, unique_ptr<Vertex>>();
    edges = std::vector<unique_ptr<Edge>>();
    currentMaxID = UNINITIALIZED_ID;
}

void Mesh3D::_copy_data(const Mesh3D& other){
    const auto& copyVertices = other.vertices;
    const auto& copyEdges = other.edges;

    for(auto itr = copyVertices.begin(); itr != copyVertices.end(); ++itr){ // copy all (non-null) vertices
        if(itr->second != nullptr){
            IDType vtxId = itr->second->id;
            this->vertices.emplace(vtxId, make_unique<Vertex>(*(itr->second)));
        }
    }

    for(auto itr = copyEdges.begin(); itr != copyEdges.end(); ++itr){ // make new edges connecting the already-copied vertices
        const unique_ptr<Edge>& currEdge = *itr;
        this->edges.push_back(make_unique<Edge>(currEdge->vtx1, currEdge->vtx2));
    }
    
    this->currentMaxID = other.currentMaxID;
}

void Mesh3D::clear(){
    for(auto itr = vertices.begin(); itr != vertices.end(); ++itr){
        itr->second.reset();
    }
    vertices.clear();
    for(auto itr = edges.begin(); itr != edges.end(); ++itr){
        itr->reset();
    }
    edges.clear();
    currentMaxID = UNINITIALIZED_ID;
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

/*
Mesh3D& Mesh3D::shallow_copy(const Mesh3D& rhs){
    this->vertices = rhs.vertices;
    this->edges = rhs.edges;
    this->currentMaxID = rhs.currentMaxID;
    return *this;
}*/

void Mesh3D::add_vertex(Vertex* vtx){
    if(vtx != nullptr){
        IDType vtxId = vtx->id;
        if(vertices.find(vtxId) == vertices.end()){ // else there's already a vertex with that ID
            unique_ptr<Vertex> smartPtr(vtx);
            vertices.emplace(vtxId, std::move(smartPtr));
            if(currentMaxID < vtxId || currentMaxID == UNINITIALIZED_ID){
                currentMaxID = vtxId;
            }
        }
    }
}

IDType Mesh3D::add_vertex_at_pos(Vector3 pos){
    IDType vtxId = currentMaxID + 1;
    currentMaxID++;
    unique_ptr<Vertex> vtx = make_unique<Vertex>(pos, vtxId);
    vertices.emplace(vtxId, std::move(vtx));
    return vtxId;
}

IDType Mesh3D::add_vertex_at_pos(float x, float y, float z){
    IDType vtxId = currentMaxID + 1;
    currentMaxID++;
    unique_ptr<Vertex> vtx = make_unique<Vertex>(x,y,z, vtxId);
    vertices.emplace(vtxId, std::move(vtx));
    return vtxId;
}

void Mesh3D::remove_vertex(IDType id){
    vertices.erase(id);
}

unsigned short Mesh3D::get_number_of_vertices() const{
    return vertices.size();
}

const IDType Mesh3D::MAX_NUMBER_OF_VERTICES() const{
    return UNINITIALIZED_ID; //There can be at most 65535 vertices in a single mesh (which is equal to (unsigned short)(-1) - 1)
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

const Vertex& Mesh3D::vertex(IDType id) const{
    auto itr = vertices.find(id);
    if(itr == vertices.end()){
        return VERTEX_NULL;
    } else {
        return *(itr->second);
    }
}

Vertex& Mesh3D::vertex(IDType id){
    auto itr = vertices.find(id);
    if(itr == vertices.end()){ //vertex isn't in mesh, create it
        unique_ptr<Vertex> newVtx = make_unique<Vertex>(VECTOR3_NULL, id);
        vertices.emplace(id, std::move(newVtx));
        if(id > currentMaxID || currentMaxID == UNINITIALIZED_ID){
            currentMaxID = id;
        }
        return vertex(id); //calls method again after having created the vertex, now should jump to other branch
    } else {
        return *(itr->second);
    }
}

bool Mesh3D::is_vertex_id_taken(IDType id) const{
    return (vertices.find(id) != vertices.end());
}

IDType Mesh3D::new_free_vertex_id() const{
    return 1 + currentMaxID;
}

void Mesh3D::connect_vertices(IDType id1, IDType id2){
    if(is_vertex_id_taken(id1) && is_vertex_id_taken(id2)){
        unique_ptr<Edge> edge = make_unique<Edge>(id1, id2);
        edges.push_back(std::move(edge));
    }
}

void Mesh3D::disconnect_vertices(IDType id1, IDType id2){
    if(is_vertex_id_taken(id1) && is_vertex_id_taken(id2)){
        bool foundAndErased = false;
        for(auto itr = edges.begin(); !foundAndErased && itr != edges.end(); ++itr){
            unique_ptr<Edge>& currEdge = *itr;
            if(currEdge != nullptr){
                if(*currEdge == Edge(id1, id2) || *currEdge == Edge(id2, id1)){ // vertices could also be switched up
                    itr = edges.erase(itr);
                    foundAndErased = true;
                }
            }
        }
    }
}

unsigned int Mesh3D::get_number_of_edges() const{
    return edges.size();
}

bool Mesh3D::are_vertices_connected(IDType id1, IDType id2) const{
    bool found = false;
    for(auto itr = edges.begin(); !found && itr != edges.end(); ++itr){
        const unique_ptr<Edge>& currEdge = *itr;
        if(currEdge != nullptr){
            if(*currEdge == Edge(id1, id2) || *currEdge == Edge(id2, id1)){ // vertices could also be switched up
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
        } else if((*edgItr)->vtx1 == UNINITIALIZED_ID || (*edgItr)->vtx2 == UNINITIALIZED_ID){
            edgItr = edges.erase(edgItr);
        } else {
            ++edgItr;
        }
    }
}

Mesh3D::Iterator::Iterator(){}
Mesh3D::Iterator::Iterator(const Mesh3D::Iterator& other){
    this->iter = other.iter;
}
Mesh3D::Iterator& Mesh3D::Iterator::operator=(const Mesh3D::Iterator& rhs){
    this->iter = rhs.iter;
    return *this;
}
bool Mesh3D::Iterator::operator==(const Mesh3D::Iterator& rhs) const{
    return (this->iter == rhs.iter);
}
bool Mesh3D::Iterator::operator!=(const Mesh3D::Iterator& rhs) const{
    return (this->iter != rhs.iter);
}
Mesh3D::Iterator& Mesh3D::Iterator::operator++(){
    ++iter;
    return *this;
}
Vertex& Mesh3D::Iterator::operator*(){
    return *(iter->second);
}
Vertex* Mesh3D::Iterator::operator->(){
    return iter->second.get();
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
bool Mesh3D::ConstIterator::operator==(const Mesh3D::ConstIterator& rhs) const{
    return (this->iter == rhs.iter);
}
bool Mesh3D::ConstIterator::operator!=(const Mesh3D::ConstIterator& rhs) const{
    return (this->iter != rhs.iter);
}
Mesh3D::ConstIterator& Mesh3D::ConstIterator::operator++(){
    ++iter;
    return *this;
}
const Vertex& Mesh3D::ConstIterator::operator*() const{
    return *(iter->second);
}
const Vertex* Mesh3D::ConstIterator::operator->() const{
    return iter->second.get();
}
IDType Mesh3D::ConstIterator::id() const{
    return iter->first;
}

Mesh3D::ConstIterator Mesh3D::begin() const{
    Mesh3D::ConstIterator output;
    output.iter = vertices.begin();
    return output;
}

Mesh3D::ConstIterator Mesh3D::end() const{
    Mesh3D::ConstIterator output;
    output.iter = vertices.end();
    return output;
}

Mesh3D::EdgeIterator::EdgeIterator(){}
Mesh3D::EdgeIterator::EdgeIterator(const Mesh3D::EdgeIterator& other){
    this->iter = other.iter;
}
Mesh3D::EdgeIterator& Mesh3D::EdgeIterator::operator=(const Mesh3D::EdgeIterator& rhs){
    this->iter = rhs.iter;
    return *this;
}
bool Mesh3D::EdgeIterator::operator==(const Mesh3D::EdgeIterator& rhs) const{
    return (this->iter == rhs.iter);
}
bool Mesh3D::EdgeIterator::operator!=(const Mesh3D::EdgeIterator& rhs) const{
    return (this->iter != rhs.iter);
}
Mesh3D::EdgeIterator& Mesh3D::EdgeIterator::operator++(){
    ++iter;
    return *this;
}
Edge& Mesh3D::EdgeIterator::operator*(){
    return *(*iter);
}
Edge* Mesh3D::EdgeIterator::operator->(){
    return iter->get();
}

Mesh3D::EdgeIterator Mesh3D::nc_begin_e(){
    Mesh3D::EdgeIterator output;
    output.iter = edges.begin();
    return output;
}

Mesh3D::EdgeIterator Mesh3D::nc_end_e(){
    Mesh3D::EdgeIterator output;
    output.iter = edges.end();
    return output;
}

Mesh3D::ConstEdgeIterator::ConstEdgeIterator(){}
Mesh3D::ConstEdgeIterator::ConstEdgeIterator(const Mesh3D::ConstEdgeIterator& other){
    this->iter = other.iter;
}
Mesh3D::ConstEdgeIterator::ConstEdgeIterator(const Mesh3D::EdgeIterator& other){
    this->iter = other.iter;
}
Mesh3D::ConstEdgeIterator& Mesh3D::ConstEdgeIterator::operator=(const Mesh3D::ConstEdgeIterator& rhs){
    this->iter = rhs.iter;
    return *this;
}
bool Mesh3D::ConstEdgeIterator::operator==(const Mesh3D::ConstEdgeIterator& rhs) const{
    return (this->iter == rhs.iter);
}
bool Mesh3D::ConstEdgeIterator::operator!=(const Mesh3D::ConstEdgeIterator& rhs) const{
    return (this->iter != rhs.iter);
}
Mesh3D::ConstEdgeIterator& Mesh3D::ConstEdgeIterator::operator++(){
    ++iter;
    return *this;
}
const Edge& Mesh3D::ConstEdgeIterator::operator*() const{
    return *(*iter);
}
const Edge* Mesh3D::ConstEdgeIterator::operator->() const{
    return iter->get();
}

Mesh3D::ConstEdgeIterator Mesh3D::begin_e() const{
    Mesh3D::ConstEdgeIterator output;
    output.iter = edges.begin();
    return output;
}

Mesh3D::ConstEdgeIterator Mesh3D::end_e() const{
    Mesh3D::ConstEdgeIterator output;
    output.iter = edges.end();
    return output;
}

void Mesh3D::print_data(std::ostream& out) const{
    out << "VERTICES: (" << vertices.size() << ")\n";
    for(auto iter = begin(); iter != end(); ++iter){
        const Vertex& vtx = *iter; 
        Vector3 pos = vtx.pos;
        out << "\t(" << pos.x << ',' << pos.y << ',' << pos.z << "), id: " << vtx.id << '\n';
    }
    out << "EDGES: (" << edges.size() << ")\n";
    unsigned int counter = 0;
    for(auto iter = begin_e(); iter != end_e(); ++iter){
        const Edge& edge = *iter;
        out << "\tEdge " << counter << " joining vertices " << edge.vtx1 << " and " << edge.vtx2 << '\n';
        counter++;
    }
}

void Mesh3D::read_from_input(std::istream& in){
    std::string currentInput;
    char tellerChar;
    in >> ws >> currentInput;
    if(currentInput == "@VERTICES"){
        in >> ws;
        tellerChar = in.peek();
        if(std::isdigit(tellerChar) || tellerChar == '-'){ // No @NOIDS tag, assume "ID X Y Z"-format
            clear();
            read_vertices(in);
        } else {
            in >> currentInput;
            if(currentInput == "@NOIDS"){
                clear();
                read_vertices_implicit_ids(in);
            } else {
                throw std::invalid_argument("Expected either @NOIDS tag or vertex data after @VERTICES, found" + currentInput);
            }
        }
        //Finished reading vertices, next thing in istream should be @EDGES
        in >> currentInput;
        if(currentInput == "@EDGES"){
            read_edges(in); 
        } else {
            throw std::invalid_argument("Expected @EDGES tag after vertex data, found " + currentInput);
        }
    } else {
        throw std::invalid_argument("Input to Mesh3D must begin with @VERTICES, found " + currentInput);
    }
}

void Mesh3D::read_vertices_implicit_ids(std::istream& in){
    //assumes data in istream is triplets of floats, followed by tag @EDGES
    float x, y, z;
    in >> ws;
    char tellerChar = in.peek();
    while(std::isdigit(tellerChar) || tellerChar == '-'){
        in >> x >> y >> z;
        add_vertex_at_pos(x, y, z);
        in >> ws;
        tellerChar = in.peek();
    }
}

void Mesh3D::read_vertices(std::istream& in){
    float x, y, z;
    IDType id;
    in >> ws;
    char tellerChar = in.peek();
    while(std::isdigit(tellerChar) || tellerChar == '-'){
        in >> x >> y >> z >> id;
        add_vertex(new Vertex(x, y, z, id));
        in >> ws;
        tellerChar = in.peek();
    }
}

void Mesh3D::read_edges(std::istream& in){
    IDType id1, id2;
    in >> ws;
    char tellerChar = in.peek();
    while(std::isdigit(tellerChar)){
        in >> id1 >> id2;
        connect_vertices(id1, id2);
        in >> ws;
        tellerChar = in.peek();
    }
}

void Mesh3D::read_from_file(const std::string& filename){
    if(filename.length() < 8 || filename.substr(filename.length() - 8) != ".usmg3dm"){
        std::cerr << filename.substr(filename.length() - 8) << std::endl;
        std::cerr << "Warning: File " << filename << " does not have .usmg3dm extension. It is recommended to use that extension for mesh data" << std::endl;
    }
    std::ifstream file(filename);
    if(!file){
        throw std::runtime_error("File " + filename + " does not exist or could not be opened");
    }
    read_from_input(file);
    file.close();
}

void Mesh3D::write_to_file(const std::string& filename) const{
    if(filename.length() < 8 || filename.substr(filename.length() - 8) != ".usmg3dm"){
        std::cerr << filename.substr(filename.length() - 8) << std::endl;
        std::cerr << "Warning: File " << filename << " does not have .usmg3dm extension. It is recommended to use that extension for mesh data" << std::endl;
    }
    std::ofstream file(filename);
    if(!file){
        throw std::runtime_error("File " + filename + " could not be opened");
    }
    file << "@VERTICES\n";
    for(auto itr = begin(); itr != end(); ++itr){
        file << (*itr).pos.x << '\t' << (*itr).pos.y << '\t' << (*itr).pos.z << '\t' << itr.id() << '\n';
    }
    file << "@EDGES\n";
    for(auto itr = begin_e(); itr != end_e(); ++itr){
        file << (*itr).vtx1 << '\t' << (*itr).vtx2 << '\n';
    }
    file << "@END" << std::flush;
    file.close();
}