#ifndef TRIANGULATION_TREEFUNCTIONS_H
#define TRIANGULATION_TREEFUNCTIONS_H

#include "triangulation.h"


void Triangulation::clearSpanningTree() { for( auto s : simplices_ ) for(int i=0;i<4;++i) for(int j=0;j<3;++j) setInSpanningTree(s->getEdge(i,j),false); }


void Triangulation::clearDualSpanningTree() { for( auto s : simplices_ ) for(int i=0;i<4;++i) setInDualSpanningTree(s->getEdge(i,0),false); }	

bool Triangulation::isSpanningTree() {
    for(int i=0;i<numSimplices();++i) {
        for(int j=0;j<4;++j) {
            for(int k=0;k<3;++k) {
                if( isInSpanningTree(getSimplex(i)->getEdge(j,k)) != isInSpanningTree(getSimplex(i)->getEdge(j,k).getAdjacent2D()) 
                    || isInSpanningTree(getSimplex(i)->getEdge(j,k)) != isInSpanningTree(getSimplex(i)->getEdge(j,k).getAdjacent3D()) ) {
                    std::cout << "span inconsistent\n";
                    return false;
                }
            }
        }
    }
    
    if( !spanningTreeAgrees() ) return false;        

//        if( !spanningTreeAgrees() ) return false; // why twice??

    std::vector<bool> visited(12*numSimplices(),false);
    
    std::queue<Handle> queue;
    Handle start = getSimplex(0)->getEdge(0,0);

    visitVertexHandles(start,[&visited](Handle h)->bool{  visited[h.uniqueId()] = true; return false; });

    visitVertexEdges(start,[&queue](Handle h)->bool{ if( isInSpanningTree(h) ) queue.push( h ); return false; });
    
    while(!queue.empty()) {
        Handle nbr = queue.front().getAdjacent2D();
        queue.pop();

        if( !isInSpanningTree(nbr) ) { std::cout << "span sym error\n"; return false; }
        
        if( visited[nbr.uniqueId()] ) { std::cout << "span loop error\n"; return false; }
        
        visitVertexHandles(nbr,[&visited](Handle h)->bool{ visited[h.uniqueId()] = true;  return false; });
                  
        visitVertexEdges(nbr,[&queue,nbr](Handle h)->bool{ if( h != nbr && isInSpanningTree(h) ) { queue.push(h); } return false; });

    }
    for( auto in : visited ) { if( !in ) {
        std::cout << "span nonspan error\n";
        
        for( auto in2 : visited ) std::cout << in2;
        
        std::cout << "\n";
        return false;
    } }
    
    return true;
}
    
    
bool Triangulation::dualSpanningTreeAgrees() {
    for(int i=0;i<numSimplices();++i) {
        Simplex * s= getSimplex(i);
        TreeNode * n = s->getNode();
        TreeNode * p = Parent(n);
        if( !p ) continue;
        bool neighbour = false;
        for(int j=0;j<4;++j) { if( s->getNeighbour(j)->getNode() == p && s->isInDualSpanningTree(j) ) { neighbour = true; break; } }
        
        if( !neighbour ) {
            std::cout << s->getId() << " " << p->getHandle().getSimplex()->getId() << "\n";
            
            for(int j=0;j<4;++j) { std::cout << s->getNeighbour(j)->getId() << ": " << s->isInDualSpanningTree(j) << ", "; }
            std::cout << "\ndual tree disagrees1!\n";
            
            return false;
        }
    }
    for(int i=0;i<numSimplices();++i) {
        Simplex * s= getSimplex(i);
        TreeNode * n = s->getNode();
        TreeNode * p = Parent(n);
        if( !p ) continue;
        for(int j=0;j<4;++j) {
            if( s->isInDualSpanningTree(j) && s->getNode() != Parent(s->getNeighbour(j)->getNode()) && Parent(s->getNode()) != s->getNeighbour(j)->getNode()  ) {
            	std::cout << "dual tree disagrees3!\n"; 
            	return false;
            }
        }
    }
    return true;
}
    
bool Triangulation::isDualSpanningTree() {
    std::vector<bool> visited(numSimplices(),false);
    std::queue<Handle> queue;
    Simplex * start = getSimplex(0);
    visited[0] = true;
    
    for(int i=0;i<4;++i) if( start->isInDualSpanningTree(i) ) queue.push( start->getEdge(i,0) );
    
    while(!queue.empty()) {
        Handle nbr = queue.front().getAdjacent3D();
        queue.pop();
        
        if( !isInDualSpanningTree(nbr) ) { std::cout << "sym err\n"; return false; }
        
        if( visited[nbr.getSimplex()->getId()] ) { std::cout << "loop err\n"; return false; }
        
        visited[nbr.getSimplex()->getId()] = true;
        
        for(int i=0;i<3;++i) {
            if( isInDualSpanningTree(nbr.getAdjacent2D()) ) { queue.push(nbr.getAdjacent2D()); }
            
            nbr = nbr.getNext();
        }
    }
    for( auto in : visited ) { if( !in ) { std::cout << "nonspan err\n"; return false; } }

    for(int i=0;i<numSimplices();++i) {
        Simplex * s= getSimplex(i);
        TreeNode * n = s->getNode();
        
        if( !SameTree(n,getSimplex(0)->getNode()) ) { std::cout << "not one tree\n"; return false; }
        
    }
    if( !dualSpanningTreeAgrees() ) return false;

    return true;
}
    
    
bool Triangulation::spanningTreeAgrees() {
    for(auto n : nodes_[0] ) {
        TreeNode * p = Parent(n);

        if( !p ) continue;
        
        if( p->getDimension() != 1 ) { std::cout << "vertex not edge nbr: " << p->getDimension() << "\n"; return false; }

        if( p->getHandle().getVertexNode() != n && p->getHandle().getAdjacent2D().getVertexNode() != n ) { std::cout << "vertex wrong edge\n"; return false; }

        if( !isInSpanningTree(p->getHandle()) ) { std::cout << "edge not in spanning\n"; return false; }

    }
    return true;
}



#endif
