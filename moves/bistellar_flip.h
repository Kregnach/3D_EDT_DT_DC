#ifndef BISTELLAR_FLIP_H
#define BISTELLAR_FLIP_H


template<typename RNG> bool Triangulation::TryGraphMove23(RNG & rng) {
		proposed_rate[2]++;
		Handle interface = getRandomEdge(rng);
		
		double SA = getActionNow(); 

		// move does not make sense if the interface triangle is adjacent to the same simplex on both sides
		if( allowsSelfAdjacency() && interface.getAdjacent3D().getSimplex() == interface.getSimplex() ) return false;
		
		if( !allowsSelfAdjacency() ) {
		    Handle rootA = interface;
		    Handle rootB = interface.getAdjacent3D().getNext();
		    std::vector<Handle> nbr{
		        rootA.getNext().getAdjacent2D(),
		        rootA.getPrevious().getAdjacent2D(),
		        rootB.getNext().getAdjacent2D(),
		        rootB.getPrevious().getAdjacent2D()}; 
		        
		    //  check that move will not lead to self-adjacency
		    if( sameFace(rootA.getAdjacent2D().getAdjacent3D(), nbr[3]) 
		        || sameFace(nbr[0].getAdjacent3D(), nbr[2])
		        || sameFace(nbr[1].getAdjacent3D(), rootB.getAdjacent2D()) )
		        return false;
		}
		
		if( !allowsDegeneracy() ) {
		    TreeNode * vertex1 = interface.getPrevious().getAdjacent2D().getPrevious().getVertexNode();
		    TreeNode * vertex2 = interface.getAdjacent3D().getAdjacent2D().getPrevious().getVertexNode();
		    
		    if( vertex1 == vertex2 ) return false;
		}
		
		// the move is ok from the point of view of the
		
		SpanningMove dual_move{0,0,0,0};
		//if( hasDualSpanningTree() ) {
	    bool allowed;
	    std::tie( allowed, dual_move) = getDualSpanningTreeMove23Id(interface, rng, true);
	    if( !allowed ) return false;
	    // if there are more returnoptions then we should only accept this move with probability options/returnoptions
	    if( dual_move.return_options > dual_move.options && !random_bernoulli(rng,((double)dual_move.options)/dual_move.return_options ) ) {
	        // reset links
	        setInDualSpanningTreeAfterMove32(interface, dual_move.id_before);
	        //assert(isDualSpanningTree());
	        return false;
	    }

		//double dual_tree_rng = uniform_real(rng);
		//double tree_rng = uniform_real(rng);
		

		//std::pair<Handle, int> IDM23 = Identify_move23(interface, dual_tree_rng, tree_rng);
		
		//if(IDM23.second == false) return false;
		
		clearGraphFaceLinksBeforeMove23(interface);
		
		int wb = 0;
		
		int c1 = interface.getAdjacent2D().getNext().getEdgeNode()->getCoord();
		int c2 = interface.getNext().getAdjacent2D().getNext().getEdgeNode()->getCoord();
		int c3 = interface.getPrevious().getAdjacent2D().getNext().getEdgeNode()->getCoord();
		
		int c4 = interface.getAdjacent3D().getAdjacent2D().getNext().getEdgeNode()->getCoord();
		int c5 = interface.getAdjacent3D().getNext().getAdjacent2D().getNext().getEdgeNode()->getCoord();
		int c6 = interface.getAdjacent3D().getPrevious().getAdjacent2D().getNext().getEdgeNode()->getCoord();
		
		int c01 = interface.getEdgeNode()->getCoord();
		int c02 = interface.getNext().getEdgeNode()->getCoord();
		int c03 = interface.getPrevious().getEdgeNode()->getCoord();
		
		
		(c1 == 2) ? wb++ : (c1 == 3) ? wb-- : wb = wb;
		(c2 == 2) ? wb++ : (c2 == 3) ? wb-- : wb = wb;
		(c3 == 2) ? wb++ : (c3 == 3) ? wb-- : wb = wb;
		(c4 == 2) ? wb++ : (c4 == 3) ? wb-- : wb = wb;
		(c5 == 2) ? wb++ : (c5 == 3) ? wb-- : wb = wb;
		(c6 == 2) ? wb++ : (c6 == 3) ? wb-- : wb = wb;
		
		(c01 == 3) ? wb-- : (c01 == 4) ? wb++ : wb = wb;
		(c02 == 3) ? wb-- : (c02 == 4) ? wb++ : wb = wb;
		(c03 == 3) ? wb-- : (c03 == 4) ? wb++ : wb = wb;
		

		double prob = ((double)numSimplices()*2 )/((double)(GetCoord3Size() + wb + 1));
		
		Handle diagonal = diagonalSubdivide(interface);

		setInDualSpanningTreeAfterMove23(diagonal, dual_move.id_after); //if( hasDualSpanningTree() )
		
		setGraphFaceLinksAfterMove23(diagonal);
		
		double SB = getActionNow(); 
		double moveprob = uniform_real(rng);

		if( !getProbabilityNow(moveprob,SA,SB,prob) ) {
			
			unsetGraphFaceLinksAfterMove23(diagonal);
		    clearLinksBeforeMove32(diagonal);
		    Handle interface2 = unDiagonalSubdivide(diagonal);
		    setInDualSpanningTreeAfterMove32(interface2, dual_move.id_before);
		    
		    bool success = setGraphFaceLinksAfterMove32(interface2);
		    
		    assert(success);
		    
		    return false;
		}		
		//printf("RETURned TRUE!!!\n");
		
		
		//PrintGraphIndAf();
		//PrintMiddleGraph();

	accepted_rate[2]++;
		return true;

	}
	
	Handle Triangulation::Rotate_Interface(Handle interface) {
	
		Handle newInterface;
			
		int int_ct = isInDualSpanningTree(interface);
		int inlc_l1 = isInSpanningTree(interface);
		int inlc_l2 = isInSpanningTree(interface.getNext());
		int inlc_l3 = isInSpanningTree(interface.getPrevious());
		
		int inll_l1 = isInSpanningTree(interface.getAdjacent2D().getNext());
		int inll_l2 = isInSpanningTree(interface.getAdjacent2D().getPrevious());
		int inll_l3 = isInSpanningTree(interface.getNext().getAdjacent2D().getPrevious());
		
		int inlr_l1 = isInSpanningTree(interface.getAdjacent3D().getAdjacent2D().getPrevious());
		int inlr_l2 = isInSpanningTree(interface.getAdjacent3D().getAdjacent2D().getNext());
		int inlr_l3 = isInSpanningTree(interface.getAdjacent3D().getNext().getAdjacent2D().getPrevious());
		
		int lweightnum = inll_l1+inll_l2+inll_l3;
		int rweightnum = inlr_l1+inlr_l2+inlr_l3;
		int cweightnum = inlc_l1+inlc_l2+inlc_l3;
		int fweight = lweightnum + rweightnum + cweightnum;
		
		if(rweightnum > lweightnum) {
			newInterface = interface.getAdjacent3D();

			inlc_l1 = isInSpanningTree(newInterface);
			inlc_l2 = isInSpanningTree(newInterface.getNext());
			inlc_l3 = isInSpanningTree(newInterface.getPrevious());
			
			inll_l1 = isInSpanningTree(newInterface.getAdjacent2D().getNext());
			inll_l2 = isInSpanningTree(newInterface.getAdjacent2D().getPrevious());
			inll_l3 = isInSpanningTree(newInterface.getNext().getAdjacent2D().getPrevious());
			
			inlr_l1 = isInSpanningTree(newInterface.getAdjacent3D().getAdjacent2D().getPrevious());
			inlr_l2 = isInSpanningTree(newInterface.getAdjacent3D().getAdjacent2D().getNext());
			inlr_l3 = isInSpanningTree(newInterface.getAdjacent3D().getNext().getAdjacent2D().getPrevious());	
		}
		
		int lweight = 100*inll_l1+10*inll_l2+inll_l3;
		int rweight = 100*inlr_l1+10*inlr_l2+inlr_l3;
		int cweight = 100*inlc_l1+10*inlc_l2+inlc_l3;
		
		if(cweight == 1 || cweight == 101) newInterface = newInterface.getPrevious();
		
		else if (cweight == 10 || cweight == 11) newInterface = newInterface.getNext(); 
		
		return newInterface;
	}

	std::pair<Handle, int> Triangulation::Identify_move23(Handle interface, double dual_tree_rng, double tree_rng) { 

	std::pair<Handle, int> RESULT;
	
	
	int int_ct = isInDualSpanningTree(interface);
	int inlc_l1 = isInSpanningTree(interface);
	int inlc_l2 = isInSpanningTree(interface.getNext());
	int inlc_l3 = isInSpanningTree(interface.getPrevious());
	
	int inll_l1 = isInSpanningTree(interface.getAdjacent2D().getNext());
	int inll_l2 = isInSpanningTree(interface.getAdjacent2D().getPrevious());
	int inll_l3 = isInSpanningTree(interface.getNext().getAdjacent2D().getPrevious());
	
	int inlr_l1 = isInSpanningTree(interface.getAdjacent3D().getAdjacent2D().getPrevious());
	int inlr_l2 = isInSpanningTree(interface.getAdjacent3D().getAdjacent2D().getNext());
	int inlr_l3 = isInSpanningTree(interface.getAdjacent3D().getNext().getAdjacent2D().getPrevious());
	
	int lweightnum = inll_l1+inll_l2+inll_l3;
	int rweightnum = inlr_l1+inlr_l2+inlr_l3;
	int cweightnum = inlc_l1+inlc_l2+inlc_l3;
	int fweight = lweightnum + rweightnum + cweightnum;
	
	int lweight = 100*inll_l1+10*inll_l2+inll_l3;
	int rweight = 100*inlr_l1+10*inlr_l2+inlr_l3;
	int cweight = 100*inlc_l1+10*inlc_l2+inlc_l3;

	int intl_t1 = isInDualSpanningTree(interface.getAdjacent2D());
	int intl_t2 = isInDualSpanningTree(interface.getNext().getAdjacent2D());
	int intl_t3 = isInDualSpanningTree(interface.getPrevious().getAdjacent2D());

	int intr_t1 = isInDualSpanningTree(interface.getAdjacent3D().getAdjacent2D());
	int intr_t2 = isInDualSpanningTree(interface.getAdjacent3D().getPrevious().getAdjacent2D());
	int intr_t3 = isInDualSpanningTree(interface.getAdjacent3D().getNext().getAdjacent2D());
	
	if(!int_ct) { 
		if( (intl_t1 + intr_t1) == 2) return std::pair(interface,false);
		if( (intl_t2 + intr_t2) == 2) return std::pair(interface,false);
		if( (intl_t3 + intr_t3) == 2) return std::pair(interface,false);
	}
	
	
	// COMMENT : sides flipped
	// todo: rotate center OR rotate side
	
	
	
	
	/*
	if(!int_ct) {
		if(inlc_l1) DeleteEdgeMiddleGraphPure( interface );
		if(inlc_l2) DeleteEdgeMiddleGraphPure( interface.getNext() );
		if(inlc_l3) DeleteEdgeMiddleGraphPure( interface.getPrevious() );
		
	}*/
	
	
	
	
	
	
	Handle newhandle = interface;
	
	
	RESULT.first = interface;
	RESULT.second = true;
	
	return RESULT;
    }

#endif
