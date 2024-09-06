#ifndef QUADRANGULAR_PILLOW_INVERSE_H
#define QUADRANGULAR_PILLOW_INVERSE_H

#include "triangulation.h"

template<typename RNG> Handle Triangulation::GetRandomEdgeCoord2(RNG & rng) { return coord_2_edges_[(int)(coord_2_edges_.size()*uniform_real(rng))]->getHandle(); }


void Triangulation:: unsetGraphFaceLinksBeforeMoveEdge20(Handle h1) {
	unsetGraphFaceLinks(h1);
	unsetGraphFaceLinks(h1.getAdjacent2D());
	
	unsetGraphFaceLinks(h1.getNext().getAdjacent2D());
	unsetGraphFaceLinks(h1.getPrevious().getAdjacent2D());
	unsetGraphFaceLinks(h1.getAdjacent3D().getNext().getAdjacent2D());
	unsetGraphFaceLinks(h1.getAdjacent3D().getPrevious().getAdjacent2D());
	
	unsetGraphEdgeLinks(h1.getNext().getAdjacent2D().getPrevious());
	unsetGraphEdgeLinks(h1.getAdjacent3D().getNext().getAdjacent2D().getPrevious());
}

template<typename RNG> bool Triangulation::TryGraphMoveEdge20(RNG & rng) {

	proposed_rate[5]++;
	
	if(GetEdgeCoord2Size() == 0 || numSimplices() < 4) return false;

	double SA = getActionNow();

	Handle handle1 = GetRandomEdgeCoord2(rng);  //randomly choose an edge

	if( isInSpanningTree(handle1)) return false; // the move is performed only if there is no vertex tree changes

	Handle h1A3DgNA2D = handle1.getAdjacent3D().getNext().getAdjacent2D(); // shortcut as it appears 5 times
	Handle h1gPgA2D = handle1.getPrevious().getAdjacent2D();
	
	//if( isInSpanningTree(h1gPgA2D.getNext()) ||	isInSpanningTree(h1A3DgNA2D.getPrevious()) ) return false;
		
	if( handle1.getVertexNode()->getCoord() == 2 || handle1.getAdjacent3D().getVertexNode()->getCoord() == 2) return false;
		
	if(h1gPgA2D.getNext().getEdgeNode()->getId() == h1A3DgNA2D.getPrevious().getEdgeNode()->getId()) return false;

	Handle nbrR1 = h1gPgA2D.getNext();
	Handle nbrR2 = h1gPgA2D.getNext().getAdjacent2D();

	Handle nbrL1 = h1A3DgNA2D.getPrevious();
	Handle nbrL2 = h1A3DgNA2D.getPrevious().getAdjacent2D();
		
	Handle ad2D = handle1.getAdjacent2D();
		
	int cr = nbrR1.getEdgeNode()->getCoord();
	int cl = nbrL1.getEdgeNode()->getCoord();
		
	int inR1 = isInDualSpanningTree(nbrR1);
	int inR2 = isInDualSpanningTree(nbrR2);
	int inL1 = isInDualSpanningTree(nbrL1);
	int inL2 = isInDualSpanningTree(nbrL2);
		
	int int1 = isInDualSpanningTree(handle1);
	int int2 = isInDualSpanningTree(ad2D);
	int lR1 = isInSpanningTree(handle1.getPrevious().getAdjacent2D().getNext());
	int lL1 = isInSpanningTree(handle1.getAdjacent3D().getNext().getAdjacent2D().getPrevious());

	if( (inR1+inR2+inL1+inL2 > 2) ) return false;
		
	if( ( ( inR1 && inL2 ) && (int1 || int2) ) || 
		( ( inL1 && inR2 ) && (int1 || int2) ) || 
		( inR1 && inR2 ) || ( inL1 && inL2 ) ) return false;

	std::vector<bool> treeinfo;	    // undoing is a pain here: need to store the tree info
		
	treeinfo.push_back(isInDualSpanningTree(handle1) );
	treeinfo.push_back(isInDualSpanningTree(ad2D) );
	treeinfo.push_back(isInDualSpanningTree(handle1.getNext().getAdjacent2D()) );
	treeinfo.push_back(isInDualSpanningTree(h1gPgA2D) );
	treeinfo.push_back(isInDualSpanningTree(h1A3DgNA2D) );
	treeinfo.push_back(isInDualSpanningTree(handle1.getAdjacent3D().getPrevious().getAdjacent2D()) );

	treeinfo.push_back(isInSpanningTree(handle1.getNext()));
	treeinfo.push_back(isInSpanningTree(handle1.getPrevious()) );
	treeinfo.push_back(isInSpanningTree(ad2D.getNext()) );
	treeinfo.push_back(isInSpanningTree(ad2D.getPrevious()) );
	treeinfo.push_back(isInSpanningTree(handle1.getPrevious().getAdjacent2D().getNext()) );
	treeinfo.push_back(isInSpanningTree(handle1.getAdjacent3D().getNext().getAdjacent2D().getPrevious()) );

	int rotator_number = nbrL1.getEdgeNode()->getCoord()-1;

	int wb = 0;
		
		
	if( ( (inR1 && inL1) && (int1 || int2) ) || ( (inR2 && inL2) && (int1 || int2) ) ) { //if the tree traverses the pillow
		//wb--; //then the other two triangles are in the middle sector; after collapse decrease the graph cord by one
		
		if( !random_bernoulli(rng,0.5) ) return false ;
		
	}
	else{								//if doesn't traverse
			//if( (int1 || int2) ) wb--; // it has only one external triangle which are in the tree, thus has a pair that will collapse into one, decrease the graph coord
			//else if( !(int1 || int2) &&  ((inR1 && inL1) || (inR2 && inL2)) ) wb--; //if internal triangles are not in the dual tree, then it has two external one, if the 
		if( !random_bernoulli(rng,1.0/12.0) ) return false;
	}
		
	double factor = 1.0;
	
	if(isInSpanningTree(nbrR1) || isInSpanningTree(nbrL1)) factor = 2.0;
	
	double prob = (double)( ( GetEdgeCoord2Size() ) / (double)( factor*3*(numSimplices() - 2)*(cr + cl - 3)) );

	
	unsetGraphFaceLinksBeforeMoveEdge20(handle1);
	Handle handle2 = unexpandRectangle(handle1, 1);
	

	double SB = getActionNow();
	double moveprob = uniform_real(rng);

	if( !getProbabilityNow(moveprob,SA,SB,prob) ) {

		Handle handle3 = handle2.getRotationAroundEdge(rotator_number).getAdjacent3D(); //avoid 0 or max rotation

		unsetGraphFaceLinksBeforeMoveEdge02(handle2,handle3);

		Handle handle_after = expandRectangle(handle2,handle3,-1,-1,rotator_number);


		setInDualSpanningTree( handle_after,treeinfo[0] );
		setInDualSpanningTree( handle_after.getAdjacent2D(),treeinfo[1] );
		setInDualSpanningTree( handle_after.getNext().getAdjacent2D(),treeinfo[2] );
		setInDualSpanningTree( handle_after.getPrevious().getAdjacent2D(),treeinfo[3] );
		setInDualSpanningTree( handle_after.getAdjacent3D().getNext().getAdjacent2D(),treeinfo[4] );
		setInDualSpanningTree( handle_after.getAdjacent3D().getPrevious().getAdjacent2D(),treeinfo[5] );
			
		setInSpanningTree( handle_after.getNext(),treeinfo[6] );
		setInSpanningTree( handle_after.getPrevious(),treeinfo[7] );
		setInSpanningTree( handle_after.getAdjacent2D().getNext(),treeinfo[8] );
		setInSpanningTree( handle_after.getAdjacent2D().getPrevious(),treeinfo[9] );
		setInSpanningTree( handle_after.getPrevious().getAdjacent2D().getNext(),treeinfo[10] );
		setInSpanningTree( handle_after.getAdjacent3D().getNext().getAdjacent2D().getPrevious(),treeinfo[11] );


		
		setGraphFaceLinksAfterMoveEdge02(handle_after);
		

		return false;
	}

	accepted_rate[5]++;
	return true;
}

