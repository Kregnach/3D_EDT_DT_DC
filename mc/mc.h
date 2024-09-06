#ifndef MC_H
#define MC_H



enum MoveType { MOVE02, MOVE20, MOVE23, MOVE32, TREEMOVE, DUALTREEMOVE, FAILED };
const std::string MoveNames[7] = { "move02", "move20", "move23", "move32", "tree", "dualtree", "failed" };


//enum MoveType { MOVE14, MOVE41, MOVE23, MOVE32, FAILED };

template<typename RNG> Handle randomEdge(Triangulation & triangulation, RNG & rng) { return triangulation.getSimplex( uniform_int(rng, triangulation.numSimplices()) )->getEdge( uniform_int(rng,4), uniform_int(rng,3) ); }
template<typename RNG> Handle randomSimplex(Triangulation & triangulation, RNG & rng) { return triangulation.getSimplex( uniform_int(rng, triangulation.numSimplices()) ); }
template<typename RNG> Handle randomUniformEdge(Triangulation & triangulation, RNG & rng) { return triangulation.getNode(1,uniform_int(rng,triangulation.numEdges()))->getHandle(); }
template<typename RNG> Handle randomUniformVertex(Triangulation & triangulation, RNG & rng) { return triangulation.getNode(0,uniform_int(rng,triangulation.numVertices()))->getHandle(); }


template<typename RNG> void uniformDualSpanningTree(Triangulation & triangulation, RNG & rng) {    
    std::vector<bool> intree(triangulation.numSimplices(),false);
    std::vector<int> direction(triangulation.numSimplices(),-1);
    std::queue<Simplex *> queue;
    
    triangulation.clearDualSpanningTree();
    
    for(int i=0;i<triangulation.numSimplices();++i) queue.push(triangulation.getSimplex(i));
    
    // grow tree via Wilson's algorithm
    intree[queue.front()->getId()] = true;
    queue.pop();
        
    while( !queue.empty() ) {
        Simplex * start = queue.front();
        queue.pop();
        
        if( intree[start->getId()] ) continue;
        
        // perform random walk until hitting the tree
        Simplex * current = start;
        while( !intree[current->getId()] ) {
            int dir = uniform_int(rng,4);
            direction[current->getId()] = dir;
            current = current->getNeighbour(dir);
        }
        
        // retrace the loop-erased walk and add it to the tree
        current = start;
        while( !intree[current->getId()] ) {
            int dir = direction[current->getId()];
            intree[current->getId()] = true;
            setInDualSpanningTree(current,dir);
            current = current->getNeighbour(dir);
        }
    }
}


template<typename RNG> std::pair<bool,std::pair<Handle,Handle> > updateDualSpanningTree(Triangulation & triangulation, RNG & rng) {
    
    // choose a uniform face not in the dual spanning tree
    Handle newedge;
    
    do { newedge = randomEdge(triangulation,rng); } 
    	while( isInDualSpanningTree(newedge) );
    
    if( newedge.getSimplex() == newedge.getAdjacent3D().getSimplex() ) return{false,std::pair<Handle,Handle>(newedge,newedge)};
    
    TreeNode * u = newedge.getSimplex()->getNode();
    TreeNode * v = newedge.getAdjacent3D().getSimplex()->getNode();
   
    dtree::WithEvert<TreeCount>::Evert(u);
    int cyclelength = TreeCount::AggrAnc(v);
    int position = uniform_int(rng,cyclelength);
    
    if( position == 0 ) return{false,std::pair<Handle,Handle>(newedge,newedge)};
    
    TreeNode* w = TreeCount::FindRootmostAnc(v, dtree::Index(position));
    TreeNode* wparent = Parent(w);
    
    // find the right face adjacency
    for(int i=0;i<4;++i) {
        Simplex * s = w->getHandle().getSimplex();
        if( s->isInDualSpanningTree(i) && s->getNeighbour(i)->getNode() == wparent ) {
            Handle oldedge = s->getEdge(i,0);
            setInDualSpanningTree( oldedge, false);
            setInDualSpanningTree( newedge );
            
            //assert(triangulation.isDualSpanningTree());
            return{true,std::pair<Handle,Handle>(oldedge,newedge)};
        }
    }
    // should not reach this point
    assert(false);
    return{false,std::pair<Handle,Handle>(newedge,newedge)};
}

