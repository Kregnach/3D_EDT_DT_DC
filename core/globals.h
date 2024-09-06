#pragma once


int proposed_rate[8] = {0,0,0,0,0,0,0,0};
int accepted_rate[8] = {0,0,0,0,0,0,0,0};


double kappa3 = 0;
double kappa0 = 0;
double betaL = 0;
double betaC = 0;
int n3bar = 0;

double probm1 = 0;
double probm2 = 0;
double probm3 = 0;
double probmt = 0;

double epsilon = 0;

int config_number = 0;


int measurementsweeps = 0;
int thermalizationsweeps = 0;

int tuningsweeps = 0;
int ksteps = 0;

std::vector<uint64_t> moves(9,0);
std::vector<uint64_t> mov(9,0);


int selfadj =0;
int degeneracy = 0;
int dualspanningtree = 0;
int spanningtree = 0;
int middlegraph = 0;
int seed = 0;


int input = 0;
int measure_data = 0;
int measure_hist =0;
int measure_adj =0;

int printfeq = 0;

int adjacency_matrix_version = 0;

std::string infile, outfile, fname;
