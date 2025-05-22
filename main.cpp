//#pragma once
#include "core/includes.h"

ConfigReader cfr;

void SetProposedFrequencies() {

	bool p1_0 = false, p2_0 = false ,p3_0 = false,pt_v = false,pt_d = false;
	
	if(probm1 == 0) p1_0 = true;
	if(probm2 == 0) p2_0 = true;
	if(probm3 == 0) p3_0 = true;
	if(probmtv == 0) pt_v = true;
	if(probmtd == 0) pt_d = true;


	double vm1 = proposed_rate[0] + proposed_rate[1] > 0 ? (double)(accepted_rate[0] + accepted_rate[1]) /  (double)(proposed_rate[0] + proposed_rate[1]) : 0;
	double vm2 = proposed_rate[2] + proposed_rate[3] > 0 ? (double)(accepted_rate[2] + accepted_rate[3]) /  (double)(proposed_rate[2] + proposed_rate[3]) : 0;
	double vm3 = proposed_rate[4] + proposed_rate[5] > 0 ? (double)(accepted_rate[4] + accepted_rate[5]) /  (double)(proposed_rate[4] + proposed_rate[5]) : 0;
	double vm4 = proposed_rate[6] > 0 ? (double)(accepted_rate[6]) / (double)(proposed_rate[6]) : 0;
	double vm5 = proposed_rate[7] > 0 ? (double)(accepted_rate[7]) / (double)(proposed_rate[7]) : 0;


	if(!p1_0) probm1 = 1/vm1;
	if(!p2_0) probm2 = 1/vm2;
	if(!p3_0) probm3 = 1/vm3;
	if(!pt_v) probmtv = 1/vm4;
	if(!pt_d) probmtv = 1/vm5;
	
	if(probm1 > 100) probm1 = 50.0;
	if(probm2 > 100) probm2 = 50.0;
	if(probm3 > 100) probm3 = 50.0;
	if(probmtv > 100) probmtv = 50.0;
	if(probmtd > 100) probmtd = 50.0;

	cfr.setDouble("m_triangular_pillow", probm1);	
	cfr.setDouble("m_bistellar_flip", probm2);
	cfr.setDouble("m_quadrangular_pillow", probm3);
	
	cfr.setDouble("m_vertextree", probmtv);
	cfr.setDouble("m_dualtree", probmtd);


	cfr.writeBack();	
}	


void ReadConfigParameters(std::string fname) {

	cfr.read(fname);

	kappa0 = cfr.getDouble("kappa0");
	kappa3 = cfr.getDouble("kappa3");
	
	betaC = cfr.getDouble("betac");
	betaL = cfr.getDouble("betal");
	
	epsilon = cfr.getDouble("sigma3");
	probm1 = cfr.getDouble("m_triangular_pillow");
	probm2 = cfr.getDouble("m_bistellar_flip");
	probm3 = cfr.getDouble("m_quadrangular_pillow");
	probmtv = cfr.getDouble("m_vertextree");
	probmtd = cfr.getDouble("m_dualtree");

	
	measurementsweeps = cfr.getInt("measurementsweeps");
	thermalizationsweeps = cfr.getInt("thermalizationsweeps");
	tuningsweeps = cfr.getInt("tuningsweeps");
	ksteps = cfr.getInt("ksteps");

	seed = cfr.getInt("seed");
	n3bar = cfr.getInt("volume");
	
	input = cfr.getInt("input");
	measure_data = cfr.getInt("measuresimplexdata");
	measure_hist = cfr.getInt("measurehistograms");
	measure_adj = cfr.getInt("measureadjacency");
	config_number = cfr.getInt("number");
	printfeq = cfr.getInt("printfreq");
	
	adjacency_matrix_version = cfr.getInt("adjacency_matrix_version");
	

	infile = cfr.getString("infile");
	outfile = cfr.getString("outfile");
	verbose = cfr.getInt("verbose");
	
	printf("CFG details:\n");
	
	printf("volume: %d\n",n3bar);
	printf("meas_sweeps: %d\n",measurementsweeps);
	printf("thermal_sweeps: %d\n",thermalizationsweeps);
	printf("ksteps_sweeps: %d.000\n",ksteps);
	
	printf("prints config files at after every %d sweeps\n",printfeq);
	
	printf("print_data: %d \n",measure_data);
	printf("print_histograms: %d \n",measure_hist);
	printf("print_adjacency_matrix: %d \n",measure_adj);
	
	std::string fID = cfr.getString("fileID");

    std::string  savefile, logfile;



}
	
