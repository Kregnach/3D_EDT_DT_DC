#ifndef MOVES_H
#define MOVES_H

#include "triangulation.h"


Handle Triangulation::expandTriangle(Handle handle, double dual_tree_rooting, double tree_rooting) {
		Handle nbr = handle.getAdjacent3D();

		bool indualtree = false;
        //assert(isDualSpanningTree());
		if( isInDualSpanningTree( handle ) ) {
			setInDualSpanningTree( handle, false );
			indualtree = true;
		    //assert( dualSpanningTreeAgrees() );
		}
                
        Simplex * s1 = newSimplex();
        Simplex * s2 = newSimplex();
        for(int i=0;i<3;++i) { setAdjacent(s1->getEdge(0,i).getAdjacent2D(), s2->getEdge(0,(3-i)%3).getAdjacent2D() ); }
        
        setAdjacent(s1->getEdge(0,0),handle);
        setAdjacent(s2->getEdge(0,0),nbr);
        for(int i=0;i<3;++i) {
            s1->getEdge(0,i).setVertexNode(s1->getEdge(0,i).getAdjacent3D().getNext().getVertexNode());
            s1->getEdge(0,i).setEdgeNode(s1->getEdge(0,i).getAdjacent3D().getEdgeNode());
            s2->getEdge(0,i).setVertexNode(s2->getEdge(0,i).getAdjacent3D().getNext().getVertexNode());
            s2->getEdge(0,i).setEdgeNode(s2->getEdge(0,i).getAdjacent3D().getEdgeNode());
            TreeNode * newedge = newNode(1);
            
            IncrementCoord(newedge,1);
            IncrementCoord(newedge,1);
            
            s1->getEdge(0,i).getAdjacent2D().getPrevious().setEdgeNode(newedge);
            s2->getEdge(0,(3-i)%3).getAdjacent2D().getNext().setEdgeNode(newedge);
            TreeNode * newface = newNode(2);
            s1->getEdge(0,i).getAdjacent2D().setFaceNode(newface);
            s2->getEdge(0,(3-i)%3).getAdjacent2D().setFaceNode(newface);
        }
        TreeNode * vertex = newNode(0);
        
        IncrementCoord(vertex,0);
        IncrementCoord(vertex,0);
        
        setVertexNode(s1->getEdge(0,0).getAdjacent2D().getPrevious(),vertex);
        s1->setFaceNode(0,handle.getFaceNode());
        setFaceNode(nbr,newNode(2));
        
        
        //increment coordination number of the 3 old edge and 3 old vertex
        
        IncrementCoord(handle.getEdgeNode(),1);
        IncrementCoord(handle.getEdgeNode(),1);
        IncrementCoord(handle.getNext().getEdgeNode(),1);
        IncrementCoord(handle.getNext().getEdgeNode(),1);
        IncrementCoord(handle.getPrevious().getEdgeNode(),1);
        IncrementCoord(handle.getPrevious().getEdgeNode(),1);
        
        
        IncrementCoord(handle.getVertexNode(),0);
        IncrementCoord(handle.getVertexNode(),0);
        IncrementCoord(handle.getNext().getVertexNode(),0);
        IncrementCoord(handle.getNext().getVertexNode(),0);
        IncrementCoord(handle.getPrevious().getVertexNode(),0);
        IncrementCoord(handle.getPrevious().getVertexNode(),0);

        
        int dir = (int)(3*tree_rooting);
        setInSpanningTree(s1->getEdge(0,dir).getAdjacent2D().getNext());
        // update spanning tree info on boundary edges
        for(int i=0;i<3;++i) {
            if( isInSpanningTree(s1->getEdge(0,i).getAdjacent3D()) ) {
                s1->getEdge(0,i).setInSpanningTree();
                s1->getEdge(0,i).getAdjacent2D().getAdjacent3D().setInSpanningTree();
            }
        }

        if( indualtree ) {
            // 3 options
            setInDualSpanningTree( handle );
            setInDualSpanningTree( nbr );
            int dir = (int)(3*dual_tree_rooting);
            setInDualSpanningTree( s1->getEdge(0,dir).getAdjacent2D() ); 
            //assert( dualSpanningTreeAgrees() );
            //assert( isDualSpanningTree() );
        } 
        else {
            // 7 options
            int dir = (int)(7*dual_tree_rooting);
            //std::cout << "move02: " << handle.getSimplex()->getId() << " " << nbr.getSimplex()->getId() << " " << s1->getId() << " " << s2->getId() << "\n";
            if( dir < 3 ) {
                setInDualSpanningTree( handle );
                //assert( dualSpanningTreeAgrees() );
                setInDualSpanningTree( s1->getEdge(0,dir).getAdjacent2D() ); 
                //assert( dualSpanningTreeAgrees() );
            } 
            else if( dir < 6 ) {
                setInDualSpanningTree( nbr );
                //assert( dualSpanningTreeAgrees() );
                setInDualSpanningTree( s2->getEdge(0,dir-3).getAdjacent2D() ); 
                //assert( dualSpanningTreeAgrees() );
            } 
            else{
                setInDualSpanningTree( handle );
                //assert( dualSpanningTreeAgrees() );
                setInDualSpanningTree( nbr );
                //assert( dualSpanningTreeAgrees() );
            }
            //assert( isDualSpanningTree() );
        }

        // return a handle with the origin at the new vertex
        return s1->getEdge(0,0).getAdjacent2D().getPrevious();
    } // END expandTriangle

