#ifndef TREE_FUNCTIONS_H
#define TREE_FUNCTIONS_H



bool isInDualSpanningTree(Handle h) { return h.getSimplex()->isInDualSpanningTree(h.getFace()); }
bool isInSpanningTree(Handle h) { return h.getSimplex()->isInSpanningTree(h.getFace(),h.getEdge()); }

void setInDualSpanningTree(Handle h, bool in=true) {
    if( in == isInDualSpanningTree(h) ) { return; }
    h.getSimplex()->setInDualSpanningTree(h.getFace(),in);
    Handle adj = h.getAdjacent3D();
    adj.getSimplex()->setInDualSpanningTree(adj.getFace(),in);
 
    TreeNode * u = h.getSimplex()->getNode();
    TreeNode * v = adj.getSimplex()->getNode();
    if( in ) {
        if( Root(v) == v ) { Link(v,u); } 
        else {
            dtree::WithEvert<TreeCount>::Evert(u);
            Link(u,v);
        }
    } 
    else  {
        if( Parent(v) == u ) { Cut(v); } // there is a link from v to u, so remove it
        
        else Cut(u);
    }
}
void setInDualSpanningTree(Simplex * s, int face, bool in=true) { setInDualSpanningTree(s->getEdge(face,0)); }

void setInSpanningTree(Handle h, bool in=true) {
    if( in == isInSpanningTree(h) ) return;

    Handle edge = h;
    do {
        edge.getSimplex()->setInSpanningTree(edge.getFace(),edge.getEdge(),in);
        edge = edge.getRotationAroundEdge();
    } while( edge != h );
    
    TreeNode * u = h.getVertexNode();
    TreeNode * v = h.getAdjacent2D().getVertexNode();
    TreeNode * edgenode = h.getEdgeNode();
    if( in ) {
        // assuming edgenode is a singleton
        assert( Root(edgenode) == edgenode );
        Link( edgenode, u);
        dtree::WithEvert<TreeCount>::Evert(edgenode);
        Link( edgenode, v);
    } 
    else  {
        if( Parent(u) == edgenode ) Cut(u);
        if( Parent(v) == edgenode ) Cut(v);
        if( Parent(edgenode) ) Cut(edgenode);
    }
}
void setInMiddleTree(Handle h, bool in=true) {
    TreeNode * u = h.getEdgeNode();
    TreeNode * v = h.getFaceNode();
    if( in && Parent(u) != v && Parent(v) != u ) {
    	if( Root(v) == v ) { Link(v,u); } 
    	else {
            dtree::WithEvert<TreeCount>::Evert(u);
            Link(u,v);
        }
    } 
    else if( !in && (Parent(u) == v || Parent(v) == u ) ) { if( Parent(v) == u ) { Cut(v); } // there is a link from v to u, so remove it
        else Cut(u);
    }
}

void clearFaceLinks(Handle h) {
    Handle current = h;
    do{
        setInMiddleTree(current,false);    
        current = current.getNext();
    }while(current != h);
}
bool setFaceLinks(Handle h) {
    if( isInDualSpanningTree(h) ) return true;
    
    Handle current = h;
    do{
        if( !isInSpanningTree(current) ) {             
        	if( SameTree(current.getEdgeNode(),current.getFaceNode()) ) return false;
            setInMiddleTree(current);    
        }
        current = current.getNext();
    } while(current != h);
    return true;
}

void clearEdgeLinks(Handle h) {
    Handle current = h;
    do{
        setInMiddleTree(current,false);    
        current = current.getRotationAroundEdge();
        
    }while(current != h);
}
bool setEdgeLinks(Handle h) {
    if( isInSpanningTree(h) ) return true;
    
    Handle current = h;
    do{
    	if( !isInDualSpanningTree(current) ) {
        	if( SameTree(current.getEdgeNode(),current.getFaceNode()) ) return false;

            setInMiddleTree(current);    
        }
        current = current.getRotationAroundEdge();
    }while(current != h);
    return true;
}

Handle Handle::getAdjacent3D() { return simplex_->getAdjacent3D(face_,edge_); }
int Handle::uniqueId() { return 12*simplex_->getId() + 3*face_ + edge_; }

TreeNode * Handle::getVertexNode() { return simplex_->getVertexNode(face_,edge_); }
TreeNode * Handle::getEdgeNode() {	return simplex_->getEdgeNode(face_,edge_); }
TreeNode * Handle::getFaceNode() { return simplex_->getFaceNode(face_); }
TreeNode * Handle::getSimplexNode() { return simplex_->getNode(); }

