#ifndef TRIANGULATION_INICIALIZE_H
#define TRIANGULATION_INICIALIZE_H

void Triangulation::InitInactiveGraphNodes() { for(int i = 0; i < ABSMAX_DC ; i++) inactive_graph_nodes_[i] = i; }

void Triangulation::initializeMiddleGraph() {
    PrintMiddleGraph();
     
    for(auto s : simplices_ ) {
    	for(int i=0;i<4;++i) {
    		if( s->isInDualSpanningTree(i) ) continue;
    		for(int j=0;j<3;++j) {
                if( s->isInSpanningTree(i,j) ) continue;
				
				printf("EDGE ID: %d\t%d\n",s->getEdge(i,j).getFaceNode()->getGraphId(),s->getEdge(i,j).getEdgeNode()->getGraphId());
    			setHandleInMiddleGraph(s->getEdge(i,j));
    		}
        }
    }
	PrintMiddleGraph();
}

void Triangulation:: setInitCoords() {
	for ( auto s: simplices_) {

		//printf("START\n");

		IncrementCoord(s->getVertexNode(0),0);
		IncrementCoord(s->getVertexNode(1),0);
		IncrementCoord(s->getVertexNode(2),0);
		IncrementCoord(s->getVertexNode(3),0);
		
		
		IncrementCoord(s->getEdgeNode(0),1);
		IncrementCoord(s->getEdgeNode(1),1);
		IncrementCoord(s->getEdgeNode(2),1);
		IncrementCoord(s->getEdgeNode(3),1);
		IncrementCoord(s->getEdgeNode(4),1);
		IncrementCoord(s->getEdgeNode(5),1);

	}
}


void Triangulation::initializeCoordinationNumbers() {
	
	for(int i = 0; i < nodes_[0].size(); i++) nodes_[0][i]->setCoord();
	for(int i = 0; i < nodes_[1].size(); i++) nodes_[1][i]->setCoord();
	for(int i = 0; i < nodes_[2].size(); i++) nodes_[2][i]->setCoord();
	for(int i = 0; i < nodes_[3].size(); i++) nodes_[3][i]->setCoord();
	
	for(int i = 0; i < nodes_[0].size(); i++) {
		IncrementCoord(nodes_[0][i],0);
		IncrementCoord(nodes_[0][i],0);
	}
	
	for(int i = 0; i < nodes_[1].size(); i++) {
		IncrementCoord(nodes_[1][i],1);
		IncrementCoord(nodes_[1][i],1);
	}

}

void Triangulation::initializeVertexNodes() {
        // assume that no vertex nodes exist
        for( auto s : simplices_ ) for(int i=0;i<4;++i) s->setVertexNode(i,0);

        for( auto s : simplices_ ) {
            for(int i=0;i<4;++i) {
                if( s->getVertexNode(i) ) continue;
                
                TreeNode * node = newNode(0);
                setVertexNode(s->getVertexHandle(i),node);
            }
        }
    }
    
    

    
void Triangulation::initializeEdgeNodes() {
        // assume that no edge nodes exist
	for( auto s : simplices_ ) { for(int i=0;i<6;++i) { s->setEdgeNode(i,0); } }
	for( auto s : simplices_ ) {
		for(int i=0;i<6;++i) { 

			if( s->getEdgeNode(i) ) continue;

			TreeNode * node = newNode(1);
			setEdgeNode(s->getEdgeHandle(i),node);
		}
	}
}
    
void Triangulation::initializeFaceNodes() { // assume that no face nodes exist
	for( auto s : simplices_ ) for(int i=0;i<4;++i)  s->setFaceNode(i,0); 

	for( auto s : simplices_ ) {
		for(int i=0;i<4;++i) {
		if( s->getFaceNode(i) ) continue;

			TreeNode * node = newNode(2);
			setFaceNode(s->getEdge(i,0),node);
		}
	}
	printf("\n\n");
}

void Triangulation::initializeMinimal() {

	clear();

	InitInactiveGraphNodes();

	Simplex * simplex1 = newSimplex();
	Simplex * simplex2 = newSimplex();
	Handle h1 = simplex1->getEdge(0,0);
	Handle h2 = simplex2->getEdge(1,2);
	setAdjacent( h1, h2 );
	h1 = h1.getAdjacent2D().getNext();
	h2 = h2.getAdjacent2D().getPrevious();
	setAdjacent( h1, h2 );
	h1 = h1.getAdjacent2D().getNext();
	h2 = h2.getAdjacent2D().getPrevious();
	setAdjacent( h1, h2 ); 
	h1 = h1.getNext().getAdjacent2D();
	h2 = h2.getPrevious().getAdjacent2D();
	setAdjacent( h1, h2 );

	initializeVertexNodes();

	initializeEdgeNodes();
	initializeFaceNodes();

	if( spanning_tree_ ) {
		for(int i=0;i<3;++i) {
			setInSpanningTree(h1);
			h1 = h1.getAdjacent2D().getNext();
		}
	}

	if( dual_spanning_tree_ ) setInDualSpanningTree(h1);

	initializeCoordinationNumbers();

	printf("THERE IS SPANNING TREE? : %d\t%d\n",dual_spanning_tree_,spanning_tree_);

	if( dual_spanning_tree_ ) std::cout << "dual span: " << ( isDualSpanningTree() ? "good" : "wrong" ) << "\n";
	if( spanning_tree_ ) std::cout << "span: " << ( isSpanningTree() ? "good" : "wrong" ) << "\n";
	
	if( middle_graph_ ) initializeMiddleGraph();
	
	
	printf("PERFORM EVERY SINGLE CHECK POSSIBLE: [    ] \n");
	doAllChecks();
	printf("PERFORM EVERY SINGLE CHECK POSSIBLE: [ OK ]\n");
	
} // END initializeMinimal()














#endif
