% $Id: GetEdgeByGraphSearch.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	GetEdgeByGraphSearch() 
% // file:		EdgeExtraction.cpp
% // author:		siddharth
% // date:		
% // task:		calls the the A-algorithm graph search algorithm to build an edge 
% //				taking any one point (seed point) on the edge as input & the upper & lower bounding points
% //				the task is thus split into constructing two edges 
% //				one from the point to upper bound traversing from bottom to up
% //				the second from the seed point to lower bound by traversing up to bottom
% //				additional heuristic knowledge could be passed: reference border from which a specific
% //				distance has to be maintained
% // parameter:	a seed point on the edge, two bounding points, reference border heuristic information
% // return value: 
% // NOTE: This is the key function available as a public function of this class which does the major functionality of building the edges		
% //-----------------------------------------------------------------------------
% NODE= [POINT_X POINT_Y ConnectedNodeIndex dAssociatedCummulativeCost]
% NodesList = [ Node1;
%               Node2;
%               Node3......]

function [lEdge] = GetEdgeByGraphSearch (IedgeImage, iEdgeThreshold, pSeed, pStart, pEnd, bUseGlobalXInfo, bUseDirectionEstimate, InitialDirectionEstimate)

	
% 	//Note: //pStart is a lower point in image but has greater value of y
 	if(		(pSeed(1,1) ~= pStart(1,1)) 		&&	(pSeed(1,1) ~= pEnd(1,1)) )
	
% 		//divide into two parts
%  		//1) middle point to lower part of image using UpBottom
% 		//2) middle point to upper part of image using bottom up
		

% 		//1) middle point to lower part of image using UpBottom
		lEdge1 = GetEdgeByGraphSearchTraverseUpBottom(IedgeImage,iEdgeThreshold, pSeed,pStart, bUseGlobalXInfo, bUseDirectionEstimate, InitialDirectionEstimate);

		
% 		//2) middle point to upper part of image using bottom up
		lEdge2 = GetEdgeByGraphSearchTraverseBottomUp(IedgeImage,iEdgeThreshold, pSeed, pEnd, bUseGlobalXInfo, bUseDirectionEstimate, InitialDirectionEstimate);
        
        lEdge = [lEdge1; lEdge2];
	else
	
		if(pSeed (1,1)== pStart (1,1))
					
% 			//traverse bottom up
			lEdge = GetEdgeByGraphSearchTraverseBottomUp(IedgeImage,iEdgeThreshold, pStart, pEnd, bUseGlobalXInfo, bUseDirectionEstimate, InitialDirectionEstimate);
                                                    
		else
	
% 			//traverse up towards bottom
			lEdge = GetEdgeByGraphSearchTraverseUpBottom(IedgeImage,iEdgeThreshold,	pEnd,pStart, bUseGlobalXInfo, bUseDirectionEstimate, InitialDirectionEstimate);
            
        end
        
    end
    
    
    
    
 % $Log: GetEdgeByGraphSearch.m,v $
 % Revision 1.1  2005/10/27 19:10:47  svikal
 % Added CVS tags
 %