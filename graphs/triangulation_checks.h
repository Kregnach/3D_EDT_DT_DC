#ifndef TRIANGULATION_CHECKS_H
#define TRIANGULATION_CHECKS_H

#include "triangulation.h"



bool Triangulation::isManifold() {
	for(auto s : simplices_ ) {
		for(int i=0;i<4;++i) {
		    for(int j=0;j<3;++j) {

		        if( s->getEdge(i,j).getAdjacent3D().getSimplex()->getId() < 0 || s->getEdge(i,j).getAdjacent3D().getSimplex()->getId() >= numSimplices() ) { std::cout << "invalid adj\n"; return false; }
		        
		        if( s->getEdge(i,j).getAdjacent3D().getAdjacent3D() != s->getEdge(i,j) ) { std::cout << "adjacency fail\n"; return false; }
		        
		        if( !allowSelfAdjacency_ && s->getEdge(i,j).getAdjacent3D().getSimplex() == s ) { std::cout << "self-adjacency fail\n"; return false; }
		    }
		}
	}
	for(auto s : simplices_ ) {
		for(int i=0;i<4;++i) {
		    for(int j=0;j<3;++j) {
		        int eulerCharacteristic = vertexEdgeDegree(s->getEdge(i,j)) - vertexCornerDegree(s->getEdge(i,j))/2;
		        
		        if( eulerCharacteristic != 2 ) { std::cout << "euler characteristic fail: " << eulerCharacteristic << "\n"; return false; }
		    }
		}
	}
return true;
}// END isManifold


bool Triangulation::checkNodes() {
	assert( simplices_.size() == nodes_[3].size() );
	assert( nodes_[0].size() + nodes_[2].size() == nodes_[1].size() + nodes_[3].size() );
	for( auto n : nodes_[0] ) {
		assert( Root(n) ); 
		//printf("n->getDim = %d\n",n->getDimension());
		assert( n->getDimension() == 0 );
		
		
		if(n->getHandle().getVertexNode() != n) {
		    printf("NOT EQUAL: %d\t%d\t%d\t%d\t\n",n->getHandle().getVertexNode()->getId(),n->getHandle().getSimplex()->getId(),
		    n->getHandle().getFace(),
		    n->getHandle().getEdge()
		    );

		    assert(1 == 0);
		}
		//assert( n->getHandle().getVertexNode() == n );
		
		
		
		TreeNode * p = Parent(n);
		if( p ) {
			//printf("p->getDim = %d\n",p->getDimension());
		    assert( p->getDimension() == 1 );
		    assert( nodes_[1][p->getId()] == p );
		} 
	}
	for( auto n : nodes_[1] ) {
		assert( Root(n) ); 
		if( n->getHandle().getEdgeNode() != n ) {
		    std::cout << "edge node handle\n";
		    std::cout << n->getId() << ": ";
		    Handle current = n->getHandle();
		    do {
		        std::cout << "(" << current.getSimplex()->getId() << "," << current.getFace() << "," << current.getEdge() << ")-" << current.getEdgeNode()->getId()<< " " ;
		        current = current.getRotationAroundEdge();
		    } while( current != n->getHandle() );
		    std::cout << "\n";
		    return false;
		}
		TreeNode * p = Parent(n);
		if( p ) {
		    assert( p->getDimension() == 2  || p->getDimension() == 0 );
		    assert( nodes_[p->getDimension()][p->getId()] == p );
		} 
	}
	for( auto n : nodes_[2] ) {
		assert( Root(n) ); 
		if( n->getHandle().getFaceNode() != n ) {
		    std::cout << n << " " << n->getHandle().getFaceNode() << "\n";
		    std::cout << "node2 " <<  n->getId() << "\n";
		    std::cout << "node2 " <<  n->getId() << " " << n->getHandle().getFaceNode()->getId() << "\n";
		    assert(false);
		}
		TreeNode * p = Parent(n);
		if( p ) {
		    assert( p->getDimension() == 1 );
		    assert( nodes_[1][p->getId()] == p );
		} 
	}
	for( auto n : nodes_[3] ) {
		assert( Root(n) ); 
		assert( n->getHandle().getSimplexNode() == n );
		TreeNode * p = Parent(n);
		if( p ) {
		    assert( p->getDimension() == 3 );
		    assert( nodes_[3][p->getId()] == p );
		} 
}
int stop = 0;
for( auto s : simplices_ ) {
    for(int i=0;i<4;++i) {
        for(int j=0;j<3;++j) {
            if( !s->getEdge(i,j).getVertexNode() || !s->getEdge(i,j).getEdgeNode() || !s->getEdge(i,j).getFaceNode() ) { std::cout << "node missing\n"; return false; }
            
            if( s->getEdge(i,j).getVertexNode() != s->getEdge(i,j).getAdjacent3D().getAdjacent2D().getVertexNode() ||
                s->getEdge(i,j).getVertexNode() != s->getEdge(i,j).getAdjacent2D().getNext().getVertexNode() ) {
                std::cout << "vertex node error\n";
                return false;
            }
            
            if( s->getEdge(i,j).getEdgeNode() != s->getEdge(i,j).getAdjacent2D().getEdgeNode() ||
                s->getEdge(i,j).getEdgeNode() != s->getEdge(i,j).getAdjacent3D().getEdgeNode() ) {
                	
                printf("%d\t%d\n",s->getEdge(i,j).getEdgeNode() == s->getEdge(i,j).getAdjacent2D().getEdgeNode(),s->getEdge(i,j).getEdgeNode() == s->getEdge(i,j).getAdjacent3D().getEdgeNode());
                std::cout << "edge node error\n";
                assert(stop == 1);
                return false;
            }
            
            if( s->getEdge(i,j).getFaceNode() != s->getEdge(i,j).getAdjacent3D().getFaceNode() ) { std::cout << "face node error\n"; return false; }
        }
    }
}
return true;
} // END checkNodes


