#ifndef TRIANGULAR_PILLOW_H
#define TRIANGULAR_PILLOW_H

#include "triangulation.h"



template<typename RNG> bool Triangulation::TryGraphMove02(RNG & rng) {
	proposed_rate[0]++;

	double SA = getActionNow(); 

	Handle handle = getRandomEdge(rng);  //randomly choose an edge

	Handle newhandle = handle ;
	
    double factor = 3.0;
	
	unsetGraphFaceLinks(newhandle);

	int coord1 = handle.getVertexNode()->getCoord();
	int coord2 = handle.getPrevious().getVertexNode()->getCoord();
	int coord3 = handle.getNext().getVertexNode()->getCoord();
	
	int wb = 1; 
	(coord1 == 2) ? wb-- : wb = wb;
	(coord2 == 2) ? wb-- : wb = wb;
	(coord3 == 2) ? wb-- : wb = wb;
	
	double prob = factor * (double)( numSimplices()*2 ) / ((double)(GetCoord2Size() + wb )); // * deltap; 
	
	//double prob = ((double)numSimplices())/((double)numSimplices()+2)*3.0;	
	
	
	Handle origin = expandTriangle(newhandle,uniform_real(rng),uniform_real(rng));  // perform the expand move
	setGraphFaceLinksAfterMove02(origin);

	double SB = getActionNow(); 
	double move_probability = uniform_real(rng);//*deltap;	

	if( !getProbabilityNow(move_probability,SA,SB,prob) ) {
		
		unsetGraphFaceLinksAfterMove02(origin);
		Handle handle2 = unExpandTriangle(origin);
		setGraphFaceLinksAfterMove20(handle2);
		return false;
	}
	
	accepted_rate[0]++;
		
	return true; 
}



