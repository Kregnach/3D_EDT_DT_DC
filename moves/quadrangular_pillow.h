#ifndef QUADRANGULAR_PILLOW_H
#define QUADRANGULAR_PILLOW_H

void Triangulation:: unsetGraphFaceLinksBeforeMoveEdge02(Handle h1, Handle h2) {
	unsetGraphEdgeLinks(h1);
	
	unsetGraphFaceLinks(h1);
	unsetGraphFaceLinks(h2);
}

void Triangulation:: setGraphFaceLinksAfterMoveEdge02(Handle h1) {
	setGraphFaceLinks(h1);
	setGraphFaceLinks(h1.getAdjacent2D());
	
	setGraphFaceLinks(h1.getNext().getAdjacent2D());
	setGraphFaceLinks(h1.getPrevious().getAdjacent2D());
	setGraphFaceLinks(h1.getAdjacent3D().getNext().getAdjacent2D());
	setGraphFaceLinks(h1.getAdjacent3D().getPrevious().getAdjacent2D());
	
	setGraphEdgeLinks(h1.getNext().getAdjacent2D().getPrevious());
	setGraphEdgeLinks(h1.getAdjacent3D().getNext().getAdjacent2D().getPrevious());
}

template<typename RNG> bool Triangulation::TryGraphMoveEdge02(RNG & rng) {
	proposed_rate[4]++;

	double SA = getActionNow();

	Handle handle1 = getRandomEdge(rng);  //randomly choose an edge
		
	int h_coord = handle1.getEdgeNode()->getCoord();

	int rotator_number = (int)( uniform_real(rng) * (h_coord-1) ) + 1; //random 2nd handle around the link, avoid 0 and max rotation

	Handle handle2 = handle1.getRotationAroundEdge(rotator_number).getAdjacent3D(); //avoid 0 or max rotation

	int h1in = isInDualSpanningTree( handle1 );
	int h2in = isInDualSpanningTree( handle2 );
	
		

	if( h1in && h2in ) return false;  // RETURN if both are in the dual tree, the move would break the dual tree
	
	if( handle1.getPrevious().getVertexNode()->getId() == handle2.getPrevious().getVertexNode()->getId() ) return false; // avoid ruining the manifold constraints

	std::vector<bool> treeinfo;	    // undoing to store the tree info
		treeinfo.push_back( h1in );
		treeinfo.push_back( h2in );
		treeinfo.push_back( isInSpanningTree(handle1.getNext()) );
		treeinfo.push_back( isInSpanningTree(handle1.getPrevious()) );
		treeinfo.push_back( isInSpanningTree(handle2.getNext()) );
		treeinfo.push_back( isInSpanningTree(handle2.getPrevious()) );
		treeinfo.push_back( isInSpanningTree(handle1));
	
	double rng1 = uniform_real(rng);
	double rng2 = uniform_real(rng);
	double moveprob = uniform_real(rng);
	
	int wb = 1;

	int c1 = handle1.getEdgeNode()->getCoord();
	int c2 = handle1.getNext().getEdgeNode()->getCoord();
	int c3 = handle1.getPrevious().getEdgeNode()->getCoord();
	int c4 = handle2.getNext().getEdgeNode()->getCoord();
	int c5 = handle2.getPrevious().getEdgeNode()->getCoord();
	
	if( ( (h_coord-rotator_number) == 1) || (rotator_number == 1) ) wb = 2 ; //if we rotate 1 or -1, we have one less tetrahedra, thus +1
	
	if (c2 == 2) wb-- ; //if any is of coord 2, they will increase, thus there will be less c2 edges
	if (c3 == 2) wb-- ;
	if (c4 == 2) wb-- ;
	if (c5 == 2) wb-- ; //if any is of coord 2, they will increase, thus there will be less c2 edges

	double factor = 1.0;
	
	if(isInSpanningTree(handle1)) factor = 2.0;

	if( (h1in || h2in) ) factor *= 2.0; //in the paper
	else factor *= 12.0;                   // in the paper
	
// in the paper:
	double prob = factor*(double)((6*numSimplices()*(h_coord-1)) / ((GetEdgeCoord2Size() + wb)  ));
	
	unsetGraphFaceLinksBeforeMoveEdge02(handle1,handle2);
	
	Handle newlink = expandRectangle(handle1,handle2,rng1,rng2,rotator_number);

	setGraphFaceLinksAfterMoveEdge02(newlink);

	double SB = getActionNow();
	
	
	if( !getProbabilityNow(moveprob,SA,SB,prob) ) {

		unsetGraphFaceLinksBeforeMoveEdge20(newlink);

		handle1 = unexpandRectangle(newlink, -1);
		handle2 = handle1.getRotationAroundEdge(rotator_number).getAdjacent3D(); //avoid 0 or max rotation

		setInDualSpanningTree( handle1, treeinfo[0] );
		setInDualSpanningTree( handle2, treeinfo[1] );
		
		setInSpanningTree( handle1.getNext(), treeinfo[2] );
		setInSpanningTree( handle1.getPrevious(), treeinfo[3] );
		setInSpanningTree( handle2.getNext(), treeinfo[4] );
		setInSpanningTree( handle2.getPrevious(), treeinfo[5] );
		setInSpanningTree( handle1, treeinfo[6] );

		setGraphFaceLinks(handle1);
		setGraphFaceLinks(handle2);
		setGraphEdgeLinks(handle1);
		
		return false;
	}
	
	accepted_rate[4]++;
	return true; 
}

