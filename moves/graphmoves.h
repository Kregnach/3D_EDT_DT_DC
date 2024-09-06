#ifndef GRAPHMOVES_H
#define GRAPHMOVES_H


#include "triangulation.h"


template<typename RNG> std::pair<Handle,Handle> Triangulation::randomGraphSpanningTreeMove(RNG & rng) {
	// choose a uniform edge not in the spanning tree
	Handle newedge;
	do { newedge = getRandomUniformEdge(rng);} 
		while( isInSpanningTree(newedge) || newedge.getVertexNode() == newedge.getAdjacent2D().getVertexNode() );
	
	TreeNode * u = newedge.getVertexNode();
	TreeNode * v = newedge.getAdjacent2D().getVertexNode();

	dtree::WithEvert<TreeCount>::Evert(u);
	int cyclelength = TreeCount::AggrAnc(v);
	// the chain from v to u looks like vert-edg-vert-edge-vert (length 5 example)
	int numedges = (cyclelength-1)/2;
	int position = 1+2*uniform_int(rng,numedges);

	Handle oldedge = TreeCount::FindRootmostAnc(v, dtree::Index(position))->getHandle();

	setInSpanningTree(oldedge,false);
	setInSpanningTree(newedge,true);

	return {oldedge,newedge};
}
	
