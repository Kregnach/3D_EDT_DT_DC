#ifndef PROFILES_H
#define PROFILES_H



std::vector<int> hierarchicalprofile(Triangulation & triangulation, Handle start) {
    assert(!isInDualSpanningTree(start));
    assert(isInSpanningTree(start));
    std::set<int> visited;
    std::vector<int> profile;
    profile.reserve(2*triangulation.numSimplices()+4);
    Handle current = start;
    int d = 0;
    do {
        if( visited.find(current.getAdjacent3D().uniqueId()) == visited.end() ) {
            d++;
            visited.insert(current.uniqueId());
        } 
        else d--;
        
        profile.push_back(d);
        
        current = current.getNext();
        while( !isInSpanningTree(current) ) {
            current = current.getAdjacent2D();

            while( isInDualSpanningTree(current) ) current = current.getAdjacent3D().getAdjacent2D();
            
            current = current.getNext();
        }
    } while (current != start);
    assert(d == 0);
    assert( (int)profile.size() == 2*triangulation.numSimplices()+4);
    return profile;
}
std::vector<int> apollonianprofile(Triangulation & triangulation, Handle start) {
    assert(!isInDualSpanningTree(start));
    assert(isInSpanningTree(start));
    std::set<int> visited;
    std::vector<int> profile;
    profile.reserve(2*triangulation.numSimplices()+4);
    Handle current = start;
    int d = 0;
    do {
        Handle adj = current.getAdjacent2D();
        while( isInDualSpanningTree(adj) ) adj = adj.getAdjacent3D().getAdjacent2D();
        
        if( visited.find(adj.uniqueId()) == visited.end() ) {
            d++;
            visited.insert(current.uniqueId());
        } 
        else d--;
        
        profile.push_back(d);
        
        current = current.getNext();
        while( !isInSpanningTree(current) ) {
            current = current.getAdjacent2D();
            while( isInDualSpanningTree(current) ) current = current.getAdjacent3D().getAdjacent2D();
            
            current = current.getNext();
        }
    } while (current != start);
    assert(d == 0);
    assert( (int)profile.size() == 2*triangulation.numSimplices()+4);
    return profile;
}


std::vector<int> outerplanarprofile(Triangulation & triangulation, Handle start) {
    assert(!isInDualSpanningTree(start));
    assert(isInSpanningTree(start));
    std::set<int> visited;
    std::vector<int> profile;
    profile.reserve(2*triangulation.numSimplices()+4);
    Handle current = start;
    int d = 0;
    do {      
        current = current.getNext();
        while( !isInSpanningTree(current) ) {
            Handle adj = current.getAdjacent2D();
            
            while( isInDualSpanningTree(adj) ) adj = adj.getAdjacent3D().getAdjacent2D();
            
            if( visited.find(current.uniqueId()) == visited.end() ) {
                d++;
                visited.insert(adj.uniqueId());
            } 
            else d--;
            
            current = adj.getNext();
        }
        profile.push_back(d);
    } while (current != start);
    assert(d == 0);
    assert( (int)profile.size() == 2*triangulation.numSimplices()+4);
    return profile;
}


template<typename RNG>
void saveProfilesToStream(Triangulation & triangulation, std::ostream & stream, RNG & rng) {
    Handle start;
    do { start = randomEdge(triangulation,rng); } 
    	while( isInDualSpanningTree(start) || !isInSpanningTree(start));
    
    std::vector<int> hprofile = hierarchicalprofile(triangulation,start);
    std::vector<int> aprofile = apollonianprofile(triangulation,start);
    std::vector<int> oprofile = outerplanarprofile(triangulation,start);
    stream << hprofile.size() << "\n";
    
    for( auto x : hprofile) stream << x << "\n";
    for( auto x : aprofile) stream << x << "\n";
    for( auto x : oprofile) stream << x << "\n";
}

std::pair<std::vector<int>,int> getCanonicalProfile(Triangulation & triangulation) {
    
    std::vector<int> canonical_profile;
    int num = 0;
    for(int i=0;i<triangulation.numSimplices();++i) {
        for(int j=0;j<4;++j) {
            for(int k=0;k<3;++k) {
                Handle start = triangulation.getSimplex(i)->getEdge(j,k);
                
                if( isInDualSpanningTree(start) || !isInSpanningTree(start)) continue;
                    
                std::vector<int> profile = hierarchicalprofile(triangulation,start);
                std::vector<int> ap = apollonianprofile(triangulation,start);
                profile.insert(profile.end(),ap.begin(),ap.end());
                std::vector<int> op = outerplanarprofile(triangulation,start);
                profile.insert(profile.end(),op.begin(),op.end());
               
                if( profile == canonical_profile ) num++;
                
                if( canonical_profile.size() == 0 || std::lexicographical_compare(profile.begin(),profile.end(),
                    canonical_profile.begin(),canonical_profile.end())) {
                    canonical_profile = profile;
                    num = 1;
                }
            }
        }
    }
    return{canonical_profile,num};
}


#endif