void vertexList(Triangulation & triangulation, std::vector<Handle> & vert, std::vector<int> & id) {
    id.resize(12*triangulation.numSimplices());
    std::fill(id.begin(),id.end(),-1);
    
    int nextid = 0;
    for(int i=0;i<triangulation.numSimplices();++i) {
        for(int j=0;j<4;++j) {
            for(int k=0;k<3;++k) {
                Handle edge = triangulation.getSimplex(i)->getEdge(j,k);
                if( id[edge.uniqueId()] == -1 ) {
                    
                    visitVertexHandles(edge,[&id,&nextid](Handle h)->bool{ id[h.uniqueId()] = nextid; return false; });
                    
                    vert.push_back(edge);
                    
                    nextid++;
                }
            }
        }
    }
}

template<typename RNG> std::pair<Handle,Handle> randomSpanningTreeMove(Triangulation & triangulation, RNG & rng) {
    
    // choose a uniform edge not in the spanning tree
    Handle newedge;
    
    do { newedge = randomUniformEdge(triangulation,rng); } 
    	while( isInSpanningTree(newedge) || newedge.getVertexNode() == newedge.getAdjacent2D().getVertexNode() );
    
    
    TreeNode * u = newedge.getVertexNode();
    TreeNode * v = newedge.getAdjacent2D().getVertexNode();
   
    dtree::WithEvert<TreeCount>::Evert(u);
    int cyclelength = TreeCount::AggrAnc(v);
    // the chain from v to u looks like vert-edg-vert-edge-vert (length 5 example)
    int numedges = (cyclelength-1)/2;
    int position = 1+2*uniform_int(rng,numedges);
    
    Handle oldedge = TreeCount::FindRootmostAnc(v, dtree::Index(position))->getHandle();
    
    return {oldedge,newedge};
}

void clearFaceLinksBeforeMove23(Triangulation & triangulation, Handle interface) {
    std::vector<Handle> facehandles{ 
            interface.getAdjacent2D(),
            interface.getNext().getAdjacent2D(),
            interface.getPrevious().getAdjacent2D(),
            interface.getPrevious().getAdjacent3D().getAdjacent2D(),
            interface.getNext().getAdjacent3D().getAdjacent2D(),
            interface.getAdjacent3D().getAdjacent2D(),
            interface};
    
    for( auto h : facehandles ) { clearFaceLinks(h); }
}
    
bool setFaceLinksAfterMove23(Triangulation & triangulation, Handle diagonal) {
    std::set<TreeNode *> faces{ 
            diagonal.getAdjacent3D().getPrevious().getAdjacent2D().getFaceNode(),
            diagonal.getNext().getAdjacent2D().getFaceNode(),
            diagonal.getAdjacent2D().getAdjacent3D().getNext().getAdjacent2D().getFaceNode(),
            diagonal.getAdjacent2D().getAdjacent3D().getPrevious().getAdjacent2D().getFaceNode(),
            diagonal.getPrevious().getAdjacent2D().getFaceNode(),
            diagonal.getAdjacent3D().getNext().getAdjacent2D().getFaceNode(),
            diagonal.getAdjacent3D().getAdjacent2D().getFaceNode(),
            diagonal.getAdjacent2D().getFaceNode(),
            diagonal.getFaceNode()
        };
        
    bool success = true;

    for( auto n : faces ) { if( !setFaceLinks(n->getHandle()) ) { success = false; break; } }

    /*if( !success || (triangulation.hasMiddleTree() && !triangulation.middleForestIsTree()) ) {
    	for( auto n : faces ) clearFaceLinks(n->getHandle()); 
    	
    	return false; 
    }*/
    
    return true;
}

void clearFaceLinksBeforeMove32(Triangulation & triangulation, Handle diagonal) {
    std::vector<Handle> facehandles{ 
            diagonal.getAdjacent3D().getPrevious().getAdjacent2D(),
            diagonal.getNext().getAdjacent2D(),
            diagonal.getAdjacent2D().getAdjacent3D().getNext().getAdjacent2D(),
            diagonal.getAdjacent2D().getAdjacent3D().getPrevious().getAdjacent2D(),
            diagonal.getPrevious().getAdjacent2D(),
            diagonal.getAdjacent3D().getNext().getAdjacent2D(),
            diagonal.getAdjacent3D().getAdjacent2D(),
            diagonal.getAdjacent2D(),
            diagonal
        };

    
    for( auto h : facehandles ) clearFaceLinks(h);
    
}
    