template<typename RNG> std::pair<bool,std::pair<Handle,Handle> > Triangulation::updateGraphDualSpanningTree(RNG & rng) {
	
	// choose a uniform face not in the dual spanning tree
	Handle newedge;
	do { newedge = getRandomEdge(rng); } while( isInDualSpanningTree(newedge) );
	
	if( newedge.getSimplex() == newedge.getAdjacent3D().getSimplex() ) return{false,std::pair<Handle,Handle>(newedge,newedge)};
	
	TreeNode * u = newedge.getSimplex()->getNode();
	TreeNode * v = newedge.getAdjacent3D().getSimplex()->getNode();
   
	dtree::WithEvert<TreeCount>::Evert(u);
	int cyclelength = TreeCount::AggrAnc(v);
	int position = uniform_int(rng,cyclelength);
	
	if( position == 0 ) return{false, std::pair<Handle,Handle>(newedge,newedge)};
	
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



template<typename RNG> bool Triangulation::TryGraphTreeMove(RNG & rng) {
	proposed_rate[7]++;
	double SA = getActionNow();

	if( numVertices() < 2 ) return false;

	Handle oldedge, newedge;
	std::tie(oldedge, newedge) = randomGraphSpanningTreeMove(rng);

	unsetGraphEdgeLinks(newedge);
	setGraphEdgeLinks(oldedge);
	
	double SB = getActionNow();
	
	double move_probability = uniform_real(rng);	
	
	if( !getProbabilityNow(move_probability,SA,SB,1) ) {
		unsetGraphEdgeLinks(oldedge);
		setInSpanningTree(newedge,false);
		setInSpanningTree(oldedge,true);
		setGraphEdgeLinks(newedge);
		return false;
	}

	accepted_rate[7]++;
	return true;
}

template<typename RNG> bool Triangulation::TryGraphDualTreeMove(RNG & rng) {
	proposed_rate[6]++;

	double SA = getActionNow();

	std::pair<bool, std::pair<Handle,Handle> > result = updateGraphDualSpanningTree(rng);

	if( !result.first ) return false;

	setGraphFaceLinks(result.second.first);
	unsetGraphFaceLinks(result.second.second);

	double SB = getActionNow();
	double move_probability = uniform_real(rng);
		
	if (!getProbabilityNow(move_probability,SA,SB,1)) { 
		
		setInDualSpanningTree(result.second.second,false);
		setInDualSpanningTree(result.second.first);
		
		unsetGraphFaceLinks(result.second.first);
		setGraphFaceLinks(result.second.second); 
	
		return false;
	};
	
	accepted_rate[6]++;
	return true;
}


	


//#############################################################################################
//################################### SET FACES AFTER MOVES ###################################
//#############################################################################################	


bool Triangulation::setGraphFaceLinksAfterMove02(Handle origin) {
	std::vector<Handle> facehandles = {
	    origin,
	    origin.getAdjacent2D(),
	    origin.getPrevious().getAdjacent2D(),
	    origin.getNext().getAdjacent2D(),
	    origin.getAdjacent3D().getPrevious().getAdjacent2D()};
	
	for( auto h : facehandles ) setGraphFaceLinks(h);
	
	
	return true;
}

bool Triangulation::unsetGraphFaceLinksAfterMove02(Handle origin) {
	std::vector<Handle> facehandles = {
	    origin,
	    origin.getAdjacent2D(),
	    origin.getPrevious().getAdjacent2D(),
	    origin.getNext().getAdjacent2D(),
	    origin.getAdjacent3D().getPrevious().getAdjacent2D()};
	
	for( auto h : facehandles ) unsetGraphFaceLinks(h);
	
	return true;
}

bool Triangulation::setGraphFaceLinksAfterMove20(Handle handle) { setGraphFaceLinks(handle); return true; }

bool Triangulation::unsetGraphFaceLinksAfterMove20(Handle handle) { unsetGraphFaceLinks(handle); return true; }


void Triangulation::clearGraphFaceLinksBeforeMove20(Handle origin) {
	std::vector<Handle> facehandles = {
	    origin,
	    origin.getAdjacent2D(),
	    origin.getPrevious().getAdjacent2D(),
	    origin.getNext().getAdjacent2D(),
	    origin.getAdjacent3D().getPrevious().getAdjacent2D()};

	for( auto h : facehandles ) unsetGraphFaceLinks(h);
}

bool Triangulation::setGraphFaceLinks(Handle h) {
	if( isInDualSpanningTree(h) ) return true;
	Handle current = h;

	do{ if( !isInSpanningTree(current) ) setHandleInMiddleGraph(current);
		current = current.getNext(); } 
		while(current != h);

	return true;
}

void Triangulation::unsetGraphFaceLinks(Handle h) {
	Handle current = h;
	do{ unsetHandleInMiddleGraph(current); 
		current = current.getNext(); } 
		while(current != h);
}


void Triangulation::clearGraphFaceLinksBeforeMove32(Handle diagonal) {
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

	for( auto h : facehandles ) unsetGraphFaceLinks(h);
}


void Triangulation::clearGraphFaceLinksBeforeMove23(Handle interface) {
	std::vector<Handle> facehandles{ 
	        interface.getAdjacent2D(),
	        interface.getNext().getAdjacent2D(),
	        interface.getPrevious().getAdjacent2D(),
	        interface.getPrevious().getAdjacent3D().getAdjacent2D(),
	        interface.getNext().getAdjacent3D().getAdjacent2D(),
	        interface.getAdjacent3D().getAdjacent2D(),
	        interface};
	        
	for( auto h : facehandles ) unsetGraphFaceLinks(h);
	
}

bool Triangulation::setGraphFaceLinksAfterMove32(Handle interface) {
	std::set<TreeNode *> faces{ 
		interface.getAdjacent2D().getFaceNode(),
		interface.getNext().getAdjacent2D().getFaceNode(),
		interface.getPrevious().getAdjacent2D().getFaceNode(),
		interface.getPrevious().getAdjacent3D().getAdjacent2D().getFaceNode(),
		interface.getNext().getAdjacent3D().getAdjacent2D().getFaceNode(),
		interface.getAdjacent3D().getAdjacent2D().getFaceNode(),
		interface.getFaceNode()}; 
	bool success = true;

	for( auto n : faces ) setGraphFaceLinks(n->getHandle()); 

	return true;
}

bool Triangulation::unsetGraphFaceLinksAfterMove32(Handle interface) {
	std::set<TreeNode *> faces{ 
		interface.getAdjacent2D().getFaceNode(),
		interface.getNext().getAdjacent2D().getFaceNode(),
		interface.getPrevious().getAdjacent2D().getFaceNode(),
		interface.getPrevious().getAdjacent3D().getAdjacent2D().getFaceNode(),
		interface.getNext().getAdjacent3D().getAdjacent2D().getFaceNode(),
		interface.getAdjacent3D().getAdjacent2D().getFaceNode(),
		interface.getFaceNode()}; 
	bool success = true;

	for( auto n : faces ) unsetGraphFaceLinks(n->getHandle()); 

	return true;
}


bool Triangulation::setGraphFaceLinksAfterMove23(Handle diagonal) {
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
	
	for( auto n : faces ) setGraphFaceLinks(n->getHandle());
	

return true;
}

bool Triangulation::unsetGraphFaceLinksAfterMove23(Handle diagonal) {
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
	
	for( auto n : faces ) unsetGraphFaceLinks(n->getHandle());

	return true;
}


bool Triangulation::setGraphEdgeLinks(Handle h) {
	if( isInSpanningTree(h) ) return true;
	Handle current = h;

	do{ if( !isInDualSpanningTree(current) ) {
			if( SameTree(current.getEdgeNode(),current.getFaceNode()) ) return false;
			setHandleInMiddleGraph(current); 
		}
		current = current.getRotationAroundEdge();  } 
		while(current != h);
	
	return true;
}

void Triangulation::unsetGraphEdgeLinks(Handle h) {
	Handle current = h;
	do{ unsetHandleInMiddleGraph(current);    
		current = current.getRotationAroundEdge(); } 
		while(current != h);
}

#endif
