#ifndef INCLUDES_H
#define INCLUDES


#define ABSMAX_DC 50000 // SET THE ABSOLUTE MAXIMUM NUMBER


#include <vector>
#include <iostream>
#include <time.h>
#include <queue>
#include <sstream>
#include <fstream>
#include <string>
#include <set>
#include <stack>
#include <algorithm>
#include <assert.h>
#include <bitset>
#include <tuple>
#include <cstdint>
#include <numeric>

#include "core/globals.h"
#include "core/config.h"
#include "core/random.h"

//#include "boost/program_options.hpp"

// suppress "maybe-uninitialized" warning in dtree
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include "dtree/tree.h"
#include "dtree/tree-inl.h"
#pragma GCC diagnostic pop


#include "graphs/treetables.h"


#include "core/simp_struct.h"

#include "core/triangulation.h"

#include "graphs/tree_functions.h"
#include "graphs/triangulation_treefunctions.h"

//#include "triangulation.h"gh


#include "core/triangulation_iostream.h"


#include "graphs/triangulation_nodes.h"
#include "graphs/triangulation_coordination.h"
#include "graphs/triangulation_middlegraph.h"
#include "graphs/triangulation_checks.h"

#include "moves/moves.h"
#include "moves/graphmoves.h"

#include "moves/triangular_pillow.h"
#include "moves/triangular_pillow_inverse.h"
#include "moves/bistellar_flip.h"
#include "moves/bistellar_flip_inverse.h"
#include "moves/quadrangular_pillow.h"
#include "moves/quadrangular_pillow_inverse.h"

#include "mc/triangulation_initialize.h"
#include "mc/triangulation_action.h"
#include "mc/histogram.h"
#include "mc/mc.h"
#include "mc/profiles.h"
#include "mc/triangulation_adjacency_matrix.h"
#include "mc/triangulation_print.h"
#include "mc/tune.h"


#endif