Handle Triangulation::unExpandTriangle(Handle origin) {
        Handle adj1 = origin.getNext().getAdjacent2D().getAdjacent3D();
        Handle adj2 = origin.getNext().getAdjacent3D().getAdjacent2D().getAdjacent3D();
                
        bool indualtree = false;
                 
        
        indualtree =  isInDualSpanningTree(adj1) && isInDualSpanningTree(adj2) &&
             (isInDualSpanningTree(origin) || 
              isInDualSpanningTree(origin.getAdjacent2D()) || 
              isInDualSpanningTree(origin.getPrevious().getAdjacent2D()));
        setInDualSpanningTree(adj1, false);
        setInDualSpanningTree(adj2, false);
        setInDualSpanningTree(origin, false);
        setInDualSpanningTree(origin.getAdjacent2D(), false);
        setInDualSpanningTree(origin.getPrevious().getAdjacent2D(), false);
        
        
        Handle edge = origin;
        for(int i=0;i<3;++i) {
            if( isInSpanningTree(edge) ) { setInSpanningTree(edge,false); }
           
            edge = edge.getAdjacent2D().getNext();
        }
        
        Handle current = origin.getNext().getAdjacent2D();
        for(int i=0;i<3;++i) {
            current.getVertexNode()->setHandle(current.getAdjacent3D().getNext());
            current.getEdgeNode()->setHandle(current.getAdjacent3D());
            deleteNode(current.getAdjacent2D().getFaceNode());
            
            DecrementCoord(current.getAdjacent2D().getNext().getEdgeNode(),1);
            DecrementCoord(current.getAdjacent2D().getNext().getEdgeNode(),1);
            
            deleteNode(current.getAdjacent2D().getNext().getEdgeNode());
            current = current.getNext();
        }
        deleteNode(adj2.getFaceNode());
        adj2.setFaceNode(adj1.getFaceNode());
        
        setAdjacent(adj1,adj2);
        
        DecrementCoord(origin.getVertexNode(),0);
  		DecrementCoord(origin.getVertexNode(),0);
  		
        deleteNode(origin.getVertexNode());
        deleteSimplex(origin.getAdjacent3D().getSimplex());
        deleteSimplex(origin.getSimplex());
        
        if( indualtree ) { setInDualSpanningTree(adj1); }

        // return a handle incident to the remaining triangle
        
        
        DecrementCoord(adj1.getEdgeNode(),1);
        DecrementCoord(adj1.getEdgeNode(),1);
        DecrementCoord(adj1.getNext().getEdgeNode(),1);
        DecrementCoord(adj1.getNext().getEdgeNode(),1);
        DecrementCoord(adj1.getPrevious().getEdgeNode(),1);
        DecrementCoord(adj1.getPrevious().getEdgeNode(),1);
        
		DecrementCoord(adj1.getVertexNode(),0);
        DecrementCoord(adj1.getVertexNode(),0);
        DecrementCoord(adj1.getNext().getVertexNode(),0);
        DecrementCoord(adj1.getNext().getVertexNode(),0);
        DecrementCoord(adj1.getPrevious().getVertexNode(),0);
        DecrementCoord(adj1.getPrevious().getVertexNode(),0);
        
        
        
        return adj1;
    } // END unExpandTriangle
    

