#ifndef TRIANGULAR_PILLOW_INVERSE_H
#define TRIANGULAR_PILLOW_INVERSE_H

#include "triangulation.h"

template<typename RNG> Handle Triangulation::GetRandomCoord2(RNG & rng) {	return coord_2_nodes_[(int)(coord_2_nodes_.size()*uniform_real(rng))]->getHandle(); }



template<typename RNG> bool Triangulation::TryGraphMove20(RNG & rng) {	
	proposed_rate[1]++;

	double SA = getActionNow(); 	

	Handle handle0;

	if(coord_2_nodes_.size() != 0) handle0 = GetRandomCoord2(rng); 
	else return false;

	//std::pair<Handle, int> IDM20 = Identify_move20(handle0, move_probability, fail_probability);

	//if(!IDM20.second) { return false;}
	//else {/*printf("SUCCESS M20!!!\n");*/}
	
	//Handle origin = IDM20.first;
	Handle origin = handle0;	
	
	//printf("GOOD!\n");
	
	//PrintGraphIndBef();
	//Handle origin = getRandomEdge(rng);
	
	// check that we have two simplices with three glued triangles
	if( !sameCorner(origin.getAdjacent3D().getNext(),origin.getAdjacent2D().getNext().getAdjacent3D().getNext()) ||
	    !sameCorner(origin.getAdjacent3D().getNext(),origin.getPrevious().getAdjacent2D().getAdjacent3D().getNext()) ||
	    numSimplices() == 2) {
	    return false;
	}

	Handle adj1 = origin.getNext().getAdjacent2D().getAdjacent3D();
	Handle adj2 = origin.getNext().getAdjacent3D().getAdjacent2D().getAdjacent3D();
	
	if( !allowsSelfAdjacency() && adj1.getSimplex() == adj2.getSimplex() ) return false;
	
    int numspanning = 0;
    
    Handle edge = origin;
    for(int i=0;i<3;++i) {
        if( isInSpanningTree(edge) ) numspanning++;
        edge = edge.getAdjacent2D().getNext();
    }
    
    if( numspanning != 1 ) return false; 

	double factor;
    if( !isInDualSpanningTree(adj1) || !isInDualSpanningTree(adj2) ||
        !(isInDualSpanningTree(origin) || 
          isInDualSpanningTree(origin.getAdjacent2D()) || 
          isInDualSpanningTree(origin.getPrevious().getAdjacent2D())) ) {
        // since there are 7 return options, we should only accept this move with probability 1/7
        if( !random_bernoulli(rng,1.0/7) ) return false;
    } 
    else if( !random_bernoulli(rng,1.0/3) ) return false; // since there are 3 return options, we should only accept this move with probability 1/3

    factor = 3.0;

	std::vector<bool> treeinfo;	    // undoing is a pain here: need to store the tree info

    treeinfo.push_back( isInDualSpanningTree( origin ) );
    treeinfo.push_back( isInDualSpanningTree( origin.getAdjacent2D() ) );
    treeinfo.push_back( isInDualSpanningTree( origin.getPrevious().getAdjacent2D() ) );
    treeinfo.push_back( isInDualSpanningTree( origin.getNext().getAdjacent2D() ) );
    treeinfo.push_back( isInDualSpanningTree( origin.getAdjacent3D().getPrevious().getAdjacent2D() ) );
    treeinfo.push_back( isInSpanningTree( origin ) );
    treeinfo.push_back( isInSpanningTree( origin.getPrevious() ) );
    treeinfo.push_back( isInSpanningTree( origin.getAdjacent2D().getNext() ) );		
	
	
	clearGraphFaceLinksBeforeMove20(origin);

	double move_probability = uniform_real(rng);
	double prob;

	prob  = ( (double)(GetCoord2Size()) ) / ( (double)(numSimplices()-2)*2) / factor ;
	
	Handle handle = unExpandTriangle(origin);
	
	setGraphFaceLinksAfterMove20(handle);

	double SB = getActionNow(); 
	

	if( !getProbabilityNow(move_probability,SA,SB,prob) ) {
	    // undoing is a pain here
	    unsetGraphFaceLinksAfterMove20(handle);
	    Handle origin2 = expandTriangle(handle,0.0,0.0);
	    
	    setInDualSpanningTree( origin2, treeinfo[0] );
	    setInDualSpanningTree( origin2.getAdjacent2D(), treeinfo[1] );
	    setInDualSpanningTree( origin2.getPrevious().getAdjacent2D() , treeinfo[2] );
	    setInDualSpanningTree( origin2.getNext().getAdjacent2D() , treeinfo[3] );
	    setInDualSpanningTree( origin2.getAdjacent3D().getPrevious().getAdjacent2D(), treeinfo[4] );
	    setInSpanningTree( origin2 , false );
	    setInSpanningTree( origin2.getPrevious() , false );
	    setInSpanningTree( origin2.getAdjacent2D().getNext() , false );
	    setInSpanningTree( origin2 , treeinfo[5] );
	    setInSpanningTree( origin2.getPrevious(), treeinfo[6] );
	    setInSpanningTree( origin2.getAdjacent2D().getNext() , treeinfo[7] );
	    setGraphFaceLinksAfterMove02(origin2);
	    
	    return false;
	}
	
	
	accepted_rate[1]++;
	return true;
}


