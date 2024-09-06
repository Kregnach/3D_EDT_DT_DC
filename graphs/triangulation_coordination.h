#ifndef TRIANGULATION_COORDINATION_H
#define TRIANGULATION_COORDINATION_H





int Triangulation:: GetEdgeCoord2Size() { return coord_2_edges_.size(); }
int Triangulation:: GetCoord2Size() { return coord_2_nodes_.size(); }
int Triangulation:: GetCoord3Size() { return coord_3_nodes_.size(); }

void Triangulation:: AddEdgeCoord2(TreeNode * node) {
	coord_2_edges_.push_back(node);
	node->setCoordIndex(coord_2_edges_.size() - 1);
}


void Triangulation:: RemoveEdgeCoord2(TreeNode * node) {
	int index = node->getCoordIndex();
	
	coord_2_edges_[index] = coord_2_edges_.back();
	coord_2_edges_[index]->setCoordIndex(index);
	node->setCoordIndex(-1);

	coord_2_edges_.pop_back();
}


void Triangulation:: AddVertexCoord2(TreeNode * node) {
	coord_2_nodes_.push_back(node);
	node->setCoordIndex(coord_2_nodes_.size()-1);
}

void Triangulation:: RemoveVertexCoord2(TreeNode * node) {
	int index = node->getCoordIndex();

	coord_2_nodes_[index] = coord_2_nodes_.back();
	coord_2_nodes_[index]->setCoordIndex(index);
	node->setCoordIndex(-1);

	coord_2_nodes_.pop_back();
}

void Triangulation:: AddEdgeCoord3(TreeNode * node) {
	//if( isInSpanningTree(node->getHandle()) ) {
		coord_3_nodes_.push_back(node);
		node->setCoordIndex(coord_3_nodes_.size()-1);
		
	//}
}

void Triangulation:: RemoveEdgeCoord3(TreeNode * node) {
	//if( isInSpanningTree(node->getHandle()) ) {
		int index = node->getCoordIndex();
		
		coord_3_nodes_[index] = coord_3_nodes_.back();
		coord_3_nodes_[index]->setCoordIndex(index);
		node->setCoordIndex(-1);

		coord_3_nodes_.pop_back();
}

void Triangulation:: IncrementCoord(TreeNode * node, int dim) {
	if(dim == 0) {
		if(node->getCoord() == 1) AddVertexCoord2(node);
		else if(node->getCoord() == 2) RemoveVertexCoord2(node);
	}
	else if(dim == 1) {
		if (node->getCoord() == 1) AddEdgeCoord2(node);
		else if(node->getCoord() == 2) {
			RemoveEdgeCoord2(node);
			AddEdgeCoord3(node);
		}
		else if(node->getCoord() == 3) RemoveEdgeCoord3(node);
	}
	node->incrementCoord();
}

void Triangulation:: DecrementCoord(TreeNode * node, int dim) {
	if(dim == 0) {
		if(node->getCoord() == 2) RemoveVertexCoord2(node);
		else if(node->getCoord() == 3) AddVertexCoord2(node);
	}
	else if(dim == 1) {
		if(node->getCoord() == 2) RemoveEdgeCoord2(node);
		else if(node->getCoord() == 3) {
			RemoveEdgeCoord3(node);
			AddEdgeCoord2(node);
		}
		else if(node->getCoord() == 4) AddEdgeCoord3(node);
	}
	node->decrementCoord();
}

#endif