Handle Triangulation::diagonalSubdivide(Handle interface) {
        // Assume the triangulation has been checked for the correct configuration
        
        //if( interface.getAdjacent3D().getSimplex() == interface.getSimplex() )
        //    return std::pair<bool,Handle>(false,interface);
        
        //assert( checkNodes() );
        
        IncrementCoord(interface.getAdjacent2D().getPrevious().getEdgeNode(),1);
        IncrementCoord(interface.getNext().getAdjacent2D().getPrevious().getEdgeNode(),1);
        IncrementCoord(interface.getPrevious().getAdjacent2D().getPrevious().getEdgeNode(),1);
        IncrementCoord(interface.getAdjacent3D().getAdjacent2D().getPrevious().getEdgeNode(),1);
        IncrementCoord(interface.getAdjacent3D().getNext().getAdjacent2D().getPrevious().getEdgeNode(),1);
        IncrementCoord(interface.getAdjacent3D().getPrevious().getAdjacent2D().getPrevious().getEdgeNode(),1);

        DecrementCoord(interface.getEdgeNode(),1);
        DecrementCoord(interface.getNext().getEdgeNode(),1);
        DecrementCoord(interface.getPrevious().getEdgeNode(),1);
        
        
        
        
        Handle rootA = interface;
        Handle rootB = interface.getAdjacent3D().getNext();
        std::vector<TreeNode *> vert{
            rootA.getAdjacent2D().getPrevious().getVertexNode(),
            rootB.getAdjacent2D().getPrevious().getVertexNode(),
            rootB.getVertexNode(),
            rootB.getNext().getVertexNode(),
            rootB.getPrevious().getVertexNode()
        };
        std::vector<TreeNode *> edges{
            rootA.getAdjacent2D().getNext().getEdgeNode(),
            rootA.getAdjacent2D().getPrevious().getEdgeNode(),
            rootA.getNext().getAdjacent2D().getPrevious().getEdgeNode(),
            rootB.getAdjacent2D().getNext().getEdgeNode(),
            rootB.getNext().getAdjacent2D().getPrevious().getEdgeNode(),
            rootB.getAdjacent2D().getPrevious().getEdgeNode(),
            rootA.getNext().getEdgeNode(),
            rootA.getEdgeNode(),
            rootA.getPrevious().getEdgeNode() 
        };
        std::vector<TreeNode *> faces{ 
            interface.getAdjacent2D().getFaceNode(),
            interface.getNext().getAdjacent2D().getFaceNode(),
            interface.getPrevious().getAdjacent2D().getFaceNode(),
            interface.getPrevious().getAdjacent3D().getAdjacent2D().getFaceNode(),
            interface.getNext().getAdjacent3D().getAdjacent2D().getFaceNode(),
            interface.getAdjacent3D().getAdjacent2D().getFaceNode(),
            interface.getFaceNode(),
            newNode(2),
            newNode(2)
        };

        std::vector<bool> in;
        
        for(auto t : edges) in.push_back(isInSpanningTree(t->getHandle()));
    
        // use the four faces of a temporary dummy simplex to hold the adjacency
        // information of the boundary triangles whose adjacency we will change
        TreeNode dummyNode;
        Simplex dummyOutside(&dummyNode);
        dummyOutside.setId(-1);
        std::vector<Handle> nbr{
            rootA.getNext().getAdjacent2D(),
            rootA.getPrevious().getAdjacent2D(),
            rootB.getNext().getAdjacent2D(),
            rootB.getPrevious().getAdjacent2D()};
            
        /*if( !allowSelfAdjacency_ ) {
            if( sameFace(rootA.getAdjacent2D().getAdjacent3D(), nbr[3]) )
                return std::pair<bool,Handle>(false,interface);
            if( sameFace(nbr[0].getAdjacent3D(), nbr[2]) )
                return std::pair<bool,Handle>(false,interface);
            if( sameFace(nbr[1].getAdjacent3D(), rootB.getAdjacent2D()) )
                return std::pair<bool,Handle>(false,interface);
        }*/    
        

        
        for(int i=0;i<4;++i) setAdjacent(dummyOutside.getEdge(i,0),nbr[i].getAdjacent3D());
        
        Handle rootC = newSimplex()->getEdge(0,0);

        // set the three internal adjacencies
        setAdjacent(rootC,rootA.getNext().getAdjacent2D().getPrevious());
        setAdjacent(rootA.getPrevious().getAdjacent2D(),rootB.getAdjacent2D().getNext().getAdjacent2D());
        setAdjacent(rootC.getAdjacent2D(),rootB.getNext().getAdjacent2D().getPrevious());
        
        std::vector<Handle> newnbr{
            rootC.getAdjacent2D().getPrevious().getAdjacent2D().getNext(), rootB,
            rootC.getPrevious().getAdjacent2D().getNext(), rootA
        };
            
        for(int i=0;i<4;++i) setAdjacent(dummyOutside.getEdge(i,0).getAdjacent3D(),newnbr[i]);
        
        // update vertex nodes
        rootA.getPrevious().setVertexNode(vert[1]);
        rootB.getPrevious().setVertexNode(vert[0]);
        rootC.setVertexNode(vert[1]);
        rootC.getPrevious().setVertexNode(vert[4]);
        rootC.getNext().setVertexNode(vert[0]);
        rootC.getAdjacent2D().getPrevious().setVertexNode(vert[3]);
        //vert[4]->setHandle(rootA.getNext());
        //vert[3]->setHandle(rootB.getNext());
        
        // update edge nodes
        setEdgeNode(rootC,newNode(1));
        rootB.getPrevious().setEdgeNode(edges[0]);
        rootB.getNext().setEdgeNode(edges[2]);
        rootA.getPrevious().setEdgeNode(edges[3]);
        rootA.getNext().setEdgeNode(edges[4]);
        rootC.getPrevious().setEdgeNode(edges[4]);
        rootC.getNext().setEdgeNode(edges[1]);
        rootC.getPrevious().getAdjacent2D().getNext().setEdgeNode(edges[6]);
        rootC.getAdjacent2D().getPrevious().setEdgeNode(edges[2]);
        rootC.getAdjacent2D().getNext().setEdgeNode(edges[5]);
        edges[7]->setHandle(rootA);
        edges[8]->setHandle(rootB);
        
        // update face nodes
        rootA.setFaceNode(faces[5]);
        rootA.getPrevious().getAdjacent2D().setFaceNode(faces[6]);
        rootA.getNext().getAdjacent2D().setFaceNode(faces[8]);
        rootB.setFaceNode(faces[2]);
        rootB.getNext().getAdjacent2D().setFaceNode(faces[7]);
        rootB.getPrevious().getAdjacent2D().setFaceNode(faces[6]);
        rootC.setFaceNode(faces[8]);
        rootC.getAdjacent2D().setFaceNode(faces[7]);
        rootC.getPrevious().getAdjacent2D().setFaceNode(faces[4]);
        rootC.getNext().getAdjacent2D().setFaceNode(faces[1]);
        

        /*for(int i=0;i<9;++i) {
            std::cout << "e" << i << " = " << edges[i]->getId() << ", ";
        }
        std::cout << "id = " << rootA.getSimplex()->getId() << " " << rootB.getSimplex()->getId() << " " <<rootC.getSimplex()->getId() << "\n";
        assert( checkNodes() );
        */

        rootB.getPrevious().setInSpanningTree(in[0]);
        rootB.getNext().setInSpanningTree(in[2]);
        rootB.getNext().getAdjacent2D().getPrevious().setInSpanningTree(false);
        rootA.getPrevious().setInSpanningTree(in[3]);
        rootA.getNext().setInSpanningTree(in[4]);
        rootA.getNext().getAdjacent2D().getPrevious().setInSpanningTree(false);
        rootC.getPrevious().setInSpanningTree(in[4]);
        rootC.getNext().setInSpanningTree(in[1]);
        rootC.getPrevious().getAdjacent2D().getNext().setInSpanningTree(in[6]);
        rootC.getAdjacent2D().getPrevious().setInSpanningTree(in[2]);
        rootC.getAdjacent2D().getNext().setInSpanningTree(in[5]);
        rootC.setInSpanningTree(false);

//        interface 1 2 3 was connected to 2, now to 1
//		  edge ones were connected to 1 now to 2
//		  new link coord num is 3
		
		IncrementCoord(rootC.getVertexNode(),0);
		IncrementCoord(rootC.getVertexNode(),0);
		
		IncrementCoord(rootC.getAdjacent3D().getVertexNode(),0);
		IncrementCoord(rootC.getAdjacent3D().getVertexNode(),0);
		
		IncrementCoord(rootC.getEdgeNode(),1);
		IncrementCoord(rootC.getEdgeNode(),1);
		IncrementCoord(rootC.getEdgeNode(),1);

        return rootC;
    } // END diagonalSubdivide
        