Handle Triangulation::unexpandRectangle(Handle handle1, int setTrees) {
	

	DecrementCoord(handle1.getEdgeNode() , 1);
	DecrementCoord(handle1.getEdgeNode() , 1);

	Handle nbrR1 = handle1.getPrevious().getAdjacent2D().getNext().getAdjacent3D();
	Handle nbrR2 = handle1.getPrevious().getAdjacent2D().getNext().getAdjacent2D().getAdjacent3D();

	Handle nbrL1 = handle1.getAdjacent3D().getNext().getAdjacent2D().getPrevious().getAdjacent3D();
	Handle nbrL2 = handle1.getAdjacent3D().getNext().getAdjacent2D().getPrevious().getAdjacent2D().getAdjacent3D();
	
	TreeNode * rightNode = nbrR1.getEdgeNode();
	TreeNode * leftNode = nbrL1.getEdgeNode();
	
	TreeNode * L1FaceNode = nbrL1.getFaceNode();
	TreeNode * L2FaceNode = nbrL2.getFaceNode();
	TreeNode * R1FaceNode = nbrR1.getFaceNode();
	TreeNode * R2FaceNode = nbrR2.getFaceNode();
	
	int cr = rightNode->getCoord();
	int cl = leftNode->getCoord();

	for(int i = 0 ; i < cl; i++) DecrementCoord( leftNode , 1); //left to be removed
	for(int i = 0 ; i < cl-2; i++) IncrementCoord( rightNode , 1);  //right to be merged

	DecrementCoord(nbrR1.getNext().getEdgeNode(), 1);
	DecrementCoord(nbrR1.getNext().getEdgeNode(), 1);
	
	DecrementCoord(nbrR1.getPrevious().getEdgeNode(), 1);
	DecrementCoord(nbrR1.getPrevious().getEdgeNode(), 1);
	
	DecrementCoord(nbrR2.getNext().getEdgeNode(), 1);
	DecrementCoord(nbrR2.getNext().getEdgeNode(), 1);
	
	DecrementCoord(nbrR2.getPrevious().getEdgeNode(), 1);
	DecrementCoord(nbrR2.getPrevious().getEdgeNode(), 1);

	DecrementCoord(nbrR1.getVertexNode(), 0);
	DecrementCoord(nbrR1.getVertexNode(), 0);

	DecrementCoord(nbrR1.getNext().getVertexNode(), 0);
	DecrementCoord(nbrR1.getNext().getVertexNode(), 0);

	DecrementCoord(nbrR1.getPrevious().getVertexNode(), 0);
	DecrementCoord(nbrR1.getPrevious().getVertexNode(), 0);

	DecrementCoord(nbrR2.getPrevious().getVertexNode(), 0);
	DecrementCoord(nbrR2.getPrevious().getVertexNode(), 0);

	Handle l1 = nbrR1.getNext();//handle1.getNext();
	Handle l2 = nbrR1.getPrevious();//handle1.getPrevious();
	Handle l3 = nbrR2.getNext();//handle1.getAdjacent2D().getNext();
	Handle l4 = nbrR2.getPrevious();//handle1.getAdjacent2D().getPrevious();

	Simplex * sh1 = handle1.getSimplex();
	Simplex * sh2 = handle1.getAdjacent3D().getSimplex();

	int in11 = isInDualSpanningTree(nbrR1);
	int in12 = isInDualSpanningTree(nbrR2);
	int in21 = isInDualSpanningTree(nbrL1);
	int in22 = isInDualSpanningTree(nbrL2);
	int int1 = isInDualSpanningTree(handle1);
	int int2 = isInDualSpanningTree(handle1.getAdjacent2D());
	
	int inl1 = isInSpanningTree(l1);
	int inl2 = isInSpanningTree(l2);
	int inl3 = isInSpanningTree(l3);
	int inl4 = isInSpanningTree(l4);
	int inlR = isInSpanningTree(nbrR1);
	int inlL = isInSpanningTree(nbrL1);

	unsetGraphFaceLinks(handle1);
	unsetGraphFaceLinks(handle1.getAdjacent2D());

	unsetGraphFaceLinks(nbrR1);
	unsetGraphFaceLinks(nbrL1);
	unsetGraphFaceLinks(nbrR2);
	unsetGraphFaceLinks(nbrL2);

	unsetGraphEdgeLinks(nbrR1);
	unsetGraphEdgeLinks(nbrL1);

	if(int1) setInDualSpanningTree(handle1,false);
	if(int2) setInDualSpanningTree(handle1.getAdjacent2D(),false);
	
	if(in11) setInDualSpanningTree(nbrR1,false);
	if(in12) setInDualSpanningTree(nbrR2,false);
	if(in21) setInDualSpanningTree(nbrL1,false);
	if(in22) setInDualSpanningTree(nbrL2,false);

	if(inl1) setInSpanningTree(l1,false);
	if(inl2) setInSpanningTree(l2,false);
	if(inl3) setInSpanningTree(l3,false);
	if(inl4) setInSpanningTree(l4,false);
	if(inlR) setInSpanningTree(nbrR1,false);
	if(inlL) setInSpanningTree(nbrL1,false);

	// #################### SET VERTEX HANDLE ####################

	nbrR1.getVertexNode()->setHandle(nbrR1);
	nbrR1.getNext().getVertexNode()->setHandle(nbrR1.getNext());
	nbrR1.getPrevious().getVertexNode()->setHandle(nbrR1.getPrevious());
	nbrR2.getNext().getVertexNode()->setHandle(nbrR2.getNext());
	nbrR2.getPrevious().getVertexNode()->setHandle(nbrR2.getPrevious());

	// #################### SET EDGE HANDLE ####################

	nbrR1.getEdgeNode()->setHandle(nbrR1.getAdjacent3D());
	nbrR2.getEdgeNode()->setHandle(nbrR2.getAdjacent3D());
	nbrL1.getEdgeNode()->setHandle(nbrR1.getAdjacent3D());
	nbrL2.getEdgeNode()->setHandle(nbrR1.getAdjacent3D());		
	
	l1.getEdgeNode()->setHandle(l1);
	l2.getEdgeNode()->setHandle(l2);
	l3.getEdgeNode()->setHandle(l3);
	l4.getEdgeNode()->setHandle(l4);

	// #################### SET FACE NODE ####################

	nbrL1.setFaceNode(R1FaceNode); //left top 
	nbrL2.setFaceNode(R2FaceNode); //left bot
	deleteNode(L1FaceNode); // left top
	deleteNode(L2FaceNode); // left bot

	setAdjacent(nbrR1,nbrL1);
	setAdjacent(nbrR2,nbrL2);

	setEdgeNode(nbrL1,rightNode); // THIS ruins l4

	deleteNode(leftNode); //delete left edge

	deleteNode(handle1.getFaceNode()); // delete inner 1
	deleteNode(handle1.getAdjacent2D().getFaceNode()); // delete inner2		
	deleteNode(handle1.getEdgeNode()); // delete central edge 

	deleteSimplex(sh1); //delete simplex 1
	deleteSimplex(sh2); //delete simplex 2 

	if( setTrees ) {
		if( (in11 && in21) && ( int1 || int2 ) ) { setInDualSpanningTree(nbrR1); } //CHECK HERE???
		else if( (in12 && in22) && ( int1 || int2 ) ) { setInDualSpanningTree(nbrR2); }
		else { /* do nothing */ } //( (in11 && in22) && (in21 && in12) )
		
		
		if(inlR || inlL) setInSpanningTree(nbrR1);
		if(inl1) setInSpanningTree(l1);
		if(inl2) setInSpanningTree(l2);
		if(inl3) setInSpanningTree(l3);
		if(inl4) setInSpanningTree(l4);

		setGraphEdgeLinks(nbrR1);

		setGraphFaceLinks(nbrR1);
		setGraphFaceLinks(nbrR2);

	}

	return nbrR1;	
}

#endif