void Handle::setVertexNode(TreeNode * n) { simplex_->setVertexNode(face_,edge_,n); }
void Handle::setEdgeNode(TreeNode * n) { simplex_->setEdgeNode(face_,edge_,n); }
void Handle::setFaceNode(TreeNode * n) { simplex_->setFaceNode(face_,n); }
void Handle::setInSpanningTree(bool in) { simplex_->setInSpanningTree(face_,edge_,in); }

void setAdjacent(Handle h1, Handle h2) {
    h1.getSimplex()->setNeighbour( h2.getSimplex(), h1.getFace(), h1.getEdge(), h2.getFace(), h2.getEdge() );
    h2.getSimplex()->setNeighbour( h1.getSimplex(), h2.getFace(), h2.getEdge(), h1.getFace(), h1.getEdge() );   
}

template<typename Function> void visitVertexHandles(Handle edge, Function visit) {
    std::set<Handle> explored;
    std::queue<Handle> queue;
    queue.push(edge);
    while( !queue.empty() ) {
        Handle current = queue.front();
        queue.pop();
        
        std::pair<std::set<Handle>::iterator, bool> result = explored.insert(current);
        if( result.second ) {
            if( visit(current) ) return;
            queue.push( current.getAdjacent3D().getNext() );
            
            for(int i=0;i<2;++i) {
                current = current.getAdjacent2D().getNext();
                explored.insert(current);
                if( visit(current) ) return;
                queue.push( current.getAdjacent3D().getNext() );
            }
        }
    }
}

void setVertexNode(Handle edge, TreeNode * n) { visitVertexHandles(edge,[n](Handle h)->bool{ h.setVertexNode(n); return false; }); }

void setEdgeNode(Handle edge, TreeNode * n) {
    Handle current = edge;
    do{
        current.setEdgeNode(n);
        current = current.getRotationAroundEdge();
    } while( current != edge);
}

void setFaceNode(Handle edge, TreeNode * n) { edge.setFaceNode(n); edge.getAdjacent3D().setFaceNode(n); }

template<typename Function> void visitVertexCorners(Handle edge, Function visit) {
    std::set<Handle> explored;
    std::queue<Handle> queue;
    queue.push(edge);
    while( !queue.empty() ) {
        Handle current = queue.front();
        queue.pop();
        
        std::pair<std::set<Handle>::iterator, bool> result = explored.insert(current);
        if( result.second ) {
            if( visit(current) ) return;
            queue.push( current.getAdjacent3D().getNext() );
            
            for(int i=0;i<2;++i) {
                current = current.getAdjacent2D().getNext();
                explored.insert(current);
                queue.push( current.getAdjacent3D().getNext() );
            }
        }
    }   
}

Handle getRotationAroundGraphEdge(Handle currentEdge, int n) {
	int count = 0;

	if (n > 0) {
	    while (count < n) {
	        currentEdge = currentEdge.getAdjacent3D().getAdjacent2D();   // Rotate to the next triangle
	        if (!isInDualSpanningTree(currentEdge)) ++count; 
	    }
	} 
	else {
	    while (count < -n) {
	        currentEdge = currentEdge.getAdjacent2D().getAdjacent3D(); // Rotate to the previous triangle
	        if (!isInDualSpanningTree(currentEdge)) ++count;
	    }
	}

	return currentEdge;
}

/*
template<typename Function>
void visitVertexCorners(Vertex * vertex, Function visit) {
    visitVertexCorners(vertex->getEdge(),visit);  
}*/

// Apply `visit` to one outgoing handle for each edge
// leaving the vertex at the origin of `edge`, starting
// with `edge` itself.
template<typename Function> void visitVertexEdges(Handle edge, Function visit) {
    std::set<Handle> explored;
    std::queue<Handle> queue;
    queue.push(edge);
    while( !queue.empty() ) {
        Handle current = queue.front();
        queue.pop();
        
        std::pair<std::set<Handle>::iterator, bool> result = explored.insert(current);
        if( result.second ) {
            if( visit(current) )  return;
            Handle edge = current;
            do {
                explored.insert(edge);
                queue.push( edge.getAdjacent3D().getNext() );
                edge = edge.getRotationAroundEdge();
            } while( edge != current );
        }
    }   
}
/*
template<typename Function>
void visitVertexEdges(Vertex * vertex, Function visit) {
    visitVertexEdges(vertex->getEdge(), visit);
}*/

int vertexEdgeDegree(Handle edge) {
    int nedges = 0;
    visitVertexEdges(edge,[&nedges](Handle h)->bool{ nedges++; return false; }); 
    return nedges;
}