Handle Triangulation::expandRectangle(Handle handle1, Handle handle2,double rng1,double rng2, int rots) {
	int t1_in = isInDualSpanningTree(handle1); // CHECK FOR SPANNING TREE
	int t2_in = isInDualSpanningTree(handle2);
	
	
	Handle l1n = handle1.getNext();
	Handle l1p = handle1.getPrevious();
	Handle l2n = handle2.getNext();
	Handle l2p = handle2.getPrevious();

	int l0_in = isInSpanningTree(handle1);
	int l1n_in = isInSpanningTree(l1n);
	int l1p_in = isInSpanningTree(l1p);
	int l2n_in = isInSpanningTree(l2n);
	int l2p_in = isInSpanningTree(l2p);  // CHECK FOR SPANNING TREE
	
	if(t1_in) setInDualSpanningTree( handle1, false );  //REMOVE DUAL TREE
	else if(t2_in) setInDualSpanningTree( handle2, false ); //REMOVE DUAL TREE

	if(l0_in)   setInSpanningTree( handle1, false ); //REMOVE SPANNING TREE
	if(l1n_in)  setInSpanningTree( l1n, false );
	if(l1p_in)  setInSpanningTree( l1p, false );
	if(l2n_in)  setInSpanningTree( l2n, false );
	if(l2p_in)  setInSpanningTree( l2p, false );  //REMOVE SPANNING TREE
	
    Handle handle1_nbr = handle1.getAdjacent3D(); // the other side of the handle1
    Handle handle2_nbr = handle2.getAdjacent3D(); // the other side of the handle2		

	Simplex * s1 = newSimplex(); // create the two new simplices
    Simplex * s2 = newSimplex(); // create the two new simplices		

	TreeNode * newlink = newNode(1); // create a new link to cross 
	TreeNode * newedge = newNode(1); // create a new link that is a "copy" of the handle between the nbr triangles
	TreeNode * newface1 = newNode(2); // 4 new faces are created
	TreeNode * newface2 = newNode(2); // 4 new faces are created
	TreeNode * newface3 = newNode(2); // 4 new faces are created
	TreeNode * newface4 = newNode(2); // 4 new faces are created

	// ###################### ADJACENT Simplices #########################
	
	auto s1_ge00 = s1->getEdge(0,0);
	auto s2_ge00 = s2->getEdge(0,0);
	
	setAdjacent( s1_ge00 , handle1     ); //s1 1/4
	setAdjacent( s2_ge00 , handle1_nbr ); //s2 1/4

	setAdjacent( 
		s1_ge00.getNext().getAdjacent2D() ,
		s2_ge00.getPrevious().getAdjacent2D() );  //s1 3/4 s2 3/4

	setAdjacent( 
		s1_ge00.getPrevious().getAdjacent2D() , 
		s2_ge00.getNext().getAdjacent2D() );  //s1 4/4 s2 4/4
		
	setAdjacent( s1_ge00.getAdjacent2D() , handle2     ); //s1 2/4
	setAdjacent( s2_ge00.getAdjacent2D() , handle2_nbr );  //s2 2/4

	// ###################### Assign vertices #########################

	setVertexNode(s1_ge00 , s1_ge00.getAdjacent3D().getNext().getVertexNode()); //s1 1/4
	setVertexNode(s2_ge00 , s2_ge00.getAdjacent3D().getNext().getVertexNode()); //s2 1/4

	setVertexNode(s1_ge00.getNext() , 
		s1_ge00.getAdjacent3D().getVertexNode()); //s1 2/4
	setVertexNode(s2_ge00.getNext() , 
		s2_ge00.getAdjacent3D().getVertexNode()); //s2 2/4
	setVertexNode(s1_ge00.getPrevious(), 
		s1_ge00.getAdjacent3D().getPrevious().getVertexNode()); //s1 3/4
	setVertexNode(s2_ge00.getPrevious(), 
		s2_ge00.getAdjacent3D().getPrevious().getVertexNode()); //s2 3/4
	setVertexNode(s1_ge00.getAdjacent2D().getPrevious() , 
		s1_ge00.getAdjacent2D().getAdjacent3D().getPrevious().getVertexNode()); //s1 4/4
	setVertexNode(s2_ge00.getAdjacent2D().getPrevious() , 
		s2_ge00.getAdjacent2D().getAdjacent3D().getPrevious().getVertexNode()); //s2 4/4

	// ###################### Assign edges #########################

	setEdgeNode(s1_ge00 , s1_ge00.getAdjacent3D().getEdgeNode()); //); //s2 1/6
	setEdgeNode(s2_ge00 , newedge); //s2 1/6
	
	//printf("###### st Setting up ###### \n");

	setEdgeNode(s1_ge00.getNext() , 
		s1_ge00.getAdjacent3D().getPrevious().getEdgeNode() ); // ); // s1 2/6

	setEdgeNode(s1_ge00.getPrevious() , 
		s1_ge00.getAdjacent3D().getNext().getEdgeNode() ); // s1 3/6
	
	setEdgeNode(s1_ge00.getAdjacent2D().getPrevious() , 
		s1_ge00.getAdjacent2D().getAdjacent3D().getNext().getEdgeNode() ); //, ); // s1 4/6
	setEdgeNode(s1_ge00.getAdjacent2D().getNext() , 
		s1_ge00.getAdjacent2D().getAdjacent3D().getPrevious().getEdgeNode()); //s1 5/6

	setEdgeNode(s2_ge00.getNext() , s2_ge00.getAdjacent3D().getPrevious().getEdgeNode()); // s2 2/6
	setEdgeNode(s2_ge00.getPrevious() , s2_ge00.getAdjacent3D().getNext().getEdgeNode()); //s2 3/6
	setEdgeNode(s2_ge00.getAdjacent2D().getPrevious() , s2_ge00.getAdjacent2D().getAdjacent3D().getNext().getEdgeNode()); //s2 4/6
	setEdgeNode(s2_ge00.getAdjacent2D().getNext(), s2_ge00.getAdjacent2D().getAdjacent3D().getPrevious().getEdgeNode()); //s2 5/6
	
	setEdgeNode(s1_ge00.getNext().getAdjacent2D().getPrevious() , newlink); //s1 6/6
	setEdgeNode(s2_ge00.getPrevious().getAdjacent2D().getNext() , newlink); //s2 6/6

	// ###################### Assign Inner faces #########################	
	setFaceNode(s1_ge00.getNext().getAdjacent2D() , newface3); //s1 + s2  1/4
	setFaceNode(s1_ge00.getPrevious().getAdjacent2D() , newface4); //s1 + s2 2/4
	// ###################### Assign faces #########################

	setFaceNode(s1_ge00 , handle1.getFaceNode()); // s1 3/4
	setFaceNode(s1_ge00.getAdjacent2D() , handle2.getFaceNode()); // s1 4/4
	
	setFaceNode(s2_ge00 , newface1) ; // s1 3/4
	setFaceNode(s2_ge00.getAdjacent2D() , newface2) ; // s1 3/4
	
	if(rng1 == -1 ) goto skip_trees_edge02;
	
	if(t1_in) {
		setInDualSpanningTree( handle1 , true );
		setInDualSpanningTree( handle1_nbr , true );

		int choice = (int)(2*rng1);

		choice ? setInDualSpanningTree(handle1.getAdjacent3D().getNext().getAdjacent2D(),true) : setInDualSpanningTree(handle1.getAdjacent3D().getPrevious().getAdjacent2D(),true);
	}
	else if(t2_in) {
		setInDualSpanningTree( handle2 , true );
		setInDualSpanningTree( handle2_nbr , true );

		int choice = (int)(2*rng1);

		choice ? setInDualSpanningTree(handle2.getAdjacent3D().getNext().getAdjacent2D(),true) : setInDualSpanningTree(handle2.getAdjacent3D().getPrevious().getAdjacent2D(),true);
	}
	else {

		int choice = (int)(12*rng1);

		if(choice == 0) {
			setInDualSpanningTree( handle1 , true );
			setInDualSpanningTree( handle1.getAdjacent3D().getNext().getAdjacent2D(), true);
		}
		else if(choice == 1) {
			setInDualSpanningTree( handle1 , true );
			setInDualSpanningTree( handle1.getAdjacent3D().getPrevious().getAdjacent2D(), true);
		}
		else if(choice == 2) {
			setInDualSpanningTree( handle1_nbr , true );
			setInDualSpanningTree( handle1_nbr.getAdjacent3D().getNext().getAdjacent2D(), true);
		}
		else if(choice == 3) {
			setInDualSpanningTree( handle1_nbr , true );
			setInDualSpanningTree( handle1.getAdjacent3D().getPrevious().getAdjacent2D(), true);
		}
		else if(choice == 4) {
			setInDualSpanningTree( handle2 , true );
			setInDualSpanningTree( handle2.getAdjacent3D().getNext().getAdjacent2D(), true);
		}
		else if(choice == 5) {
			setInDualSpanningTree( handle2 , true );
			setInDualSpanningTree( handle2.getAdjacent3D().getPrevious().getAdjacent2D(), true);
		}
		else if(choice == 6) {
			setInDualSpanningTree( handle2_nbr , true );
			setInDualSpanningTree( handle2.getAdjacent3D().getNext().getAdjacent2D(), true);
		}
		else if(choice == 7) {
			setInDualSpanningTree( handle2_nbr , true );
			setInDualSpanningTree( handle2.getAdjacent3D().getPrevious().getAdjacent2D(), true);
		}
		else if(choice == 8) {
			setInDualSpanningTree( handle1 , true );
			setInDualSpanningTree( handle1_nbr, true);
		}
		else if(choice == 9) {
			setInDualSpanningTree( handle1 , true );
			setInDualSpanningTree( handle2_nbr, true);
		}
		else if(choice == 10) {
			setInDualSpanningTree( handle2 , true );
			setInDualSpanningTree( handle1_nbr, true);
		}
		else if(choice == 11) {
			setInDualSpanningTree( handle2 , true );
			setInDualSpanningTree( handle2_nbr, true);
		}
	}
	
	if(l0_in) (int)(2*rng2) == 0 ? setInSpanningTree(handle1 , true) : setInSpanningTree(newedge->getHandle() , true);
	if(l1n_in) setInSpanningTree(l1n , true);
	if(l1p_in) setInSpanningTree(l1p , true); 
	if(l2n_in) setInSpanningTree(l2n , true);
	if(l2p_in) setInSpanningTree(l2p , true);

	skip_trees_edge02:

	IncrementCoord(l1n.getEdgeNode() , 1);
	IncrementCoord(l1n.getEdgeNode() , 1);
	IncrementCoord(l2n.getEdgeNode() , 1);
	IncrementCoord(l2n.getEdgeNode() , 1);

	IncrementCoord(l1p.getEdgeNode() , 1);
	IncrementCoord(l1p.getEdgeNode() , 1);
	IncrementCoord(l2p.getEdgeNode() , 1);
	IncrementCoord(l2p.getEdgeNode() , 1);
	
	for(int i  = 0 ; i < rots + 1 ; i++) { IncrementCoord(newedge , 1); }
	for(int i  = 0 ; i < rots - 1 ; i++) { DecrementCoord(handle1.getEdgeNode() , 1); }

	IncrementCoord(newlink , 1);
	IncrementCoord(newlink , 1);

	IncrementCoord(newlink->getHandle().getVertexNode(),0) ;
	IncrementCoord(newlink->getHandle().getVertexNode(),0) ;		

	IncrementCoord(newlink->getHandle().getNext().getVertexNode(),0) ;
	IncrementCoord(newlink->getHandle().getNext().getVertexNode(),0) ;

	IncrementCoord(newlink->getHandle().getPrevious().getVertexNode(),0) ;
	IncrementCoord(newlink->getHandle().getPrevious().getVertexNode(),0) ;

	IncrementCoord(newlink->getHandle().getAdjacent2D().getPrevious().getVertexNode(),0) ;
	IncrementCoord(newlink->getHandle().getAdjacent2D().getPrevious().getVertexNode(),0) ;

	return newlink->getHandle();
}

#endif