Handle Triangulation::unDiagonalSubdivide(Handle diagonal) {
        // assume triangulation has correct configuration
        // the following handles are exactly (to be) those of diagonalSubdivide

		DecrementCoord(diagonal.getEdgeNode(), 1);
		DecrementCoord(diagonal.getEdgeNode(), 1);
		DecrementCoord(diagonal.getEdgeNode(), 1);
		
		DecrementCoord(diagonal.getVertexNode(), 0);
		DecrementCoord(diagonal.getVertexNode(), 0);
		
		DecrementCoord(diagonal.getAdjacent3D().getVertexNode(), 0);
		DecrementCoord(diagonal.getAdjacent3D().getVertexNode(), 0);
		
        
		Handle rootC = diagonal;
        Handle rootA = rootC.getPrevious().getAdjacent3D().getAdjacent2D().getPrevious();
        Handle rootB = rootC.getAdjacent2D().getAdjacent3D().getNext().getAdjacent2D().getPrevious();
        std::vector<TreeNode *> vert{
            rootA.getAdjacent2D().getPrevious().getVertexNode(),
            rootB.getAdjacent2D().getPrevious().getVertexNode(),
            rootB.getVertexNode(),
            rootB.getNext().getVertexNode(),
            rootA.getNext().getVertexNode()
        };
        std::vector<TreeNode *> edges{
            rootB.getPrevious().getEdgeNode(),
            rootC.getNext().getEdgeNode(),
            rootB.getNext().getEdgeNode(),
            rootA.getPrevious().getEdgeNode(),
            rootA.getNext().getEdgeNode(),
            rootC.getAdjacent2D().getNext().getEdgeNode(),
            rootC.getPrevious().getAdjacent2D().getNext().getEdgeNode(),
            rootA.getEdgeNode(),
            rootB.getEdgeNode()
        };
        std::vector<TreeNode *> faces{ 
            diagonal.getAdjacent3D().getPrevious().getAdjacent2D().getFaceNode(),
            diagonal.getNext().getAdjacent2D().getFaceNode(),
            diagonal.getAdjacent2D().getAdjacent3D().getNext().getAdjacent2D().getFaceNode(),
            diagonal.getAdjacent2D().getAdjacent3D().getPrevious().getAdjacent2D().getFaceNode(),
            diagonal.getPrevious().getAdjacent2D().getFaceNode(),
            diagonal.getAdjacent3D().getNext().getAdjacent2D().getFaceNode(),
            diagonal.getAdjacent3D().getAdjacent2D().getFaceNode(),
            diagonal.getAdjacent2D().getFaceNode(),
            diagonal.getFaceNode()
        };
        
        std::vector<bool> in;
        
        for(auto t : edges) in.push_back(isInSpanningTree(t->getHandle())); 
        
        TreeNode dummyNode;
        Simplex dummyOutside(&dummyNode);
        dummyOutside.setId(-1);
        std::vector<Handle> nbr{
            rootC.getAdjacent2D().getPrevious().getAdjacent2D().getNext(),rootB,
            rootC.getPrevious().getAdjacent2D().getNext(),rootA
        };
            
        for(int i=0;i<4;++i) setAdjacent(dummyOutside.getEdge(i,0),nbr[i].getAdjacent3D());
         
        deleteSimplex(rootC.getSimplex());
        deleteNode(diagonal.getEdgeNode());
        deleteNode(faces[7]);
        deleteNode(faces[8]);
        
        setAdjacent(rootA,rootB.getPrevious());

        std::vector<Handle> newnbr{
            rootA.getNext().getAdjacent2D(),
            rootA.getPrevious().getAdjacent2D(),
            rootB.getNext().getAdjacent2D(),
            rootB.getPrevious().getAdjacent2D()};
            
        for(int i=0;i<4;++i) setAdjacent(dummyOutside.getEdge(i,0).getAdjacent3D(),newnbr[i]);
        
        // update vertex nodes
        rootA.getPrevious().setVertexNode(vert[3]);
        rootB.getPrevious().setVertexNode(vert[4]);
        vert[0]->setHandle(rootA.getAdjacent2D().getPrevious());
        vert[1]->setHandle(rootB.getAdjacent2D().getPrevious());
        vert[2]->setHandle(rootB);
        //vert[3]->setHandle(rootA.getPrevious());
        //vert[4]->setHandle(rootB.getPrevious());

        // update edge nodes
        rootA.getPrevious().setEdgeNode(edges[8]);
        rootA.getNext().setEdgeNode(edges[6]);
        rootA.getNext().getAdjacent2D().getPrevious().setEdgeNode(edges[2]);
        rootB.getNext().setEdgeNode(edges[6]);
        rootB.getPrevious().setEdgeNode(edges[7]);
        rootB.getNext().getAdjacent2D().getPrevious().setEdgeNode(edges[4]);
        edges[0]->setHandle(rootA.getAdjacent2D().getNext());
        edges[1]->setHandle(rootA.getAdjacent2D().getPrevious());
        edges[3]->setHandle(rootB.getAdjacent2D().getNext());
        edges[5]->setHandle(rootB.getAdjacent2D().getPrevious());

        // update face nodes
        rootA.setFaceNode(faces[6]);
        rootA.getPrevious().getAdjacent2D().setFaceNode(faces[2]);
        rootA.getNext().getAdjacent2D().setFaceNode(faces[1]);
        rootB.setFaceNode(faces[6]);
        rootB.getPrevious().getAdjacent2D().setFaceNode(faces[5]);
        rootB.getNext().getAdjacent2D().setFaceNode(faces[4]);

        rootA.getPrevious().setInSpanningTree(in[8]);
        rootA.getNext().setInSpanningTree(in[6]);
        rootA.getNext().getAdjacent2D().getPrevious().setInSpanningTree(in[2]);
        rootB.getNext().setInSpanningTree(in[6]);
        rootB.getPrevious().setInSpanningTree(in[7]);
        rootB.getNext().getAdjacent2D().getPrevious().setInSpanningTree(in[4]);

        DecrementCoord(rootA.getAdjacent2D().getPrevious().getEdgeNode(),1);
        DecrementCoord(rootA.getNext().getAdjacent2D().getPrevious().getEdgeNode(),1);
        DecrementCoord(rootA.getPrevious().getAdjacent2D().getPrevious().getEdgeNode(),1);
        DecrementCoord(rootA.getAdjacent3D().getAdjacent2D().getPrevious().getEdgeNode(),1);
        DecrementCoord(rootA.getAdjacent3D().getNext().getAdjacent2D().getPrevious().getEdgeNode(),1);
        DecrementCoord(rootA.getAdjacent3D().getPrevious().getAdjacent2D().getPrevious().getEdgeNode(),1);

        IncrementCoord(rootA.getEdgeNode(), 1);
        IncrementCoord(rootA.getNext().getEdgeNode(), 1);
        IncrementCoord(rootA.getPrevious().getEdgeNode(), 1);

        return rootA;
    } // END unDiagonalSubdivide

#endif