int vertexCornerDegree(Handle edge) {
    int ncorners = 0;
    visitVertexCorners(edge,[&ncorners](Handle h)->bool{ ncorners++; return false; });
    return ncorners;
}

int edgeDegree(Handle edge) {
    int nedges = 0;
    Handle current = edge;
    do {
        nedges++;
        current = current.getRotationAroundEdge();
    } 
    while( current != edge );
    return nedges;
}

bool graphIsForestOnBoundary(std::vector<std::vector<int> > & nbr, int boundarylength) {
    // check that the graph defined by nbr is a spanning forest with
    // at least one external point (<6) in every tree
    std::vector<int> approach(nbr.size(),-2);
    std::queue<int> queue;
    for(int i=0;i<boundarylength;++i) {
        if( approach[i] != -2 ) continue;

        approach[i] = -1;
        
        queue.push(i);
        while( !queue.empty() ) {
            int current = queue.front();
            queue.pop();
            
            for( auto j : nbr[current] ) {
                if( j != approach[current] ) {
                    if( approach[j] != -2 ) { return false; }
                    approach[j] = current;
                    queue.push(j);
                }
            }
        }
    }
    for( auto a : approach ) { if( a == -2 ) return false; }

	return true;
}



struct SpanningMove {
    unsigned long id_before;
    unsigned long id_after;
    int options;
    int return_options;
};




void clearLinksBeforeMove23(Handle interface) {
    std::vector<Handle> faces{ 
        interface.getAdjacent2D(),
        interface.getNext().getAdjacent2D(),
        interface.getPrevious().getAdjacent2D(),
        interface.getPrevious().getAdjacent3D().getAdjacent2D(),
        interface.getNext().getAdjacent3D().getAdjacent2D(),
        interface.getAdjacent3D().getAdjacent2D(),
        interface};

    for(int i=0;i<7;++i) { setInDualSpanningTree(faces[i],false); }

}


void clearLinksBeforeMove32(Handle diagonal) {
    std::vector<Handle> faces{ 
        diagonal.getAdjacent3D().getPrevious().getAdjacent2D(),
        diagonal.getNext().getAdjacent2D(),
        diagonal.getAdjacent2D().getAdjacent3D().getNext().getAdjacent2D(),
        diagonal.getAdjacent2D().getAdjacent3D().getPrevious().getAdjacent2D(),
        diagonal.getPrevious().getAdjacent2D(),
        diagonal.getAdjacent3D().getNext().getAdjacent2D(),
        diagonal.getAdjacent3D().getAdjacent2D(),
        diagonal.getAdjacent2D(),
        diagonal};
    for(int i=0;i<9;++i) { setInDualSpanningTree(faces[i],false); }
}






template<typename RNG> std::pair<bool,SpanningMove> getDualSpanningTreeMove23Id(Handle interface, RNG & rng, bool clearlinks=false) {
    std::vector<Handle> faces{ 
        interface.getAdjacent2D(),
        interface.getNext().getAdjacent2D(),
        interface.getPrevious().getAdjacent2D(),
        interface.getPrevious().getAdjacent3D().getAdjacent2D(),
        interface.getNext().getAdjacent3D().getAdjacent2D(),
        interface.getAdjacent3D().getAdjacent2D(),
        interface};
        
    std::bitset<7> b;
    for(int i=0;i<7;++i) { b[i] = isInDualSpanningTree(faces[i]); }

    SpanningMove move;
    move.id_before = b.to_ulong();
    move.options = move23_dual_tree_table[move.id_before].size();
    if( move.options > 0 ) {
        move.id_after = move23_dual_tree_table[move.id_before][uniform_int(rng,move.options)];
        move.return_options = move32_dual_tree_table[move.id_after].size();
    } 
    else { return{false,move}; }
    
    // check that the tree prescription is compatible with self-adjacency
    std::bitset<9> b_after(move.id_after);
    
    static const int edg[9][2] = {{0, 6}, {1, 8}, {2, 7}, {3, 7}, {4, 8}, {5, 6}, {6, 7}, {7, 8}, {6, 8}};
    std::vector<std::vector<int> > nbr(9);
    for(int i=0;i<9;++i) {
        if( b_after[i] ) {
            nbr[edg[i][0]].push_back(edg[i][1]);
            nbr[edg[i][1]].push_back(edg[i][0]);
        }
    }
    
    for(int i=0;i<6;++i) {
        for(int j=i+1;j<6;++j) {
            if( sameFace(faces[i].getAdjacent3D(),faces[j]) ) {
                if( b_after[i] != b_after[j] || (b_after[i] && nbr[i][0] == nbr[j][0] ) ) return{false,move};

                if( b_after[i] ) {
                    nbr[nbr[i][0]].push_back(nbr[j][0]);
                    nbr[nbr[i][0]].push_back(nbr[j][0]);
                    nbr[i].clear();
                    nbr[j].clear();
                }
            }
        }
    }
    
    if( !graphIsForestOnBoundary(nbr,6) ) return{false,move};
    
    if( clearlinks ) for(int i=0;i<7;++i) if( b[i] ) setInDualSpanningTree(faces[i],false);
    
    return{true,move};
}