std::pair<Handle, int> Triangulation::Identify_move02(Handle handle,double dual_tree_rooting, int tree_rng, double move_probability) {
		double SA = getActionNow();
		// THE idea is to identify the situation by counting the number of faces and edges in the trees
		// then rotate the handle (get next/ previous) depending on the 3 possible sets of orientations
		
		//check the two random numbers that defines the out configuration

		bool t_in = isInDualSpanningTree( handle ); // check if the triangle is I or 0
		
		bool l1_in = isInSpanningTree( handle ); // check if link 1 is I or 0
		bool l2_in = isInSpanningTree( handle.getNext() ); // check if link 2 is I or 0
		bool l3_in = isInSpanningTree( handle.getPrevious() ); // check if link 2 is I or 0
		
		int l_orient = 100*l1_in + 10*l2_in + l3_in; // create an orientation number for less if statements
		
		int orient_sum = l1_in + l2_in + l3_in; // number of edges in the tree ( 0 | 1 | 2 )

		Handle newhandle; //create a new handle
		int choice = 0; //x > 0 ? 1 : x == 0 ? 0 : -1; //triple choice
		
		if (l_orient == 10 || l_orient == 110) newhandle = handle.getNext();			//rotate orientation forward
		else if(l_orient == 1 || l_orient == 11) newhandle = handle.getPrevious();		//rotate orientation backwards
		else {newhandle = handle;} //(l_orient == 100 or l_orient == 101 or 000)			//don't rotate orientation
		
		int dual_tree_rng = (int)(3*dual_tree_rooting);
		int dual_tree_rng2 = (int)(7*dual_tree_rooting);
		
		if (t_in) {
			if (l_orient == 110 || l_orient == 11 || l_orient == 101) { /* nothing */ } 
			else if ( l_orient == 100 || l_orient == 10 || l_orient == 1 ) {
				
				choice = !getMiddleGraphIsConnected(newhandle.getNext().getEdgeNode()->getGraphId(), newhandle.getPrevious().getEdgeNode()->getGraphId());
				
			}
			else {
				int a_to_b = getMiddleGraphIsConnected(newhandle.getEdgeNode()->getGraphId(), newhandle.getNext().getEdgeNode()->getGraphId());
				int b_to_c = getMiddleGraphIsConnected(newhandle.getNext().getEdgeNode()->getGraphId(), newhandle.getPrevious().getEdgeNode()->getGraphId());
				int a_to_c = getMiddleGraphIsConnected(newhandle.getEdgeNode()->getGraphId(), newhandle.getPrevious().getEdgeNode()->getGraphId());
				
				if(a_to_b)       dual_tree_rng == 2 ? choice = 0 : choice = 1 ; // if a and b are connected and it is the "down" situation
				else if (a_to_c) dual_tree_rng == 1 ? choice = 0 : choice = 1 ; // if a and c are connected and it is the "right situation"
				else if (b_to_c) dual_tree_rng == 0 ? choice = 0 : choice = 1 ; // if b and c are connected and it is the "left" situation
				else choice = 1;
			}
		}
		else {
			if ( l_orient == 110 || l_orient == 11 || l_orient == 101) { /* nothing */ }
			else if ( l_orient == 100 || l_orient == 10 || l_orient == 1 ) { // if there is 1 edge in the tree
					DeleteEdgeMiddleGraphPure(newhandle.getNext());
					DeleteEdgeMiddleGraphPure(newhandle.getPrevious());
					
					choice = !getMiddleGraphIsConnected(newhandle.getNext().getEdgeNode()->getGraphId(), newhandle.getPrevious().getEdgeNode()->getGraphId());
					
					AddEdgeMiddleGraphPure(newhandle.getNext());
					AddEdgeMiddleGraphPure(newhandle.getPrevious());
					
				}
			else { //if there are 0 edges in the tree
				DeleteEdgeMiddleGraphPure(newhandle);
				DeleteEdgeMiddleGraphPure(newhandle.getNext());
				DeleteEdgeMiddleGraphPure(newhandle.getPrevious());
				
				int a_to_b = getMiddleGraphIsConnected(newhandle.getEdgeNode()->getGraphId(), newhandle.getNext().getEdgeNode()->getGraphId());
				int b_to_c = getMiddleGraphIsConnected(newhandle.getNext().getEdgeNode()->getGraphId(), newhandle.getPrevious().getEdgeNode()->getGraphId());
				int a_to_c = getMiddleGraphIsConnected(newhandle.getEdgeNode()->getGraphId(), newhandle.getPrevious().getEdgeNode()->getGraphId());
				
				AddEdgeMiddleGraphPure(newhandle);
				AddEdgeMiddleGraphPure(newhandle.getNext());
				AddEdgeMiddleGraphPure(newhandle.getPrevious());
				
				choice = a_to_b + a_to_c + b_to_c;
			}	
		}
		//printf("CHOICE = %d\n",choice);
		
		std::pair<int, int> Ixx_table[3][3][3] = { //table for I.0,1,2 
			{
				{choice? std::make_pair(0,1) : std::make_pair(-1,0) , std::make_pair(0,0), choice? std::make_pair(0,1) : std::make_pair(-1,0)},
				{std::make_pair(0,0), choice? std::make_pair(0,1) : std::make_pair(-1,0) , choice? std::make_pair(0,1) : std::make_pair(-1,0)},
				{choice? std::make_pair(0,1) : std::make_pair(-1,0) , choice? std::make_pair(0,1) : std::make_pair(-1,0),std::make_pair(0,0)}
			},
			{
				{std::make_pair(0,0),std::make_pair(1,0),std::make_pair(0,0)},
				{std::make_pair(1,0),std::make_pair(0,0),std::make_pair(0,0)},
				{choice? std::make_pair(0,1) : std::make_pair(-1,0),choice? std::make_pair(0,1) : std::make_pair(-1,0),std::make_pair(0,0)}
				
			},
			{
				{std::make_pair(0,0),std::make_pair(1,0),std::make_pair(0,0)},
				{std::make_pair(2,0),std::make_pair(1,0),std::make_pair(1,0)},
				{std::make_pair(0,0),std::make_pair(0,0),std::make_pair(1,0)}
			},
		};
			
		std::pair<int, int> II_AS_xx_table[3][7][3] = { // II.A.0,1 , II.S
			{
				{std::make_pair(0,1),std::make_pair(0,0),std::make_pair(0,1)},
				{std::make_pair(1,0),std::make_pair(0,1),std::make_pair(0,1)},
				{std::make_pair(0,1),std::make_pair(0,1),std::make_pair(0,0)},
				{std::make_pair(0,1),std::make_pair(0,0),std::make_pair(0,1)},
				{std::make_pair(1,0),std::make_pair(0,1),std::make_pair(0,1)},
				{std::make_pair(0,1),std::make_pair(0,1),std::make_pair(0,0)},
				{std::make_pair(0,0),std::make_pair(0,0),std::make_pair(0,0)}
			},
			{
				{std::make_pair(0,0),std::make_pair(0,1),std::make_pair(0,0)},
				{std::make_pair(1,0),std::make_pair(0,0),std::make_pair(0,0)},
				{std::make_pair(0,1),std::make_pair(0,1),std::make_pair(0,0)},
				{std::make_pair(0,0),std::make_pair(0,1),std::make_pair(0,0)},
				{std::make_pair(1,0),std::make_pair(0,0),std::make_pair(0,0)},
				{std::make_pair(0,1),std::make_pair(0,1),std::make_pair(0,0)},
				{choice? std::make_pair(-1,0) : std::make_pair(0,1),choice? std::make_pair(-1,0) : std::make_pair(0,1),choice? std::make_pair(-1,0) : std::make_pair(0,1)}
			},
			{
				{std::make_pair(0,0),std::make_pair(1,0),std::make_pair(0,0)},
				{std::make_pair(2,0),std::make_pair(1,0),std::make_pair(1,0)},
				{std::make_pair(0,0),std::make_pair(0,0),std::make_pair(1,0)},
				{std::make_pair(0,0),std::make_pair(1,0),std::make_pair(0,0)},
				{std::make_pair(2,0),std::make_pair(1,0),std::make_pair(1,0)},
				{std::make_pair(0,0),std::make_pair(0,0),std::make_pair(1,0)},
				{choice == 0 ? std::make_pair(-2,0) : choice == 1? std::make_pair(-1,1) : std::make_pair(0,2),choice == 0 ? std::make_pair(-2,0) : choice == 1? std::make_pair(-1,1) : std::make_pair(0,2),choice == 0 ? std::make_pair(-2,0) : choice == 1? std::make_pair(-1,1) : std::make_pair(0,2)}

			},
		};

		std::pair<int, int> selected;
		
		t_in ? selected = Ixx_table[orient_sum][dual_tree_rng][tree_rng] :  selected = II_AS_xx_table[orient_sum][dual_tree_rng2][tree_rng];

		//printf("CHANGE in components is %d and in loop is %d\n",selected.first,selected.second);
		
		int n0 = numVertices()+1;
		int n3 = numSimplices()+2;
		int nc = getMiddleGraphNumConnected() + selected.first;
		int nl = nc + getMiddleGraphEdgeSize() - getMiddleGraphNodeSize() + selected.second;
		
		double SB = getAction(n0,n3, nl, nc);
		
		//printf("ACTIONS: %g\t%g\t%g\t%d\n",SA,SB,move_probability,n3);
		
		int coord1 = handle.getVertexNode()->getCoord();
		int coord2 = handle.getPrevious().getVertexNode()->getCoord();
		int coord3 = handle.getNext().getVertexNode()->getCoord();

		int wb = 0; 
		if(coord1 == 2) wb-- ;
		if(coord2 == 2) wb-- ;
		if(coord3 == 2) wb-- ;


		double prob = 3.0*2*(double)(numSimplices()) / ((double)(GetCoord2Size() + wb + 1)) ;
		//double prob = ((double)numSimplices())/((double)numSimplices()+2);
				
		int success = getProbabilityNow(move_probability,SA, SB,prob);
			
		//printf("PROBABILITY_move02: %g\t%g\t%d COORD SIZE: %d \n",prob, move_probability,(prob > 1 || move_probability < prob), (getCoord2Size() + wb + 1));
		//printf("probability Elements_move02: %g\t%g\t%g\t%g\t%d\t%g\t%g\t",2*getK3(), - getK0(), - getBetaL() * selected.second,  - getBetaC() * selected.first,numSimplices(), dn3,prob);
		
		std::pair<Handle, int > RESULT;
		
		RESULT.first = newhandle;
		RESULT.second = success;
		
		//printf("MOVE02 SUCC: %d\n",success);
		
		return RESULT;
	}






#endif