bool setFaceLinksAfterMove32(Triangulation & triangulation, Handle interface) {
    
    std::set<TreeNode *> faces{ 
        interface.getAdjacent2D().getFaceNode(),
        interface.getNext().getAdjacent2D().getFaceNode(),
        interface.getPrevious().getAdjacent2D().getFaceNode(),
        interface.getPrevious().getAdjacent3D().getAdjacent2D().getFaceNode(),
        interface.getNext().getAdjacent3D().getAdjacent2D().getFaceNode(),
        interface.getAdjacent3D().getAdjacent2D().getFaceNode(),
        interface.getFaceNode()}; 
    bool success = true;
    for( auto n : faces ) { if( !setFaceLinks(n->getHandle()) ) { success = false; break; } }

    /*if( !success || (triangulation.hasMiddleTree() && !triangulation.middleForestIsTree()) ) { 
    
    	for( auto n : faces ) clearFaceLinks(n->getHandle());
        
        return false;
    }*/
    return true;
}



void clearFaceLinksBeforeMove02(Triangulation & triangulation, Handle handle) { clearFaceLinks(handle); }
    
bool setFaceLinksAfterMove02(Triangulation & triangulation, Handle origin) {
    std::vector<Handle> facehandles = {
        origin,
        origin.getAdjacent2D(),
        origin.getPrevious().getAdjacent2D(),
        origin.getNext().getAdjacent2D(),
        origin.getAdjacent3D().getPrevious().getAdjacent2D()};
        
    bool success = true;
    
    for( auto h : facehandles ) clearFaceLinks(h);
        
    for( auto h : facehandles ) { if( !setFaceLinks(h) ) { success = false; break; } }
    
    /*if( !success || (triangulation.hasMiddleTree() && !triangulation.middleForestIsTree()) ) {
        for( auto h : facehandles ) clearFaceLinks(h);
        
        return false;
    }*/
    return true;
}

void clearFaceLinksBeforeMove20(Triangulation & triangulation, Handle origin) {
    std::vector<Handle> facehandles = {
        origin,
        origin.getAdjacent2D(),
        origin.getPrevious().getAdjacent2D(),
        origin.getNext().getAdjacent2D(),
        origin.getAdjacent3D().getPrevious().getAdjacent2D()};
    
    for( auto h : facehandles ) clearFaceLinks(h);

    
}
    
bool setFaceLinksAfterMove20(Triangulation & triangulation, Handle handle) {
    clearFaceLinks(handle);

    /*if( !setFaceLinks(handle) || (triangulation.hasMiddleTree() && !triangulation.middleForestIsTree()) ) { clearFaceLinks(handle); return false; }*/

    return true;
}



template<typename RNG> int MonteCarloGraphMove(Triangulation & triangulation, RNG & rng) {
    // enum MoveType { MOVE02, MOVE20, MOVE23, MOVE32, TREEMOVE, DUALTREEMOVE, FAILED }
    double probtree = probmt;
    double prob23 = probm2;
    double probe02 = probm3;
    double probv02 = probm1;
    
    
    double move_probability = uniform_real(rng, 0,probv02 + probtree + prob23 + probe02);
    
    int isantimove = (int)(2*uniform_real(rng));
    
    if( (move_probability < probv02)  && probv02 != 0) {
		if( isantimove ) return triangulation.TryGraphMove02(rng) ? 0 : 8; 
		else return triangulation.TryGraphMove20(rng) ? 1 : 8;
	}
	else if( (move_probability < probv02 + prob23)  && prob23!= 0 ) {
        if( isantimove ) return triangulation.TryGraphMove23(rng) ? 2 : 8;
        else return triangulation.TryGraphMove32(rng) ? 3 : 8;
    }
    else if( (move_probability < probtree +  probv02 + prob23 ) && probtree != 0) {
        if( isantimove ) return triangulation.TryGraphTreeMove(rng) ? 4 : 8;
        else return triangulation.TryGraphDualTreeMove(rng) ? 5 : 8;
    }
    else if( (move_probability < probe02 + probtree +  probv02 + prob23 )  && probe02 != 0 ){
        if( isantimove ) return triangulation.TryGraphMoveEdge02(rng) ? 6 : 8;
        else return triangulation.TryGraphMoveEdge20(rng) ? 7 : 8;
    }
	else { printf("prob is different!!! \n"); assert(0 == 1); }
	return 8;
}


#endif
