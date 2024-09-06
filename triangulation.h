#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <dynamic_graph/dynamic_connectivity.cpp>
#include <dynamic_graph/dynamic_forest.cpp>
#include <dynamic_graph/sequence.cpp>
#include <dynamic_graph/graph.cpp>
#include <dynamic_graph/hash.cpp>



class Triangulation {
private:
std::vector<Simplex*> simplices_;
std::vector<std::vector<TreeNode*> > nodes_;
std::vector<std::vector<uint32_t> > flag_;
std::vector<uint32_t> currentflag_;

std::vector<TreeNode*> graph_nodes_; // should have constant size during simulation
	
DynamicConnectivity mid_graph_;   // should have constant size during simulation

std::vector<uint32_t> inactive_graph_nodes_; // stores the currently unused node labels

std::vector<TreeNode*> coord_2_edges_; // stores the vertices with coordination number 2
std::vector<TreeNode*> coord_2_nodes_; // stores the vertices with coordination number 2
std::vector<TreeNode*> coord_4_nodes_; // stores the vertices with coordination number 4 (not used)
std::vector<TreeNode*> coord_3_nodes_; // stores the links with coordination number 2

int next_inactive_graph_ind_; // index of the next free label in inactive_graph_nodes
int middle_graph_size_; // number of edges in the middle graph
int num_connected_; // number of connected components

Handle root_;
bool allowSelfAdjacency_;
bool allow_degeneracy_;
bool spanning_tree_;
bool dual_spanning_tree_;

bool middle_graph_;

public:
Triangulation() : nodes_(4), flag_(4), currentflag_(4,0), allowSelfAdjacency_(true), allow_degeneracy_(true), spanning_tree_(false), dual_spanning_tree_(false), middle_graph_(false),next_inactive_graph_ind_(0), mid_graph_(ABSMAX_DC),graph_nodes_(ABSMAX_DC),inactive_graph_nodes_(ABSMAX_DC),middle_graph_size_(0),num_connected_(0), coord_2_nodes_(0),coord_2_edges_(0),coord_3_nodes_(0) {}

~Triangulation() { for( auto s : simplices_ ) if( s ) delete s; }



void clear();

Simplex * getSImplex(int index) { return simplices_[index]; }




void DecrementCoord(TreeNode * node, int dim);
void IncrementCoord(TreeNode * node, int dim);
void RemoveEdgeCoord3(TreeNode * node);
void AddEdgeCoord3(TreeNode * node);
void RemoveVertexCoord2(TreeNode * node);
void AddVertexCoord2(TreeNode * node);
void RemoveEdgeCoord2(TreeNode * node);
void AddEdgeCoord2(TreeNode * node);
int GetCoord3Size();
int GetCoord2Size();
int GetEdgeCoord2Size();


Handle getRoot() { return root_; }

int getMiddleGraphNumVertices() { return mid_graph_.GetNumOfVertices(); };

void useMiddleGraph(bool on=true) { middle_graph_ = on; }

void useSpanningTree(bool on=true) { spanning_tree_ = on; }
void useDualSpanningTree(bool on=true) { dual_spanning_tree_ = on; }
void allowSelfAdjacency(bool allow=true) { allowSelfAdjacency_ = allow; }
void allowDegeneracy(bool allow=true) { allow_degeneracy_ = allow; }
void setRoot(Handle root) { root_ = root; }
void setNextInactiveGraphInd(int ind) { next_inactive_graph_ind_ = ind; }	// the index of the next free graph Id
void setNextInactiveGraphId(int id) { inactive_graph_nodes_[next_inactive_graph_ind_] = id; }	// in case of delete, put the now inactive node ID to the next free place

bool hasMiddleGraph() { return middle_graph_; }
bool hasSpanningTree() { return spanning_tree_; }
bool hasDualSpanningTree() { return dual_spanning_tree_; }
bool allowsSelfAdjacency() { return allowSelfAdjacency_; }
bool allowsDegeneracy() { return allow_degeneracy_; }

int getMiddleGraphNumConnected() { return num_connected_; }
void incrementNumConnected() { num_connected_++; }
void decrementNumConnected() { num_connected_--; }

int numSimplices() { return simplices_.size(); }
int numFaces() { return nodes_[2].size(); }
int numEdges() { return nodes_[1].size(); }
int numVertices() { return nodes_[0].size(); }
int numSubSimplies(int dim) { return nodes_[dim].size(); } // could be useful, more general then the above 4

int getNextInactiveGraphInd() {	return next_inactive_graph_ind_; } // get the index of the next free graph Id
int getNextInactiveGraphId() { return inactive_graph_nodes_[next_inactive_graph_ind_]; }	// get the value of the next free graph Id

void printHandleMiddleGraph(Handle h) {
	int fna = h.getFaceNode()->getGraphId();
	int enb = h.getEdgeNode()->getGraphId();
	
	printf("hasedge?: %d\tfaceid: %d %d\tedgeid: %d %d\n",getHandleMiddleGraphHasEdge(h),fna,h.getFaceNode()->getInMiddleGraph(),enb,h.getEdgeNode()->getInMiddleGraph());
}

void PrintGraphIndBef() { printf("nextgid_before:%d\n",getNextInactiveGraphInd()); }
void PrintGraphIndAf() { printf("nextgid_after: %d\n",getNextInactiveGraphInd()); }

template<typename RNG> Handle getRandomEdge(RNG & rng) { return getSimplex( uniform_int(rng, numSimplices()) )->getEdge( uniform_int(rng,4), uniform_int(rng,3) ); }
template<typename RNG> Handle getRandomSimplex(RNG & rng) { return getSimplex( uniform_int(rng, numSimplices()) ); }
template<typename RNG> Handle getRandomUniformEdge(RNG & rng) { return getNode(1,uniform_int(rng,numEdges()))->getHandle(); }
template<typename RNG> Handle getRandomUniformVertex(RNG & rng) { return getNode(0,uniform_int(rng,numVertices()))->getHandle(); }

std::pair<Handle, int> Identify_move02(Handle handle,double dual_tree_rng, int tree_rng, double move_probability); // unused
std::pair<Handle, int> Identify_move20(Handle origin, double move_probability, double fail_probability); // unused
std::pair<Handle, int> Identify_move23(Handle interface, double dual_tree_rng, double tree_rng); // unused not yet fully implemented

Handle Rotate_Interface(Handle handle);

void clearDualSpanningTree();
void clearSpanningTree();

void unsetGraphEdgeLinks(Handle h); // removes middlegraph connections related to the link and all surrounding triangles
void unsetGraphFaceLinks(Handle h); // removes middlegraph connections related to the triangle and all three surrounding links

void clearGraphFaceLinksBeforeMove20(Handle origin); // removes a set of elements from the middle graph 
void clearGraphFaceLinksBeforeMove23(Handle interface); // removes a set of elements from the middle graph
void clearGraphFaceLinksBeforeMove32(Handle diagonal); // removes a set of elements from the middle graph

bool setGraphFaceLinks(Handle h); // adds the triangle to the middle graph
bool setGraphEdgeLinks(Handle h); // adds the link to the middle graph

bool unsetGraphFaceLinksAfterMove23(Handle diagonal);
bool setGraphFaceLinksAfterMove23(Handle diagonal);
bool unsetGraphFaceLinksAfterMove32(Handle interface);
bool setGraphFaceLinksAfterMove32(Handle interface);
bool unsetGraphFaceLinksAfterMove20(Handle handle);
bool setGraphFaceLinksAfterMove20(Handle handle);
bool unsetGraphFaceLinksAfterMove02(Handle origin);
bool setGraphFaceLinksAfterMove02(Handle origin);

template<typename RNG> bool TryGraphMove02(RNG & rng);
template<typename RNG> bool TryGraphMove20(RNG & rng);
template<typename RNG> bool TryGraphMove23(RNG & rng);
template<typename RNG> bool TryGraphMove32(RNG & rng);
template<typename RNG> bool TryGraphMoveEdge20(RNG & rng);
template<typename RNG> bool TryGraphMoveEdge02(RNG & rng);
template<typename RNG> bool TryGraphDualTreeMove(RNG & rng);
template<typename RNG> bool TryGraphTreeMove(RNG & rng);
template<typename RNG> std::pair<bool,std::pair<Handle,Handle> > updateGraphDualSpanningTree(RNG & rng);
template<typename RNG> std::pair<Handle,Handle> randomGraphSpanningTreeMove(RNG & rng);
template<typename RNG> Handle GetRandomCoord3Edge(RNG & rng);
template<typename RNG> Handle GetRandomCoord3(RNG & rng);
template<typename RNG> Handle GetRandomCoord2(RNG & rng);
template<typename RNG> Handle GetRandomEdgeCoord2(RNG & rng);

Handle diagonalSubdivide(Handle interface);
Handle unDiagonalSubdivide(Handle diagonal);

Handle expandTriangle(Handle handle, double dual_tree_rooting, double tree_rooting);
Handle unExpandTriangle(Handle origin);

bool loadFromStream(std::istream & stream);
void saveToStream(std::ostream & stream);

void initializeMinimal();
void initializeFaceNodes();
void initializeEdgeNodes();
void initializeVertexNodes();
void InitInactiveGraphNodes();
void initializeMiddleGraph();
void initializeCoordinationNumbers();
void setInitCoords();


double getActionNow();
double getAction(int n0,int n3, int nl, int nc);
void SetAction();

//template<typename RNG> bool getProbability(RNG & rng, int movenum);

bool getProbabilityNow(double moveprob, double SA, double SB, double prob); // this is currently used

void deleteNode(TreeNode * node);

void setFlag(TreeNode * n);
void resetFlag(int dimension);
bool isFlagged(TreeNode * n);

TreeNode * getNode(int dimension, int i);
Simplex * getSimplex(int i);

TreeNode * newNode(int dim);
Simplex * newSimplex();

void deleteSimplex(Simplex * simplex);

void AddNodeMiddleGraph(TreeNode * node);
void DelNodeMiddleGraph(TreeNode * node);
void DeleteEdgeMiddleGraph(Handle h);
void AddEdgeMiddleGraph(Handle h);
void unsetHandleInMiddleGraph(Handle h);
void setHandleInMiddleGraph(Handle h);
void PrintMiddleGraph();
void DeleteEdgeMiddleGraphPure(Handle h);
void AddEdgeMiddleGraphPure(Handle h);

bool getMiddleGraphHasEdge(int u, int v);
bool getMiddleGraphIsConnected(int u, int v);
bool getHandleMiddleGraphHasEdge(Handle h);
bool getHandleMiddleGraphIsConnected(Handle h);

int getMaxVertexCoord();
int getMaxEdgeCoord();
int getMaxGraphCoord();
int getMiddleGraphEdgeSize();
int getMiddleGraphNodeSize();

void CheckRotationAroundEdges();
bool spanningTreeAgrees();
bool isDualSpanningTree();
bool dualSpanningTreeAgrees();
bool isSpanningTree();
bool checkNodes();
bool isManifold();
bool CheckEdgeCoord() ;
bool NodeCheck();
bool CheckSingeEdgeCoord(Handle n);
int checkIsInAnyTree();

void doAllChecks() {
	spanningTreeAgrees();
	isDualSpanningTree();
	dualSpanningTreeAgrees();
	isSpanningTree();
	checkNodes();
	isManifold();
	CheckRotationAroundEdges();
	CheckEdgeCoord() ;
	checkIsInAnyTree();
	NodeCheck();
}

std::pair<bool,Handle> isFreeFace(Handle edge,std::vector<bool> &edge_present,std::vector<bool> &face_present);

std::vector<int> getNodeCoordVector(int dim);
std::vector<int> getGraphCoordVector();



void printAdjacency(char * outfilename);
void printAdjacency_color(char * outfilename);
void printAdjMatrix(int sweepcounter);
void printHistData();
void PrintHistogram(std::vector<int> hist,char* filename, FILE * file);
void printSimplexData();


//std::pair<Handle,Handle> unexpandRectangle(Handle handle1);
Handle unexpandRectangle(Handle handle1, int setTrees);
Handle expandRectangle(Handle handle1,Handle handle2,double rng1, double rng2, int rots);

void unsetGraphFaceLinksBeforeMoveEdge20(Handle h1);
void unsetGraphFaceLinksBeforeMoveEdge02(Handle h1, Handle h2) ;
void setGraphFaceLinksAfterMoveEdge02(Handle h1);

	

void PrintRotationAroundEdge(Handle handle);
void printActionDetails();
void PrintTriangulationSize();
	


};
#endif