template<typename RNG> std::pair<bool,SpanningMove>  getDualSpanningTreeMove32Id(Handle diagonal, RNG & rng, bool clearlinks=false) {
    std::vector<Handle> faces{ 
        diagonal.getAdjacent3D().getPrevious().getAdjacent2D(),
        diagonal.getNext().getAdjacent2D(),
        diagonal.getAdjacent2D().getAdjacent3D().getNext().getAdjacent2D(),
        diagonal.getAdjacent2D().getAdjacent3D().getPrevious().getAdjacent2D(),
        diagonal.getPrevious().getAdjacent2D(),
        diagonal.getAdjacent3D().getNext().getAdjacent2D(),
        diagonal.getAdjacent3D().getAdjacent2D(),
        diagonal.getAdjacent2D(),
        diagonal};
    std::bitset<9> b;
    for(int i=0;i<9;++i) { b[i] = isInDualSpanningTree(faces[i]); }
    
    SpanningMove move;
    move.id_before = b.to_ulong();
    move.options = move32_dual_tree_table[move.id_before].size();
    if( move.options > 0 ) {
        move.id_after = move32_dual_tree_table[move.id_before][uniform_int(rng,move.options)];
        move.return_options = move23_dual_tree_table[move.id_after].size();
    } 
    else { return{false,move}; }
    
    std::bitset<7> b_after(move.id_after);
    
    
    static const int edg[7][2] = {{0, 6}, {1, 6}, {2, 6}, {3, 7}, {4, 7}, {5, 7}, {6, 7}};
    std::vector<std::vector<int> > nbr(8);
    for(int i=0;i<7;++i) { if( b_after[i] ) {
            nbr[edg[i][0]].push_back(edg[i][1]);
            nbr[edg[i][1]].push_back(edg[i][0]);
        }
    }
    
    for(int i=0;i<6;++i) {
        for(int j=i+1;j<6;++j) {
            if( sameFace(faces[i].getAdjacent3D(),faces[j]) ) {
                if( b_after[i] != b_after[j] || (b_after[i] && nbr[i][0] == nbr[j][0] ) ) return{false,move};
                
                if( b_after[i] ) {
                    nbr[nbr[i][0]].push_back(nbr[j][0]);
                    nbr[nbr[i][0]].push_back(nbr[j][0]);
                    nbr[i].clear();
                    nbr[j].clear();
                }
            }
        }
    }
    
    if( !graphIsForestOnBoundary(nbr,6) )
        return{false,move};

    if( clearlinks ) { for(int i=0;i<9;++i) { if( b[i] ) { setInDualSpanningTree(faces[i],false); } } }
    
    return{true,move};
}



void setInDualSpanningTreeAfterMove23(Handle diagonal, uint32_t code) {
    std::vector<Handle> faces{ 
        diagonal.getAdjacent3D().getPrevious().getAdjacent2D(),
        diagonal.getNext().getAdjacent2D(),
        diagonal.getAdjacent2D().getAdjacent3D().getNext().getAdjacent2D(),
        diagonal.getAdjacent2D().getAdjacent3D().getPrevious().getAdjacent2D(),
        diagonal.getPrevious().getAdjacent2D(),
        diagonal.getAdjacent3D().getNext().getAdjacent2D(),
        diagonal.getAdjacent3D().getAdjacent2D(),
        diagonal.getAdjacent2D(),
        diagonal};
        
    std::bitset<9> b(code);
    for(int i=0;i<9;++i) { setInDualSpanningTree(faces[i],b[i]); }
}

void setInDualSpanningTreeAfterMove32(Handle interface, uint32_t code) {
   std::vector<Handle> faces{ 
        interface.getAdjacent2D(),
        interface.getNext().getAdjacent2D(),
        interface.getPrevious().getAdjacent2D(),
        interface.getPrevious().getAdjacent3D().getAdjacent2D(),
        interface.getNext().getAdjacent3D().getAdjacent2D(),
        interface.getAdjacent3D().getAdjacent2D(),
        interface};
    std::bitset<7> b(code);

	for(int i=0;i<7;++i) setInDualSpanningTree(faces[i],b[i]);
}




#endif
