#ifndef SIMP_STRUCT_H
#define SIMP_STRUCT_H

class Simplex;
class NodeBase;

typedef dtree::WithAncDescAggr<dtree::Count<int>, dtree::Begin<NodeBase> > TreeCount;
typedef dtree::EndTreeWithDesc<dtree::WithEvert<TreeCount> > TreeNode;


class Handle {
private:
    Simplex * simplex_;
    int face_;
    int edge_;
public:
    Handle( ) {}
    Handle( Simplex * simplex, int face, int edge ) : simplex_(simplex), face_(face), edge_(edge) {}
    
    Simplex * getSimplex() { return simplex_; }
    
    TreeNode * getVertexNode();
    TreeNode * getEdgeNode();
    TreeNode * getFaceNode();
    TreeNode * getSimplexNode();
    
    void setVertexNode(TreeNode * v);
    void setEdgeNode(TreeNode * v);
    void setFaceNode(TreeNode * v);
    
    Handle getNext() { return Handle(simplex_,face_,(edge_+1)%3); }
    Handle getPrevious() { return Handle(simplex_,face_,(edge_+2)%3); }
   
    Handle getAdjacent2D() {
        static const int adj[4][3][2] =
            {{ {1,2},{3,0},{2,2} },
             { {2,1},{3,1},{0,0} },
             { {3,2},{1,0},{0,2} },
             { {0,1},{1,1},{2,0} }
            };
        return Handle(simplex_,adj[face_][edge_][0],adj[face_][edge_][1]);
    }
    
    Handle getAdjacent3D();
    
    int getFace() { return face_; }
    int getEdge() { return edge_; }
    
    Handle getRotationAroundEdge(){ return getAdjacent3D().getAdjacent2D(); }
    Handle getRotationAroundEdge(int n) {
        
        Handle edge(*this);
        
        if( n > 0 ) for(int i=0 ; i < n ; ++i)  edge = edge.getAdjacent3D().getAdjacent2D();  
        
        else if( n < 0 ) for(int i=0 ; i < -n ; ++i) edge = edge.getAdjacent2D().getAdjacent3D();
        
        return edge;
    }
    
    int uniqueId();
    void setInSpanningTree(bool in=true);

    friend bool operator==(const Handle &s1, const Handle &s2);
    friend bool operator!=(const Handle &s1, const Handle &s2);
    friend bool operator<(const Handle &s1, const Handle &s2);
    friend bool operator>(const Handle &s1, const Handle &s2);
    friend bool sameFace(const Handle &s1, const Handle &s2);
    friend bool sameCorner(const Handle &s1, const Handle &s2);

};

bool operator==(const Handle &s1, const Handle &s2) { return s1.simplex_ == s2.simplex_ && s1.face_ == s2.face_ && s1.edge_ == s2.edge_; }
bool operator!=(const Handle &s1, const Handle &s2) { return s1.simplex_ != s2.simplex_ || s1.face_ != s2.face_ || s1.edge_ != s2.edge_; }
bool operator<(const Handle &s1, const Handle &s2) { return s1.simplex_ < s2.simplex_ || ( s1.simplex_== s2.simplex_ && (s1.face_ < s2.face_ || (s1.face_ == s2.face_ && s1.edge_ < s2.edge_) ) ); }
bool operator>(const Handle &s1, const Handle &s2) { return s1.simplex_ > s2.simplex_ || ( s1.simplex_== s2.simplex_ && (s1.face_ > s2.face_ || (s1.face_ == s2.face_ && s1.edge_ > s2.edge_) ) ); }

bool sameFace(const Handle &s1, const Handle &s2) { return s1.simplex_ == s2.simplex_ && s1.face_ == s2.face_; }

bool sameCorner(const Handle &s1, const Handle &s2) {
    static const int corner[4][3] = {{3,2,1},{3,0,2},{1,0,3},{1,2,0}};
    return s1.simplex_ == s2.simplex_ && corner[s1.face_][s1.edge_] == corner[s2.face_][s2.edge_];
}

class NodeBase {
private:
    
    Handle handle_;
    
    char dimension_; // dimensionality (0....3) of the node
    
    int id_; // label of the node
    int graph_id_; // label of th enode in the middle graph
    bool inMiddleGraph_; // a number that signals whether the node is in middle graph.
//    int igraph_id_; // inactive graph index, the index of the node in the inactive graph array. (possibly not necessary)
    int graphCoord_; // MiddleGraph coordination number of Middle Graph nodes
    int ncoord_;
    
    int coord_index_; //index of the element in the specific coord array
    
public:
    Handle getHandle() { return handle_; }
    
    int getDimension() { return dimension_; }
    int getId() { return id_; }
    int getGraphId() { return graph_id_; }
    int getNCoord() { return ncoord_; }
    int getGraphCoord() {return graphCoord_; }
    int getCoord() { return ncoord_; }

    void setHandle(Handle h) { handle_ = h; }
    void setId(int id) { id_ = id; }
    void setGraphId(int id) { graph_id_ = id; }
    void setDimension(int dimension) { dimension_ = dimension; }
    void setInMiddleGraph(bool init) { inMiddleGraph_ = init ; }
    void incrementGraphCoord() { graphCoord_++; }
    void decrementGraphCoord() { graphCoord_--; }
    void setGraphCoord() { graphCoord_= 0; }
    void setCoord() { ncoord_ = 0; };
    

    void incrementCoord() { ncoord_++; }
    void decrementCoord() { ncoord_--; }
    void setCoordIndex(int n) { coord_index_ = n; }
	int getCoordIndex() { return coord_index_; }
	
	
    bool getInMiddleGraph() { return inMiddleGraph_ ; }
};

