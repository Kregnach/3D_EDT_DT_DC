#ifndef TRIANGULATION_IOSTREAM_H
#define TRIANGULATION_IOSTREAM_H


#include "triangulation.h"

void Triangulation::saveToStream(std::ostream & stream) {
        stream << numSimplices() << " " << numVertices() << "\n";
        stream << hasSpanningTree() << " " << hasDualSpanningTree() << "\n";
        
        for(auto s : simplices_) {
            for(int i=0;i<4;++i) {
                stream << s->getVertexNode(i)->getId() << " " << s->getNeighbour(i)->getId() << " " << s->getEdge(i,0).getAdjacent3D().getFace() << " " << s->getEdge(i,0).getAdjacent3D().getEdge() << "\n";
            }
        }
        if( hasSpanningTree() ) {
            stream << numVertices()-1 << "\n";
            
            // make sure the root is a vertex
            dtree::WithEvert<TreeCount>::Evert(nodes_[0][0]);
            
            for(auto n : nodes_[0]) {
                if( !Parent(n) ) continue;
                
                TreeNode * edge = Parent(n);
                TreeNode * othervertex = Parent(edge);
                stream << n->getId() << " " << othervertex->getId() << " " << edge->getHandle().getSimplex()->getId() << " " << edge->getHandle().getFace() << " " << edge->getHandle().getEdge() << "\n";
            }
        }
        if( hasDualSpanningTree() ) {
            stream << numSimplices()-1 << "\n";
            for(auto s : simplices_) {
                if( Parent(s->getNode()) ) {
                    Simplex * nbr = Parent(s->getNode())->getHandle().getSimplex();
                    int face=-1;
                    for(int j=0;j<4;++j) { if( isInDualSpanningTree( s->getEdge(j,0) ) && s->getNeighbour(j) == nbr ) { face = j; break; } }
                    
                    stream << s->getId() << " " << nbr->getId() << " " << face << "\n";
                }
            }
        }
        if( hasSpanningTree() && hasDualSpanningTree() ) {
            int numlinks=0;
            for( auto n : nodes_[2] ) {
                if( isInDualSpanningTree(n->getHandle()) ) continue;
                
                Handle current = n->getHandle();
                do {
                    if( !isInSpanningTree(current) ) { numlinks++; }
                    current = current.getNext();
                } while( current != n->getHandle() );
            }
            stream << numlinks << "\n";
            for( auto n : nodes_[2] ) {
                if( isInDualSpanningTree(n->getHandle()) ) continue;
                
                Handle current = n->getHandle();
                do {
                    if( !isInSpanningTree(current) ) {
                        stream << n->getId() << " " << current.getEdgeNode()->getId() << "\n"; }
                    current = current.getNext();
                } while( current != n->getHandle() );
            }
        }
    }
    
    
bool Triangulation::loadFromStream(std::istream & stream) {
        clear();
        
        InitInactiveGraphNodes();
        
        int nsimp,nvert;
        stream >> nsimp >> nvert;
        bool spanning, dualspanning;
        stream >> spanning >> dualspanning;

        if( hasSpanningTree() && !spanning ) return false;

        if( hasDualSpanningTree() && !dualspanning ) return false;

        while( numSimplices() < nsimp ) newSimplex();

        while( numVertices() < nvert ) newNode(0);

        for(auto s : simplices_) {
            for(int i=0;i<4;++i) {
                int v,nbr,face,edge;
                stream >> v >> nbr >> face >> edge;
                s->setVertexNode(i,getNode(0,v));
                getNode(0,v)->setHandle(s->getVertexHandle(i));
                s->setNeighbour(getSimplex(nbr), i, 0, face, edge );
            }
        }
        initializeEdgeNodes();
        initializeFaceNodes();
               
        /*if( !checkNodes() || !isManifold() )
        {
            std::cout << "Error: not a manifold\n";
            return false;
        }*/
        if( spanning ) {
            int numlink;
            stream >> numlink;
            for(int i=0;i<numlink;++i) {
                int v, nbr, simp, face, edge;
                stream >> v >> nbr >> simp >> face >> edge;
                if( hasSpanningTree() ) {
                    Handle h = getSimplex(simp)->getEdge(face,edge);
                    assert( (h.getVertexNode()->getId() == v && h.getAdjacent2D().getVertexNode()->getId() == nbr) ||
                        (h.getVertexNode()->getId() == nbr && h.getAdjacent2D().getVertexNode()->getId() == v)
                    );
                    setInSpanningTree( getSimplex(simp)->getEdge(face,edge) );
                }
            }
        }
        if( dualspanning ) {
            int numlink;
            stream >> numlink;
            for(int i=0;i<numlink;++i) {
                int simp, nbr, face;
                stream >> simp >> nbr >> face;
                if( hasDualSpanningTree() ) {
                    assert( getSimplex(simp)->getNeighbour(face)->getId() == nbr );
                    setInDualSpanningTree( getSimplex(simp)->getEdge(face,0) );
                }
            }
        }
		
		
		if( hasMiddleGraph() ) { initializeMiddleGraph(); }

		if( hasSpanningTree() && !isSpanningTree() ) { std::cout << "Error: not a spanning tree\n"; return false; }

		if( hasDualSpanningTree() && !isDualSpanningTree() ) { std::cout << "Error: not a dual spanning tree\n"; return false; }
		
		setInitCoords();
		
		isManifold();
		NodeCheck();
		CheckRotationAroundEdges();
		checkIsInAnyTree();
		checkNodes();
		

		return true;
    }


#endif
