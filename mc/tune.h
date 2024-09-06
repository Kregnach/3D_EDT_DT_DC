#ifndef TUNE_H
#define TUNE_H

template<typename RNG> std::vector<uint64_t> MCMoves( Triangulation & triangulation, uint64_t target, RNG & rng ) {
    std::vector<uint64_t> moves(9,0);

    for(uint64_t i=0;i<target;++i) moves[ MonteCarloGraphMove(triangulation,rng) ]++;

    return moves;
}

void TuneK3(Triangulation & triangulation) { //very simple, but seems to work well
    double delta_k3 = 0.00001;
	double ratio = 100;
	
	int actualVolume = triangulation.numSimplices();
	
	double border_far = n3bar*0.5; 
	double border_close = n3bar*0.05;
	double border_vclose = n3bar*0.01; 
	double border_vvclose = n3bar*0.001; 
	
	if ((n3bar - actualVolume)      > border_far)      kappa3 -= delta_k3*ratio*400;
	else if ((n3bar - actualVolume) < -border_far)     kappa3 += delta_k3*ratio*400;
	else if ((n3bar - actualVolume) > border_close)    kappa3 -= delta_k3*100;
	else if ((n3bar - actualVolume) < -border_close)   kappa3 += delta_k3*100;
	else if ((n3bar - actualVolume) > border_vclose)   kappa3 -= delta_k3*10;
	else if ((n3bar - actualVolume) < -border_vclose)  kappa3 += delta_k3*10;
	else if ((n3bar - actualVolume) > border_vvclose)  kappa3 -= delta_k3*2;
	else if ((n3bar - actualVolume) < -border_vvclose) kappa3 += delta_k3*2;
			
}

#endif
