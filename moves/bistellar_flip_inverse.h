#ifndef BISTELLAR_FLIP_INVERSE_H
#define BISTELLAR_FLIP_INVERSE_H
	
template<typename RNG> Handle Triangulation::GetRandomCoord3(RNG & rng) {	 return coord_3_nodes_[(int)(coord_3_nodes_.size()*uniform_real(rng))]->getHandle(); }
	


template<typename RNG> bool Triangulation::TryGraphMove32(RNG & rng) {
	proposed_rate[3]++;

	Handle diagonal;

	double SA = getActionNow();
		
		//printf("Bef\n");
	if(coord_3_nodes_.size() != 0) diagonal = GetRandomCoord3(rng);
	else return false;
		
	//diagonal = getRandomEdge(rng);
	
	SpanningMove dual_move{0,0,0,0};
	
	if(isInSpanningTree(diagonal) ) return false;
		
	//if( diagonal.getRotationAroundEdge(3) != diagonal ) return false;

	// the following handles are exactly (to be) those of diagonalSubdivide
	Handle rootC = diagonal;
	Handle rootA = rootC.getPrevious().getAdjacent3D().getAdjacent2D().getPrevious();
	Handle rootB = rootC.getAdjacent2D().getAdjacent3D().getNext().getAdjacent2D().getPrevious();
	std::vector<TreeNode *> vert{
	    rootA.getAdjacent2D().getPrevious().getVertexNode(),
	    rootB.getAdjacent2D().getPrevious().getVertexNode(),
	    rootB.getVertexNode(),
	    rootB.getNext().getVertexNode(),
	    rootA.getNext().getVertexNode()
	};
	
	if( rootA.getSimplex() == rootB.getSimplex() || rootA.getSimplex() == rootC.getSimplex() || rootB.getSimplex() == rootC.getSimplex() ) {
	    return false;
	}
	
	if( !allowsSelfAdjacency() ) {
	    if( sameFace( rootA.getAdjacent3D(), rootB.getAdjacent2D() ) ||
	        sameFace( rootC.getPrevious().getAdjacent2D().getAdjacent3D(), rootB.getAdjacent2D() ) ||
	        sameFace( rootA.getAdjacent3D(), rootC.getPrevious().getAdjacent2D() ) )
	        return false;

	    if( sameFace( rootA.getAdjacent2D().getAdjacent3D(), rootB ) ||
	        sameFace( rootC.getNext().getAdjacent2D().getAdjacent3D(), rootB ) ||
	        sameFace( rootA.getAdjacent2D().getAdjacent3D(), rootC.getNext().getAdjacent2D() ) )
	        return false;
	}

	
    bool allowed;
    std::tie( allowed, dual_move) = getDualSpanningTreeMove32Id(diagonal, rng, true);
    if( !allowed ) return false;
    // if there are more returnoptions then we should only accept this move with probability options/returnoptions
    if( dual_move.return_options > dual_move.options && !random_bernoulli(rng,((double)dual_move.options)/dual_move.return_options ) ) {
        // reset links
        setInDualSpanningTreeAfterMove23(diagonal, dual_move.id_before); 
        
        return false; //assert(isDualSpanningTree());
    }
	
	clearGraphFaceLinksBeforeMove32(diagonal);

	double prob = (double)(GetCoord3Size())/ ( ((double)numSimplices()-1)*2);

	Handle interface = unDiagonalSubdivide(diagonal);
	
	setInDualSpanningTreeAfterMove32(interface, dual_move.id_after); //if( hasDualSpanningTree() ) 
	
	setGraphFaceLinksAfterMove32(interface);
	
	
	
	double SB = getActionNow(); 
	double moveprob = uniform_real(rng);
	
	if( !getProbabilityNow(moveprob,SA,SB,prob) ) {
		unsetGraphFaceLinksAfterMove32(interface);
	    clearLinksBeforeMove23(interface);
	    Handle diagonal2 = diagonalSubdivide(interface);
	    setInDualSpanningTreeAfterMove23(diagonal2, dual_move.id_before);
	    bool success = setGraphFaceLinksAfterMove23(diagonal2);
	    assert(success);
	    return false;
	}
	
	
	accepted_rate[3]++;
	return true;
}

#endif
