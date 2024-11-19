#ifndef TRIANGULATION_ACTION_H
#define TRIANGULATION_ACTION_H

bool Triangulation::getProbabilityNow(double moveprob, double SA, double SB, double prob) {
	double proba = prob * std::exp( SA - SB );

	 if ( proba > 1 || moveprob < proba ) return true;

	 return false;
}




void Triangulation::SetAction() {

	int n0 = numVertices();
	int n3 = numSimplices();
	int nc = getMiddleGraphNumConnected();
	int nl = nc + getMiddleGraphEdgeSize() - getMiddleGraphNodeSize();
}

double Triangulation::getAction(int n0,int n3, int nl, int nc) { 	

	double action = kappa3 * n3
				  - kappa0 * n0
	              + epsilon * ( (n3 - n3bar) * (n3 -n3bar) ) 
	              - betaL * nl 
	              - betaC * nc;
return action; }

double Triangulation::getActionNow() {
	int n0 = numVertices();
	int n3 = numSimplices();
	int nc = getMiddleGraphNumConnected();
	int nl = nc + getMiddleGraphEdgeSize() - getMiddleGraphNodeSize();
	
	double action = kappa3 * n3
				  - kappa0 * n0
	              + epsilon * ( (n3 - n3bar) * (n3 - n3bar) ) 
	              - betaL * nl 
	              - betaC * nc;
return action; }



void Triangulation:: printActionDetails() {
	int n0 = numVertices();
	int n3 = numSimplices();
	int nc = getMiddleGraphNumConnected();
	int nl = nc + getMiddleGraphEdgeSize() - getMiddleGraphNodeSize();

	double action =  kappa3 * n3
				  - kappa0 * n0
		          + epsilon * ( (n3 -n3bar) * (n3 - n3bar) ) 
		          - betaL * nl 
		          - betaC * nc;

	printf("n0: %d\t n3: %d\t nc: %d\t nl: %d\t ation: %g\n",n0,n3,nc,nl,action);
}

void Triangulation:: PrintTriangulationSize() {
	printf("Triangulation Size: N0: %ld\tN1: %ld\tN2: %ld\tN3: %ld\n",nodes_[0].size(),nodes_[1].size(),nodes_[2].size(),nodes_[3].size());
}

#endif