void Triangulation:: CheckRotationAroundEdges() {
	int counter, nodecounter = 0,here;
	
	for ( auto n: nodes_[1]) {
		counter = 0;
		nodecounter++;
		Handle edge = n->getHandle();
		Handle current = edge;
		do{ current = current.getRotationAroundEdge();
			counter++;
			if(counter == 100000) goto here; // 100k is large enough typically
		 } while( current != edge);
	} 
	
	here:
	
	assert(nodecounter == nodes_[1].size());
}	


bool Triangulation:: CheckEdgeCoord() {

	int max_counter = 0;
	int max_written = 0;
	
	for( auto n : nodes_[1] ) {
		Handle current = n->getHandle();
		int coord = n->getCoord(); 
		int counter = 0;
		//printf("Node Id: %d coord : %d\t%d\n",n->getId(),coord);
		do{
			current = current.getRotationAroundEdge();
			
			//printf("rot ID: %d\t%d\n",current.getEdgeNode()->getId(),current.getFaceNode()->getId());
			counter++;
		} while( current != n->getHandle());
		
		if (counter > max_counter) max_counter = counter;
		if (coord > max_written) max_written = counter;
		
		assert(coord == counter);
		assert(coord <= numSimplices());
		printf("Node coord check: %d\t%d\t%d\t%d\n",n->getId(),coord,counter,numSimplices());
	}
	
	printf("MAX values are: %d\t%d\n",max_counter,max_written);
	
	return true;
}

bool Triangulation:: CheckSingeEdgeCoord(Handle n) {
	Handle current = n;
	int coord = n.getEdgeNode()->getCoord(); 
	int counter = 0;
		//printf("Node Id: %d coord : %d\t%d\n",n->getId(),coord);
	do{
		current = current.getRotationAroundEdge();
			
		//printf("rot ID: %d\t%d\n",current.getEdgeNode()->getId(),current.getFaceNode()->getId());
		counter++;
	} while( current != n);
		
	printf("Node coord check %d: %d\t%d\n",n.getEdgeNode()->getId(),coord,counter);
	
	if(coord == counter) return true; 
	
	return false;
}
	
	

int Triangulation:: checkIsInAnyTree() {
	for(auto n: nodes_[1]) { 
		Handle current = n->getHandle();
		if(!isInSpanningTree(current)) assert(n->getInMiddleGraph());
	}
	for(auto n: nodes_[2]) { 
		Handle current = n->getHandle();
		if(!isInDualSpanningTree(current)) assert(n->getInMiddleGraph());
	}
	
	return 1;
}

bool Triangulation:: NodeCheck() { for(int i = 0 ; i < 4 ; i++) for( auto n : nodes_[i] ) { assert( Parent(n) || Root(n)==n ); } return true; }


void Triangulation:: PrintRotationAroundEdge(Handle handle) {
	Handle current = handle;
	printf("Rotate Around:\nedge0: %d\t Face: %d\t edgeN: %d\t edgeP: %d\n",current.getEdgeNode()->getId(),current.getFaceNode()->getId(),current.getNext().getEdgeNode()->getId(),current.getPrevious().getEdgeNode()->getId());

	do{
		current = current.getRotationAroundEdge();
		
		printf("edge0: %d\t Face: %d\t edgeN: %d\t edgeP: %d\n",current.getEdgeNode()->getId(),current.getFaceNode()->getId(),current.getNext().getEdgeNode()->getId(),current.getPrevious().getEdgeNode()->getId());
	} while(current !=  handle);

	printf("\n\n");
}


#endif
