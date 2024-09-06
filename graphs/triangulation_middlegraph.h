#ifndef TRIANGULATION_MIDDLEGRAPH_H
#define TRIANGULATION_MIDDLEGRAPH_H

#include "triangulation.h"



void Triangulation::AddNodeMiddleGraph(TreeNode * node) {
		node->setInMiddleGraph(true); // node should know if its member of the middle graph
		
		int nextGraphInd = getNextInactiveGraphInd(); // get the index of next inactive graph node 
		int nextGraphId = getNextInactiveGraphId(); // get the index of next inactive graph node 
		
		//printf("ADD Node Mid Graph: %d\n",nextGraphId);
		graph_nodes_[nextGraphInd] = node; // add new node as the nextGraphId-th element of graph_nodes
		node->setGraphId(nextGraphId); // set graph ID of newnode as size of middle graph (before append)
		setNextInactiveGraphInd(nextGraphInd+1); // set the Index of the next inactive graph index as old value
    }

    void Triangulation::DelNodeMiddleGraph(TreeNode * node) {
    	if( (node->getDimension() == 1 || node->getDimension() == 2) && node->getInMiddleGraph() == true) {

	    	//printf("\nDELETE NODE: %d DIM: %d A_ind: %d A_ID: %d\n",node->getGraphId(),node->getDimension(),getNextInactiveGraphInd(),getNextInactiveGraphId());
	    	
			node->setInMiddleGraph(false);
			
			int actGraphId = node->getGraphId(); // graphID of the actual node
			//printf("Delete Node Mid Graph: %d\n",actGraphId);
			setNextInactiveGraphInd(getNextInactiveGraphInd()-1); // decrement the free element "pointer", aka. index (!!! points at wrong ID yet !!!)
			setNextInactiveGraphId(actGraphId);      // set the value of inactive[index] to be the actual ID , thus this is the new free
			node->setGraphId(ABSMAX_DC-1);
			//printf("\nDELETED NODE: %d DIM: %d N_ind: %d next_ID: %d\n",node->getGraphId(),node->getDimension(),getNextInactiveGraphInd(),getNextInactiveGraphId());
		}
	}

	
    int Triangulation::getMiddleGraphNodeSize() { return next_inactive_graph_ind_; }// next_inactive_graph_ind_ actually gives us the size of the graph
    
    
    int Triangulation::getMiddleGraphEdgeSize() { return mid_graph_.GetNumOfEdges(); }// gives the number of edges in the middle graph

    int Triangulation::getMaxGraphCoord() {
    	int max = 0;
    	
		for(int i = 0 ; i < nodes_[1].size(); i++) {
			if(nodes_[1][i]->getInMiddleGraph()) { 
				if(nodes_[1][i]->getGraphCoord() > max) { 
					max = nodes_[1][i]->getGraphCoord(); 
				} 
			}
		}
    	return max;
    }

    
    bool Triangulation::getHandleMiddleGraphIsConnected(Handle h) {
    	int fna = h.getFaceNode()->getGraphId();
		int enb = h.getEdgeNode()->getGraphId();
    	
    	return mid_graph_.IsConnected(fna,enb);
    }
    
    bool Triangulation::getHandleMiddleGraphHasEdge(Handle h) {
    	int fna = h.getFaceNode()->getGraphId();
		int enb = h.getEdgeNode()->getGraphId();
    	
    	return mid_graph_.HasEdge((UndirectedEdge(fna,enb)));
    }
    
    bool Triangulation::getMiddleGraphIsConnected(int u, int v) { return mid_graph_.IsConnected(u,v); }
    bool Triangulation::getMiddleGraphHasEdge(int u, int v) { return mid_graph_.HasEdge((UndirectedEdge(u,v))); }
    
    
    void Triangulation::PrintMiddleGraph() {
		for(auto s : simplices_ ) {
			for(int i=0;i<4;++i) {
		   		if( s->isInDualSpanningTree(i) ) continue;
				
				for(int j=0;j<3;++j) {
			        if( s->isInSpanningTree(i,j) ) continue;

					int fna = s->getEdge(i,j).getFaceNode()->getGraphId();
	    			int enb = s->getEdge(i,j).getEdgeNode()->getGraphId();
	    			
	    			int fnaid = s->getEdge(i,j).getFaceNode()->getId();
	    			int enbid = s->getEdge(i,j).getEdgeNode()->getId();
	    			
	    			int num_MGnodes = getMiddleGraphNodeSize();
	    			int num_MGedges = getMiddleGraphEdgeSize();
	    			
					int nLoop = getMiddleGraphNumConnected() + num_MGedges - num_MGnodes;
					
					printf("Graph Nodes:(%d,%d)\t InMidG: (%d,%d)\tHasEde?: %d\t Connected?: %d || numV: %d\t numE: %d,\tnumc: %d\t nl: %d\n",fna,enb,s->getEdge(i,j).getFaceNode()->getInMiddleGraph(),s->getEdge(i,j).getEdgeNode()->getInMiddleGraph(),getMiddleGraphHasEdge(fna,enb),getMiddleGraphIsConnected(fna,enb),num_MGnodes,num_MGedges,getMiddleGraphNumConnected(),nLoop);
				}
			}
		}
		
		printf("\n");
	}
	
	void Triangulation::setHandleInMiddleGraph(Handle h) { if(!getHandleMiddleGraphHasEdge(h)) AddEdgeMiddleGraph(h); }
	void Triangulation::unsetHandleInMiddleGraph(Handle h) { if(getHandleMiddleGraphHasEdge(h)) DeleteEdgeMiddleGraph(h); }

	void Triangulation::AddEdgeMiddleGraph(Handle h) { 
		
		if( ( h.getFaceNode()->getInMiddleGraph() ) && ( h.getEdgeNode()->getInMiddleGraph() ) ) {
			if(!getHandleMiddleGraphIsConnected(h) ) decrementNumConnected();
		}
		else if ( ( !h.getFaceNode()->getInMiddleGraph() ) && ( !h.getEdgeNode()->getInMiddleGraph() ) ) incrementNumConnected();
		
		
		if(h.getFaceNode()->getInMiddleGraph() == false) AddNodeMiddleGraph(h.getFaceNode());
		if(h.getEdgeNode()->getInMiddleGraph() == false) AddNodeMiddleGraph(h.getEdgeNode());

		int fna = h.getFaceNode()->getGraphId();
		int enb = h.getEdgeNode()->getGraphId();

		mid_graph_.AddEdge(UndirectedEdge(fna,enb));
		
		h.getFaceNode()->incrementGraphCoord();
		h.getEdgeNode()->incrementGraphCoord();
	}
	
    void Triangulation::DeleteEdgeMiddleGraph(Handle h) {
    	int fna = h.getFaceNode()->getGraphId();
		int enb = h.getEdgeNode()->getGraphId();
		
		if ( (h.getFaceNode()->getGraphCoord() == 1) && (h.getEdgeNode()->getGraphCoord() == 1) )  decrementNumConnected();
		
    	mid_graph_.DeleteEdge(UndirectedEdge(fna,enb));
    	h.getFaceNode()->decrementGraphCoord();
		h.getEdgeNode()->decrementGraphCoord();
		
		if(h.getFaceNode()->getGraphCoord() == 0) DelNodeMiddleGraph(h.getFaceNode());
		if(h.getEdgeNode()->getGraphCoord() == 0) DelNodeMiddleGraph(h.getEdgeNode());
		
		if( (h.getFaceNode()->getInMiddleGraph()) && (h.getEdgeNode()->getInMiddleGraph()) ) {
			if( !getHandleMiddleGraphIsConnected(h) ) incrementNumConnected();
		}
    }

    void Triangulation::AddEdgeMiddleGraphPure(Handle h) { // Useful for temporal elimination of connections (CAREFULLY !!! )
		if (!getHandleMiddleGraphHasEdge(h)) {
			int fna = h.getFaceNode()->getGraphId();
			int enb = h.getEdgeNode()->getGraphId();
			mid_graph_.AddEdge(UndirectedEdge(fna,enb));
		}
	}
	void Triangulation::DeleteEdgeMiddleGraphPure(Handle h) { // Useful for temporal elimination of connections (CAREFULLY !!! )
		if (getHandleMiddleGraphHasEdge(h)) {
			int fna = h.getFaceNode()->getGraphId();
			int enb = h.getEdgeNode()->getGraphId();
		
			mid_graph_.DeleteEdge(UndirectedEdge(fna,enb));
		}
	}

#endif
