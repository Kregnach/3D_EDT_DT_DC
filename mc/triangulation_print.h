#ifndef TRIANGULATION_PRINT_H
#define TRIANGULATION_PRINT_H


void Triangulation:: printSimplexData() {
	if(measure_data) {
		FILE * datafile;
		char loopfilename[1000];
		if(config_number == 0) {
			
			sprintf(loopfilename, "data-%g-%g-%g-%d.out",
				kappa0,
				betaL,
				betaC,
				n3bar
			);
		}
		else {
			sprintf(loopfilename, "data-%g-%g-%g-%d-%d.out",
				kappa0,
				betaL,
				betaC,
				n3bar,
				config_number
			);
		}

		datafile	= fopen(loopfilename, "a");
		int nComp = getMiddleGraphNumConnected();
		int nNodes = getMiddleGraphNodeSize();
		int nEdges = getMiddleGraphEdgeSize();
		int nLoop = nComp + nEdges - nNodes;
		int nVert = numVertices();
		int nSimp = numSimplices();
		int maxC = getMaxGraphCoord();
		int maxV = getMaxVertexCoord();
		int maxE = getMaxEdgeCoord();
		int coord2Size = GetCoord2Size();
		int coord3Size = GetCoord3Size();
		int edgecoord2Size = GetEdgeCoord2Size();
		
		fprintf(datafile,"%d\t%d\t%g\t%d\t%d\t%d\t%d\t%d\t%d\t%d \t%d\t%d\t%d\n",nVert,nSimp,kappa3,nNodes,nEdges,nComp,nLoop,maxC,maxV,maxE,coord2Size,coord3Size,edgecoord2Size);
		fclose(datafile);
	}
}

void Triangulation:: PrintHistogram(std::vector<int> hist,char* filename, FILE * file) {
	file = fopen(filename, "a");
	
	for(int i = 0 ; i<hist.size(); i++) fprintf(file,"%d\t",hist[i]);
	
	fprintf(file,"\n");
	fclose(file);
}

void Triangulation:: printHistData() {
	if(measure_hist) {
	
		FILE *vcoordfile, *lcoordfile, *gcoordfile;
		char lcoordfilename[100],vcoordfilename[100],gcoordfilename[100];
		
		if(config_number == 0) {
			sprintf(lcoordfilename, "lcoord-%g-%g-%g-%d.out",
				kappa0,
				betaL,
				betaC,
				n3bar
			);

			sprintf(vcoordfilename, "vcoord-%g-%g-%g-%d.out",
				kappa0,
				betaL,
				betaC,
				n3bar
			);
			
			sprintf(gcoordfilename, "gcoord-%g-%g-%g-%d.out",
				kappa0,
				betaL,
				betaC,
				n3bar
			);
		}
		else {
			sprintf(lcoordfilename, "lcoord-%g-%g-%g-%d-%d.out",
				kappa0,
				betaL,
				betaC,
				n3bar,
				config_number
			);

			sprintf(vcoordfilename, "vcoord-%g-%g-%g-%d-%d.out",
				kappa0,
				betaL,
				betaC,
				n3bar,
				config_number
			);
			
			sprintf(gcoordfilename, "gcoord-%g-%g-%g-%d-%d.out",
				kappa0,
				betaL,
				betaC,
				n3bar,
				config_number
			);
		}

		std::vector<int> lcoord_hist = createHistogram(getNodeCoordVector(1), 0, n3bar, 1, n3bar);
		std::vector<int> vcoord_hist = createHistogram(getNodeCoordVector(0), 0, n3bar, 1, n3bar);
		std::vector<int> gcoord_hist = createHistogram(getGraphCoordVector(), 0, n3bar, 1, n3bar);

		PrintHistogram(vcoord_hist,vcoordfilename,vcoordfile);
		PrintHistogram(lcoord_hist,lcoordfilename,lcoordfile);
		PrintHistogram(gcoord_hist,gcoordfilename,gcoordfile);
	}
}

void Triangulation:: printAdjMatrix(int sweepcounter) {
	if(measure_adj) {
		char adjacencymatrixfilename[1000];
		if(adjacency_matrix_version) {
			//FILE * adjFile;
			if(config_number == 0) {
				sprintf(adjacencymatrixfilename, "adj-%g-%g-%g-%d-%d.mtx",
					kappa0,
					betaL,
					betaC,
					n3bar,
					sweepcounter
				);
			}
			else {
			sprintf(adjacencymatrixfilename, "adj-%g-%g-%g-%d-%d-%d.mtx",
					kappa0,
					betaL,
					betaC,
					n3bar,
					config_number,
					sweepcounter
				);
			}
		//if(sweep%10 == 0) {
			//adjFile = fopen(adjacencymatrixfilename, "w");
			//);
			printAdjacency(adjacencymatrixfilename);
		}
		else {
			//FILE * adjFile;
			
			if(config_number == 0) {
				sprintf(adjacencymatrixfilename, "adj_color-%g-%g-%g-%d-%d.mtx",
					kappa0,
					betaL,
					betaC,
					n3bar,
					sweepcounter
				);
			}
			else {
				sprintf(adjacencymatrixfilename, "adj_color-%g-%g-%g-%d-%d-%d.mtx",
					kappa0,
					betaL,
					betaC,
					n3bar,
					config_number,
					sweepcounter
				);
			
			}
		//if(sweep%10 == 0) {
			//adjFile = fopen(adjacencymatrixfilename, "w");
			//);
			printAdjacency_color(adjacencymatrixfilename);
		}
	}
}


#endif