std::pair<Handle, int> Triangulation::Identify_move20(Handle origin, double move_probability, double fail_probability) { //Implementation should be finished
    	
    	// EARLY QUITS
	
	if (numSimplices() < 4) return std::make_pair(origin,false); // if there are less than 4 simplices
	
	Handle handle = origin.getNext().getAdjacent2D();
	Handle nbr = origin.getAdjacent3D().getPrevious().getAdjacent2D();
	
	std::pair<Handle, int> RESULT;
	RESULT.first = origin;
	
	//if(fail_probability > 1.0/3)  return std::make_pair(origin,false); // EARLY QUIT (second case is 1/7 )
	int t_in = 10*handle.getFaceNode()->getInMiddleGraph()+1*nbr.getFaceNode()->getInMiddleGraph();
	
    
	//Rotate picture to treehandle being at origin
	if(isInSpanningTree(origin)) {}
	else if( isInSpanningTree(origin.getAdjacent2D().getNext()) ) {origin = origin.getAdjacent2D().getNext();}
	else origin = origin.getPrevious().getAdjacent2D();
	
	//##############################################
	//##############################################
	//##############################################
	Handle adj1 = origin.getNext().getAdjacent2D().getAdjacent3D();
	Handle adj2 = origin.getNext().getAdjacent3D().getAdjacent2D().getAdjacent3D();
	
	if( !sameCorner(origin.getAdjacent3D().getNext(),origin.getAdjacent2D().getNext().getAdjacent3D().getNext()) ||
	    !sameCorner(origin.getAdjacent3D().getNext(),origin.getPrevious().getAdjacent2D().getAdjacent3D().getNext()) ||
	    numSimplices() == 2) {
	    return std::make_pair(origin,false);;
	}
	
	if( !allowsSelfAdjacency() && adj1.getSimplex() == adj2.getSimplex() ) return std::make_pair(origin,false);;
	
	
	int numspanning = 0;
    
    Handle edge = origin;
    for(int i=0;i<3;++i) {
        if( isInSpanningTree(edge) ) numspanning++;
        edge = edge.getAdjacent2D().getNext();
    }
    
    if( numspanning != 1 ) return std::make_pair(origin,false);;
    
	//###################################
	//&################################x#
    
    double SA = getActionNow(); 

	int info = 0; //middle links in the middle sector
	info += 100*origin.getFaceNode()->getInMiddleGraph();
	//info += 1000*origin.getPrevious().getAdjacent2D().getEdgeNode()->getInMiddleGraph();
	info += 10*origin.getPrevious().getAdjacent2D().getFaceNode()->getInMiddleGraph();//always true
	//info += 10*origin.getPrevious().getAdjacent2D().getPrevious().getAdjacent2D().getEdgeNode()->getInMiddleGraph();//always true
	info += 1*origin.getPrevious().getAdjacent2D().getPrevious().getAdjacent2D().getFaceNode()->getInMiddleGraph();
	
	int info_l[3]; //base links in the middle sector
	
	Handle h1 = origin.getNext();
	Handle h2 = origin.getPrevious().getAdjacent2D().getNext();
	Handle h3 = origin.getAdjacent2D().getPrevious();
	
	info_l[0] = h1.getEdgeNode()->getInMiddleGraph();
	info_l[1] = h2.getEdgeNode()->getInMiddleGraph();
	info_l[2] = h3.getEdgeNode()->getInMiddleGraph();
	
	int midsum = info_l[0] + info_l[1]+ info_l[2]; //number of links of the base triangle in the middle sector
	
//    	printf("TIN: %d\n",t_in); //both triangles are in the dual tree
	
	int info_t[3]; // Triangles in the middle sector
	info_t[0] = getHandleMiddleGraphHasEdge(h1);
	info_t[1] = getHandleMiddleGraphHasEdge(h2);
	info_t[2] = getHandleMiddleGraphHasEdge(h3);
	
	int midt_in = info_t[0]+info_t[1]+info_t[2];
	int traverse = t_in*10 + midt_in;
	
	if( ( (t_in*10 + midt_in) != 111) &&  fail_probability > 3.0/7) return std::make_pair(origin,false);
    

	//printf("INFO_L: %d\t%d\t%d\n",info_l[0],info_l[1],info_l[2]);
	//printf("INFO_T: %d\t%d\t%d\n",info_t[0],info_t[1],info_t[2]);
	
	//REMOVE potential/ disconnect center
	
	if(info_t[0]) DeleteEdgeMiddleGraphPure( h1 );
	if(info_t[1]) DeleteEdgeMiddleGraphPure( h2 );
	if(info_t[2]) DeleteEdgeMiddleGraphPure( h3 );
	
	    	
	std::pair<int, int> selected; // store choice
	
	int choice = 0;
	int link_digits = 100*info_l[0] + 10*info_l[1] + 1*info_l[2];
	int triangle_digits = 100*info_t[0] + 10*info_t[1] + 1*info_t[2];
	
	std::pair<int, int> I_table[3][3] = {	
		{std::make_pair(-1,0),std::make_pair(-1,0),std::make_pair(0,0)},
		{std::make_pair(0,0),std::make_pair(-2,0),std::make_pair(0,0)},
		{std::make_pair(0,0),std::make_pair(-1,0),std::make_pair(-1,0)}
	};
	
	std::pair<int, int> II_table[4][3] = {	
		{std::make_pair(0,0),std::make_pair(-1,0), choice == 0 ? std::make_pair(0,-1) : std::make_pair(1,0)},
		{choice == 0 ? std::make_pair(0,-1) : std::make_pair(1,0),std::make_pair(-1,0),std::make_pair(0,0)},
		{std::make_pair(0,0),std::make_pair(0,0),std::make_pair(0,0)},
		{choice == 0 ? std::make_pair(0,-1) : std::make_pair(1,0),choice == 0 ? std::make_pair(0,-1) : std::make_pair(1,0),choice == 0 ? std::make_pair(0,-1) : std::make_pair(1,0)}
	};
	
	if( midsum == 1) { //If 1 links of the triangle are in the middle graph (2 in the vertex-Tree
		if(triangle_digits != 111) {
			int a,b;
			link_digits == 100 ? a = 0 : link_digits == 10 ? a = 1 : a = 2;
			triangle_digits == 11 ? b = 0 : link_digits == 101 ? b = 1 : b = 2;
			
			selected = I_table[a][b];
		}
		else selected = std::make_pair(0,0);
	}
	else if( midsum == 2) { //If 2 links of the triangle are in the middle graph (1 in the vertex-Tree
		int a,b;
		link_digits == 110 ? a = 0 : link_digits == 11 ? a = 1 : a = 2;
		triangle_digits == 11 ? b = 0 : link_digits == 101 ? b = 1 : b = 2;
		
		if(link_digits == 110 && triangle_digits == 110) choice = getMiddleGraphIsConnected(h1.getEdgeNode()->getGraphId(),h2.getEdgeNode()->getGraphId());
		else if(link_digits == 11 && triangle_digits == 11) choice = getMiddleGraphIsConnected(h2.getEdgeNode()->getGraphId(),h3.getEdgeNode()->getGraphId());
		else if(111){ //if symmetric
			if(triangle_digits == 110) {
				choice = getMiddleGraphIsConnected(h1.getEdgeNode()->getGraphId(),h2.getEdgeNode()->getGraphId());
				choice == 0 ? selected =  std::make_pair(1,0) : selected = std::make_pair(0,-1) ;
			}
			else if(triangle_digits == 011) {
				choice = getMiddleGraphIsConnected(h2.getEdgeNode()->getGraphId(),h3.getEdgeNode()->getGraphId());
				choice == 0 ? selected = std::make_pair(1,0) : selected = std::make_pair(0,-1) ;
			}
			else if(triangle_digits == 101) {
				choice = getMiddleGraphIsConnected(h1.getEdgeNode()->getGraphId(),h3.getEdgeNode()->getGraphId());
				choice == 0 ? selected = std::make_pair(1,0) : selected = std::make_pair(0,-1) ;
			}
			a = 3;
		}
		
		selected = II_table[a][b];
	}
	else if( midsum == 3 ) {// if all links are in the middle graph, non in the vertex tree
		
		if(triangle_digits == 11) {
			choice = getMiddleGraphIsConnected(h2.getEdgeNode()->getGraphId(),h3.getEdgeNode()->getGraphId());
			choice == 0 ? selected =  std::make_pair(1,0) : selected = std::make_pair(0,-1) ;
		}
		else if(triangle_digits == 110) {
			choice = getMiddleGraphIsConnected(h1.getEdgeNode()->getGraphId(),h2.getEdgeNode()->getGraphId());
			choice == 0 ? selected = std::make_pair(1,0) : selected = std::make_pair(0,-1) ;
		}
		else if(triangle_digits == 101) selected = std::make_pair(0,0);
		else {
		
		choice += getMiddleGraphIsConnected(h1.getEdgeNode()->getGraphId(),h2.getEdgeNode()->getGraphId());
		choice += getMiddleGraphIsConnected(h1.getEdgeNode()->getGraphId(),h3.getEdgeNode()->getGraphId());
		choice += getMiddleGraphIsConnected(h2.getEdgeNode()->getGraphId(),h3.getEdgeNode()->getGraphId());
		
		choice == 0 ? selected = std::make_pair(2,0) : choice == 1 ? selected = std::make_pair(1,-1) : selected = std::make_pair(0,-2);
		}
	}
	
	int n0 = numVertices()-1;
	int n3 = numSimplices()-2;
	int nc = getMiddleGraphNumConnected() + selected.first;
	int nl = nc + getMiddleGraphEdgeSize() - getMiddleGraphNodeSize() + selected.second;
	
	//printf("DN3: %g\n",dn3);
	
	int coord1 = origin.getNext().getVertexNode()->getCoord();
	int coord2 = origin.getPrevious().getVertexNode()->getCoord();
	int coord3 = origin.getAdjacent2D().getPrevious().getVertexNode()->getCoord();
	
	int wb = 0; 
	(coord1 <= 4) ? wb++ : wb = wb;
	(coord2 <= 4) ? wb++ : wb = wb;
	(coord3 <= 4) ? wb++ : wb = wb;
	
	//double prob  = ((double)(GetCoord2Size() + wb - 1))/((double)(numSimplices()-1))/ 3.0 ;
	double prob = ((double)numSimplices())/((double)numSimplices()-2);
	
	double SB = getAction(n0,n3, nl, nc);
	
	int success = getProbabilityNow(move_probability,SA,SB,prob);
	
	//printf("PROBABILITY_move20: %g\t%g\t%d\n",prob, move_probability,(prob > 1 || move_probability < prob));
	//printf("probability Elements_move20: %g\t%g\t%g\t%g\t%d\t%g\t%g\t",-2*getK3(), + getK0(), + getBetaL() * selected.second,  + getBetaC() * selected.first,numSimplices(), dn3,prob);
	
	
	//printf("MOVE20 SUC?: %d\n",success);
	RESULT.second = success;
	
	//restore the middle graph
	
	if(info_t[0]) AddEdgeMiddleGraphPure( h1 );
	if(info_t[1]) AddEdgeMiddleGraphPure( h2 );
	if(info_t[2]) AddEdgeMiddleGraphPure( h3 );
	
	return RESULT;
}

#endif