const int simplexHandleVertexIncidence[4][3] = {{3,2,1},{3,0,2},{1,0,3},{1,2,0}};
const int simplexVertexHandleIncidence[4][2] = {{1,1},{0,2},{1,2},{0,0}};
const int simplexHandleEdgeIncidence[4][3] = {{0,2,4},{5,1,0},{3,5,4},{2,1,3}};
const int simplexEdgeHandleIncidence[6][2] = {{0,0},{1,1},{0,1},{2,0},{0,2},{1,0}};

class Simplex {
private:

	Simplex * nbr_simplex_[4]; // Edge 0 of face i is adjacent (in the 3d sense) to

    int id_;    
    int nbr_face_[4];          // edge nbr_edge_[i] of face nbr_face_[i] of simplex nbr_simplex_[i]
    int nbr_edge_[4];
    
    bool in_dual_spanning_tree_[4];
    bool in_spanning_tree_[6];
    
    TreeNode * node3_;
    TreeNode * node2_[4];
    TreeNode * node1_[6];
    TreeNode * node0_[4];
public:
    
     void printFullInfo() {
    	printf("simplex: %d\tvertices:\n",getId());
    	for(int i = 0; i <4 ; i++) { printf("%d\t",node0_[i]->getId()); }
    	printf("\nlink:\n");
    	
    	for(int i = 0; i <6 ; i++) { printf("%d\t",node1_[i]->getId()); }
    	printf("\ntriangles:\n");
    	
    	for(int i = 0; i <4 ; i++) { printf("%d\t",node2_[i]->getId()); }
    	printf("\n");
    }
    
    Handle getEdge(int face, int edge) { return Handle(this,face,edge); }
	
	Handle getAdjacent3D(int face, int edge) {
        int nbr_edge = (nbr_edge_[face] + 3 - edge)%3;
        return Handle( nbr_simplex_[face], nbr_face_[face], nbr_edge);
    }
    Handle getVertexHandle(int i) { return Handle(this,simplexVertexHandleIncidence[i][0],simplexVertexHandleIncidence[i][1]); } // return handle starting at i'th vertex
    Handle getEdgeHandle(int i) { return Handle(this,simplexEdgeHandleIncidence[i][0],simplexEdgeHandleIncidence[i][1]); }
    
    TreeNode * getNode() { return node3_; }
    TreeNode * getFaceNode(int i) { return node2_[i]; }
    TreeNode * getEdgeNode(int i) { return node1_[i]; }
    TreeNode * getEdgeNode(int face, int edge) { return node1_[simplexHandleEdgeIncidence[face][edge]]; }
	TreeNode * getVertexNode(int i) { return node0_[i]; }
	TreeNode * getVertexNode(int face, int edge) { return getVertexNode(simplexHandleVertexIncidence[face][edge]); }
	
	Simplex * getNeighbour(int face) { return nbr_simplex_[face]; }
	
	int getNeighbourOppositeVertexID(int face) { return node2_[face]->getHandle().getAdjacent2D().getPrevious().getVertexNode()->getId(); }
	
	Simplex(TreeNode * node3) : node3_(node3) { 
        node3_->setHandle(Handle(this,0,0));
        clearSpanningTree();
        clearDualSpanningTree();
    }
    
	int getId() { return id_; }
	
	int getFaceNodeGraphId(int i) { return node2_[i]->getGraphId(); }
    int getEdgeNodeGraphId(int i) { return node1_[i]->getGraphId(); }
    
    bool isInDualSpanningTree(int face) { return in_dual_spanning_tree_[face]; }
	bool isInSpanningTree(int face, int edge) { return in_spanning_tree_[simplexHandleEdgeIncidence[face][edge]]; }
	
	
    void setInDualSpanningTree(int face, bool in=true) { in_dual_spanning_tree_[face] = in; }
    void setId(int id) { id_ = id; }
    void setInSpanningTree(int face, int edge, bool in=true) { in_spanning_tree_[simplexHandleEdgeIncidence[face][edge]] = in; }
    void clearSpanningTree() { for(int i=0;i<6;++i) in_spanning_tree_[i] = false; }
    void clearDualSpanningTree() { for(int i=0;i<4;++i) in_dual_spanning_tree_[i] = false; }
    
    void setFaceNode(int i, TreeNode * n) {
        node2_[i] = n;
        if( n ) n->setHandle( Handle(this,i,0));
    }
    
    void setEdgeNode(int i, TreeNode * n) {
        node1_[i] = n;
        if( n ) n->setHandle( Handle(this,simplexEdgeHandleIncidence[i][0], simplexEdgeHandleIncidence[i][1]));
    }
    void setEdgeNode(int face, int edge, TreeNode * n) {
        node1_[simplexHandleEdgeIncidence[face][edge]] = n;
        if( n ) n->setHandle( Handle(this,face,edge) );
    }
    
    void setVertexNode(int i, TreeNode * n) {
        node0_[i] = n;
        if( n ) n->setHandle( Handle(this,simplexVertexHandleIncidence[i][0], simplexVertexHandleIncidence[i][1]));
    }

    void setVertexNode(int face, int edge, TreeNode * n) {
        setVertexNode(simplexHandleVertexIncidence[face][edge],n);
        if( n ) n->setHandle(Handle(this,face,edge));
    }

    void setNeighbour(Simplex * nbr, int face, int edge, int nbr_face, int nbr_edge ) {
        nbr_simplex_[face] = nbr;
        nbr_face_[face] = nbr_face;
        nbr_edge_[face] = (edge + nbr_edge) % 3;
    }
};


#endif
