#ifndef TRIANGULATION_NODES_H
#define TRIANGULATION_NODES_H

#include "triangulation.h"

void Triangulation::clear() {
    for( auto s : simplices_ ) if( s ) delete s;
    
    simplices_.clear();
    for(int i=0;i<4;++i) {
        for( auto n : nodes_[i] ) if( n ) delete n;
        
        nodes_[i].clear();
    }
}


TreeNode * Triangulation::getNode(int dimension, int i) { return nodes_[dimension][i]; }

TreeNode * Triangulation::newNode(int dim) {
    TreeNode * newnode = new TreeNode;
    newnode->setId(nodes_[dim].size());
	
	newnode->setInMiddleGraph(false);
	newnode->setGraphCoord();
	newnode->setGraphId(ABSMAX_DC-1);
		
    newnode->setDimension(dim);
    
    nodes_[dim].push_back(newnode); //increment size
    
    newnode->setCoord(); //set initial coordination number to zero
    
    return newnode;
}


Simplex * Triangulation::getSimplex(int i) { return simplices_[i]; }

 Simplex * Triangulation::newSimplex() {
    TreeNode * node = newNode(3);
    simplices_.push_back(new Simplex(node));
    simplices_.back()->setId(simplices_.size()-1);
    return simplices_.back();
}


void Triangulation::deleteNode(TreeNode * node) { // 
    int dim = node->getDimension();
    assert( node == nodes_[dim][node->getId()] );
    
    DelNodeMiddleGraph(node);//if it concerns middle graph set the free element to be the one that was deleted		
     	
    /////TEST////
    if( Parent(node) || Root(node)!=node ) {
        std::cout << "node: " << node->getDimension() << " " << node->getId() << "\n";
        if( Parent(node) ) std::cout << "parent: " << Parent(node)->getDimension() << " " << Parent(node)->getId() << "\n";
        
        if( Root(node)!=node ) std::cout << "root: " << Root(node)->getDimension() << " " << Root(node)->getId() << "\n";
        
        assert(false);
    }
    
    nodes_[dim].back()->setId(node->getId());
    std::swap(nodes_[dim][node->getId()],nodes_[dim].back());
    
    delete nodes_[dim].back();
    nodes_[dim].pop_back();
}

void Triangulation::deleteSimplex(Simplex * simplex) {
    assert( simplex == simplices_[simplex->getId()] );
    
    if( root_.getSimplex() == simplex ) root_ = Handle(0,0,0);
    
    deleteNode(simplex->getNode());
    simplices_.back()->setId(simplex->getId());
    std::swap(simplices_[simplex->getId()],simplices_.back());
    
    delete simplices_.back();
    
    simplices_.pop_back();
}


void Triangulation::setFlag(TreeNode * n) { flag_[n->getDimension()][n->getId()] = currentflag_[n->getDimension()]; }

void Triangulation::resetFlag(int dimension) {
    if( flag_[dimension].size() < nodes_[dimension].size() ) flag_[dimension].resize( nodes_[dimension].size(), 0 );
    
    if( currentflag_[dimension] == 0 || currentflag_[dimension] == std::numeric_limits<uint32_t>::max() ) {
        std::fill(flag_[dimension].begin(),flag_[dimension].end(),0);
        currentflag_[dimension]=0;
    }
    currentflag_[dimension]++;
}

bool Triangulation::isFlagged(TreeNode * n) { return flag_[n->getDimension()][n->getId()] == currentflag_[n->getDimension()]; }


std::pair<bool,Handle> Triangulation::isFreeFace(Handle edge,std::vector<bool> &edge_present,std::vector<bool> &face_present) {
    if( !face_present[4*edge.getSimplex()->getId()+edge.getFace()] ) return{false,edge};
    
    int num_present_edges = 0;
    Handle presentedge = edge;
    Handle current = edge;
    for(int i=0;i<3;++i) {
        if( edge_present[current.uniqueId()] ) {
            num_present_edges++;
            presentedge = current;
        }
        current = current.getNext();
    }             
    if( num_present_edges != 1 ) return{false,edge};
    
    return{true,presentedge}; 
    
}


int Triangulation::getMaxVertexCoord() {
	int max = 0;
	for( auto n : nodes_[0] ) {
		Handle current = n->getHandle();
		
		if (n->getCoord()  > max) max = n->getCoord();
	} 
	//if(max > numSimplices()) { printf("%d\t%d \n",numSimplices(),max); assert(max <= numSimplices());}
	return max;
}

int Triangulation::getMaxEdgeCoord() {
	int max = 0;	
	for( auto n : nodes_[1] ) {
		Handle current = n->getHandle();
		if (n->getCoord()  > max) max = n->getCoord();
	} 
			
//	
	//if(max > numSimplices()) { printf("%d\t%d \n",numSimplices(),max); assert(max <= numSimplices());}
	return max;
}

std::vector<int> Triangulation::getNodeCoordVector(int dim) {
	std::vector<int> node_cord_vec;
	
	for(int i = 0 ; i < nodes_[dim].size(); i++) node_cord_vec.push_back(nodes_[dim][i]->getCoord()); 
	
	return node_cord_vec;
}

std::vector<int> Triangulation::getGraphCoordVector() {
	std::vector<int> node_cord_vec;
	
	for(int i = 0 ; i < nodes_[1].size(); i++) if(nodes_[1][i]->getInMiddleGraph()) node_cord_vec.push_back(nodes_[1][i]->getGraphCoord());
	
	return node_cord_vec;
}

    
    
    
    
#endif