int main(int argc, char **argv) {
	
	if (argc > 1) {
		fname = std::string(argv[1]);
		printf("%s\n", fname.c_str());
	}
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if(verbose) printf("################# DEFINE PARAMTERES ###########################\n");
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	ReadConfigParameters(fname);

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if(verbose) printf("################# INITIALIZE SIMULATION ###########################\n");
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    if(seed == -1) seed = rand();

    Xoshiro256PlusPlus rng(seed);// initialize random number generator
    
   // INITIALIZE TRIANGULATION AND SETTINGS
    Triangulation triangulation;

    if( input ) {
        std::ifstream file(infile.c_str());
        if( !file || !triangulation.loadFromStream(file) ) { 
        if(verbose) 
        	std::cout << "Error reading " << infile << "\n"; return 1; }
        
        if(verbose) std::cout << "Triangulation loaded from " << infile << "\n"; 
    }
    else { triangulation.initializeMinimal(); std::cout << "Minimal triangulation loaded.\n"; }
    

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if(verbose) printf("################# Initial volume boost ###########################\n");
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	if(kappa3 == -1) kappa3 = 0.25*(kappa0 + std::sqrt(4.0 + kappa0*kappa0)) + 2.4;  // if k3 is set to tuning.....
	
	//if no input, we start from minimal --> boost + thermalize
	if( !input ) for(int j = 0; j < int(n3bar/10); j++) mov = MCMoves(triangulation, ksteps*10, rng); // initial boost, helps in k3 thermalization 

	//then we perform the tuning when we reached the volume, to gauge the frequencies
	for(int sweep = 0; sweep < tuningsweeps ; sweep++) {
		TuneK3(triangulation);	
		if(verbose) printf("i: %d/%d\t k0:%g\t bC:%g\t bL: %g\t k3 :%g\t N3: %d\t",sweep,thermalizationsweeps, kappa0, betaC, betaL,kappa3,triangulation.numSimplices());
			
		mov = MCMoves(triangulation, ksteps*1000, rng);

		if(verbose) {
			printf("moves: \t");
			for(int i=0;i<9;i++) printf("%ld\t",mov[i]); printf("\n"); 
			for(int i=0;i<8;i++) moves[i] += mov[i];
		}

		if(sweep == tuningsweeps-1) SetProposedFrequencies();
	}


	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if(verbose) printf("################# Run SIMULATION ###########################\n");
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	//then we thermalize and measure
	for(int sweep = 0; sweep < thermalizationsweeps + measurementsweeps ; sweep++) {
	
		if(sweep < thermalizationsweeps) { 
			TuneK3(triangulation);	
			if(verbose) printf("i: %d/%d\t k0:%g\t bC:%g\t bL: %g\t k3: %g\t N3: %d\t",sweep,thermalizationsweeps, kappa0, betaC, betaL,kappa3,triangulation.numSimplices());
				 
		}	

		mov = MCMoves(triangulation, ksteps*1000, rng);

		if(verbose) {
			printf("\nmoves: \t");
			for(int i=0;i<9;i++) printf("%ld\t",mov[i]); printf("\n"); 
			for(int i=0;i<8;i++) moves[i] += mov[i];
		}
		
		if( (sweep % printfeq) == 0) {
			std::ofstream file2(outfile.c_str());
			triangulation.saveToStream(file2);
			
			triangulation.printAdjMatrix(sweep);
		}

		triangulation.printSimplexData();

	}

	std::ofstream file2(outfile.c_str());
	triangulation.saveToStream(file2);
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if(verbose) printf("################# Finished SIMULATION ###########################\n");
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	
	return 0;
}
