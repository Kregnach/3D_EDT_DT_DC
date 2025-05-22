#ifndef TRIANGULATION_ADJACENCY_MATRIX_H
#define TRIANGULATION_ADJACENCY_MATRIX_H


#define BUFFER_SIZE 1000

void Triangulation::printAdjacency(char * outfilename) {

    FILE * tmpfile = fopen("tmp.tmp","w");
    int num_nonzero = 0; // Initialize the count of non-zero elements to 0
    fprintf(tmpfile,"%%%%MatrixMarket matrix coordinate real symmetric\n"); // Print the Matrix Market header
    fprintf(tmpfile,"%d %d %d\n", numSimplices(), numSimplices(), num_nonzero); // Print the matrix dimensions and the number of non-zero elements (which is currently 0)
    for (int i = 0; i < numSimplices(); i++) {
        Simplex *node = getSimplex(i);
        std::vector<Simplex*> nbr;
        for (int j = 0; j < 4; j++) { nbr.push_back(node->getNeighbour(j)); }
        
        std::sort(nbr.begin(), nbr.end());
        std::vector<bool> counted(4, false); // Initialize a boolean vector to keep track of which neighbors have already been counted
        for (int j = 0; j < 4; j++) {
            if (counted[j]) { continue; } // Skip this neighbor if it has already been counted 
            int count = 1; // Initialize the count to 1 for the first appearance of the neighbor
            
            for (int k = j + 1; k < 4; k++) {
                if (nbr[j] == nbr[k]) { // If the neighbor appears multiple times, increase the count and mark it as counted
                    count++;
                    counted[k] = true;
                }
            }
            // Print the simplex indices and the count of the neighbor's appearances
            if(i+1 < nbr[j]->getId()+1) { // Use k instead of j here
                fprintf(tmpfile,"%d %d %g\n", i+1, nbr[j]->getId()+1, 1*double(count)/4);
                num_nonzero++;
            }
            counted[j] = true; // Mark the current neighbor as counted
        }
        
        //std::set<Simplex *> uniqueNeighbors(nbr.begin(), nbr.end());
		//int numUniqueNeighbors = uniqueNeighbors.size();
        //fprintf(tmpfile,"%d %d %d\n", i+1, i+1, numUniqueNeighbors);
        //num_nonzero++;
    }
    
    fclose(tmpfile);
/*    outfile = fopen(outfilename,"wa");
    
    fseek(outfile, 0, SEEK_SET);
    fprintf(outfile,"%%%%MatrixMarket matrix coordinate real symmetric\n");
    fprintf(outfile,"%d %d %d\n", numSimplices(), numSimplices(), num_nonzero); // Print the matrix dimensions and the number of non-zero elements	
    fclose(outfile);*/
    

    
    
    FILE * outfile;
    char path[1000];
    
    char buffer[1000];
    int line = 2, count;

	char newline[1000];
    sprintf(newline, "%d %d %d\n",
     numSimplices(),
     numSimplices(), 
     num_nonzero
	);
    

    /*  Open all required files */
    outfile  = fopen(outfilename, "w");
    tmpfile = fopen("tmp.tmp", "r"); 

    count = 0;
    while ((fgets(buffer, BUFFER_SIZE, tmpfile)) != NULL)
    {
        count++;

        /* If current line is line to replace */
        if (count == 2)
            fputs(newline, outfile);
        else
            fputs(buffer, outfile);
    }

    /* Close all files to release resource */
    fclose(tmpfile);
    fclose(outfile);

    /* Delete original source file */
   
}




void Triangulation::printAdjacency_color(char * outfilename) {

    FILE * outfile  = fopen(outfilename, "w");
    
    int num_nonzero = 0; // Initialize the count of non-zero elements to 0
    //fprintf(outfile,"%%%%MatrixMarket matrix coordinate real symmetric\n"); // Print the Matrix Market header
    //fprintf(outfile,"%d %d %d\n", numSimplices(), numSimplices(), num_nonzero); // Print the matrix dimensions and the number of non-zero elements (which is currently 0)
    for (int i = 0; i < numSimplices(); i++) {
        Simplex *node = getSimplex(i);
        for (int j = 0; j < 4; j++) {
        	Simplex *nbr = node->getNeighbour(j);
        	
        	if(i+1 < nbr->getId()+1) {
		    	int OPV_id = node->getNeighbourOppositeVertexID(j);
		    	
		    	TreeNode * face_j = node->getFaceNode(j);
		    	
		    	int inTree_j = isInDualSpanningTree(face_j->getHandle());
		    	
		    	fprintf(outfile,"%d\t%d\t%d\t%d\n", i+1, nbr->getId()+1, OPV_id,inTree_j);
		    }
		    	
		}
	}

    fclose(outfile);
}



#endif
