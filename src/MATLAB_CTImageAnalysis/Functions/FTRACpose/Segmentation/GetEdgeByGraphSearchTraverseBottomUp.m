% $Id: GetEdgeByGraphSearchTraverseBottomUp.m,v 1.2 2005/11/28 19:45:14 anton Exp $

% //--------------------------------------------------------------------------------------------------------------------------
% // function:	GetEdgeByGraphSearchTraverseBottomUp () 
% // file:		EdgeExtraction.cpp
% // author:	siddharth
% // date:		
% // task:		implements A-algorithm graph search of border detection as graph searching
% //				LOGIC: A graph is general structure consisting of a set of nodes and arcs between
% //				the nodes. We consider oriented & numerically weighted arcs, these weights being called
% //				the costs.
% //				The border detection process is transformed into a search of optimal path in the weighted graph, 
% //				the aim of which is to find the best path that connects the two specified nodes, starting & ending nodes
% //				Assume that both edge magnitude s(x) and edge direction ö(x) is available in the edge image
% //				Edge image pixel corresponds to a graph node by a value s(x). Two nodes Ni & Nj corresponding to two
% //				8-connected adjacent pixels Xi & Xj are connected by an arc if the edge directions  ö(Xi) and ö(xj)
% //				match the local border direction. We can apply the following rules to construct the graph. To connect the
% //				node Ni representing the pixel Xi with a node Nj representing a pixel Xj, pixel Xj must be one of the three
% //				existing neighbours of Xi in the direction [ö(Xi)-pi/4, ö(Xi)+ pi/4]. Further, s(Xi) and s(Xj) must be
% //				greater than T where T is some preset threshold of edge significance. 
% //				Let Xa be the starting border element, and Xb be the end border element. To use graph search, a method of 
% //				oriented weighted-graph expansion must first be defined. A cost function f(Xi) must also be defined that is the
% //				cost estimate of the path between the nodes Na & Nb which goes through an intermediate node Ni. the cost function
% //				f(Xi) in our case consists of following costs
% //						1) Threshold Associated Cost---measure of strength of the edge, stronger the edge smaller the cost
% //						2) Starting Point Cost---distance from the starting point, smaller the distance, smaller is the cost
% //						3) TargetPointCost---Used if the final destination location exactly known, both in x & y, 
% //											 distance of the point to the targer point, smaller the cost
% //                        4) Direction associated cost: given the initial
%                               estimate of the direction, determine, if a new point added is going to
%                               cause the overall line direction to be within a certain range or not; the
%                               closer to desired direction, less the cost, more otherwise
% //				The cost of each successive node is then the commulative cost of the node & the node from where it was expanded
% //				PSUEDO CODE: A-algorithm graph search
% //					1. Expand the starting node Na and put all its viable successors into an OPEN list with pointers back at 
% //					   the starting node Na. Evaluate the cost function 'f' for each expanded node
% //					2. If the OPEN list is empty then fail. Determine the node Ni from the OPEN list with the lowest associated
% //					   cost f(Ni) and remove it. If Ni = Nb (end node), then trace back through the pointers to find the optimum
% //					   path and stop.
% //					3. If the option to stop was not taken in step 2, expand the specified node Ni and put its successors on the
% //					   OPEN list with pointers back to Ni. Compute their costs f. Go to step 2
% //
% //				Additional constraints:
% //				This process can easily result in an infinite loop. To prevent this behaviour, no node expansion is allowed
% //				that puts a node on the OPEN list if this node has already been visited and put on the OPEN list in the past.
% //
% //				Additional heuristics used:
% //				In this particular problem of bone cement edge extraction, the existence of a path between a starting pixel
% //				Xa and an ending pixel Xb is not guaranteed because of the possible discontinuities in the edge image. 
% //				Therefore, if there is no node in the OPEN list which can be expanded, it may be possible to expand the nodes
% //				with non-significant edge valued successors---this can build a bridge to pass these discontinuities in border representations
% // input parameter:	edge threshold, starting point, ending point, 
% //				a flag indicating whether the exact end point information is known & hence needs to be used or that just Y-info about the
% //				end point is known, hence x-info is not to be used,
% //                flag indicating whether to use initial direction estimate or not, initial
% //                direction estimate (used only when the corresponding flag is true)
% // return value: connected list of points
% // NOTE: This function implements algorithm A-graph search (pseudo code given in the book 
% //		Image processing, analysis & understanding---Milan Sonka, under chapter Edge-based segmentation),
% //		with edge orientation, looking/traversing from bottom to up.
% //        Algorithm is based on the assumption that at least one point on
% the line is known and initial direction is also known (which is nearly
% vertical in our case +45 to -45 with the vertical)
% //----------------------------------------------------------------------------------------------------------------------------------------------------
% NODE= [POINT_X POINT_Y ConnectedNodeIndex dAssociatedCummulativeCost]
% NodesList = [ Node1;
%               Node2;
%               Node3......]

function [lEdge] = GetEdgeByGraphSearchTraverseBottomUp (IedgeImage, iEdgeThreshold, pStart, pEnd, bUseGlobalXInfo, bUseDirectionEstimate, InitialDirectionEstimate)

	lEdge = [];
    
    lOpenPointsList = [];
    
    lExpandedNodesList = [];
	
	iX_Index = 1; iY_Index = 1;
    
    iX_Dimension = length(IedgeImage(:,1)) - 1;
    
    iY_Dimension = length(IedgeImage(1,:)) - 1;
   
	iLookAheadLevel = 1;
	
	bLookAhead = false;

	bUseLeftReference = false; bUseRightReference = false;

	iPointIntensity = 0; iAcceptableThresholdRadius = iEdgeThreshold/2;

	dThresholdAssociatedCost = 0; dStartingPointCost = 0; dTargetPointCost = 0; dLeftBorderAssociatedCost = 0; dRightBorderAssociatedCost = 0; dTotalCost = 0; dDirectionAssociatedCost = 0;
	
    
% 	//step 1

% 	//expand node, find its successors
% 	//evaluate the cost function

	nodeToAdd = [];
	
	nodeToExpand = [];	
	
	nodeToExpand(1,1) = pStart (1,1); nodeToExpand(1,2) = pStart (1,2);

%   associated cost
	nodeToExpand(1,4) = 0;
%   connected node
	nodeToExpand(1,3) = 0;

    lExpandedNodesList = [lExpandedNodesList; nodeToExpand];
    
	iIterationCount = 0;

	while(~	( ((bUseGlobalXInfo) && (nodeToExpand(1,2) == pEnd(1,2)) && (nodeToExpand(1,1) == pEnd(1,1)))  || ((~bUseGlobalXInfo) && (nodeToExpand(1,1) == pEnd(1,1)))	) )
	
		iIterationCount = iIterationCount + 1;

		if(nodeToExpand(1,1) < pEnd(1,1))
			break;
        end
	
		iX_Index = nodeToExpand(1,1);
		iY_Index = nodeToExpand(1,2);

		dThresholdAssociatedCost = 0;

		dStartingPointCost = 0;

		dTargetPointCost = 0;

		dLeftBorderAssociatedCost = 0;

		dRightBorderAssociatedCost = 0;
        
        dDirectionAssociatedCost = 0;

		dTotalCost = 0;
        
			
% 		//check the neighbours
% 
% 		//x-1,y-1
		if (iX_Index > 1 && iY_Index > 1)

            iPointIntensity = IedgeImage (iX_Index-1, iY_Index-1);
			

% 			//check the intensity range or edge strength
			if( IsWithinIntensityRange(iPointIntensity, iEdgeThreshold, iAcceptableThresholdRadius))

% 				//check if available the approx. distance from the ref.
				pToLookFor = [];
				pToLookFor = [(iX_Index -1) (iY_Index -1)];

                nodeToAdd(1,1) = iX_Index-1;
                nodeToAdd(1,2) = iY_Index-1;

                
%                   store the index of the connected node  
                    iFoundNodeIndex = 0;
                    
                    if (length(lExpandedNodesList) > 0)
                    
                        iFoundNodeIndex = find((lExpandedNodesList(:,1) + 1000*lExpandedNodesList(:,2)) == (nodeToExpand(1,1) + 1000*nodeToExpand(1,2)));
                        
                        if(length(iFoundNodeIndex) > 1)
                            hello = 0;
                            iFoundNodeIndex = iFoundNodeIndex(1,1);
                        end

                        if (iFoundNodeIndex)
                            nodeToAdd(1,3) = iFoundNodeIndex;
                        else
                            nodeToAdd(1,3) = 0;
                        end
                    end
                    
                
%               find direction associated cost
                dDirectionAssociatedCost = FindDirectionAssociatedCost (nodeToAdd, lExpandedNodesList, bUseDirectionEstimate, InitialDirectionEstimate);
                
                Cond3 = dDirectionAssociatedCost <= 10 ;
                
				if ( Cond3)

					if (iPointIntensity >= iEdgeThreshold)

                        dThresholdAssociatedCost = 2;
                                                                    
					else 
             					
				    		dThresholdAssociatedCost = 5;
                           
                        
                    end
					
                	dStartingPointCost = FindDistance (nodeToAdd(1:2), pStart);

%                     //if the precise end point x-info. is available then only use it
					if (bUseGlobalXInfo) 
						dTargetPointCost = FindDistance (nodeToAdd(1:2), pEnd);
                    end
                    
					dTotalCost = dThresholdAssociatedCost + dStartingPointCost + dTargetPointCost + dLeftBorderAssociatedCost + dRightBorderAssociatedCost + dDirectionAssociatedCost;

%                   cummulative cost  
					nodeToAdd(1,4) = dTotalCost + nodeToExpand(1,4);			
					
					[bNodeAlready lOpenPointsList] = (IsNodeInTheList(nodeToAdd, lOpenPointsList));

					if ( ~bNodeAlready )
    % 					//add in the open list
                        lOpenPointsList = [lOpenPointsList; nodeToAdd];

                        if(bLookAhead)
						
							bLookAhead = false;
							iLookAheadLevel = 1;
                            
                        end
                        
                    end
                    
                end
                                
            end
                    
        end

            


% 		//x-1,y
		if (iX_Index > 1)
		
			iPointIntensity = IedgeImage(iX_Index-1, iY_Index);
			
			if( IsWithinIntensityRange(iPointIntensity, iEdgeThreshold, iAcceptableThresholdRadius))
			
% 				//check if available the approx. distance from the ref.
				pToLookFor = [];
                
				pToLookFor = [(iX_Index -1) iY_Index];
                
					nodeToAdd(1,1) = iX_Index-1;
                    nodeToAdd(1,2) = iY_Index;
                                    
                    iFoundNodeIndex = 0;
                    
%                   store the index of the connected node  
                    if (length(lExpandedNodesList) > 0)
                        
                        iFoundNodeIndex = find((lExpandedNodesList(:,1) + 1000*lExpandedNodesList(:,2)) == (nodeToExpand(1,1) + 1000*nodeToExpand(1,2)));

                        if(length(iFoundNodeIndex) > 1)
                            hello = 0;
                            iFoundNodeIndex = iFoundNodeIndex(1,1);
                        end

                        if (iFoundNodeIndex)
                            nodeToAdd(1,3) = iFoundNodeIndex;
                        else
                            nodeToAdd(1,3) = 0;
                        end
                    end
                

%               find direction associated cost                
                dDirectionAssociatedCost = FindDirectionAssociatedCost (nodeToAdd, lExpandedNodesList, bUseDirectionEstimate, InitialDirectionEstimate);
                
                Cond3 = dDirectionAssociatedCost <= 10 ;
                
				if ( Cond3)

					if (iPointIntensity >= iEdgeThreshold)

                        dThresholdAssociatedCost = 2;
                                                                    
					else 
    					
				    		dThresholdAssociatedCost = 5;
                        
                    end
					

                	dStartingPointCost = FindDistance (nodeToAdd(1:2), pStart);

%                     //if the precise end point x-info. is available then only use it
					if (bUseGlobalXInfo) 
						dTargetPointCost = FindDistance (nodeToAdd(1:2), pEnd);
                    end
                    


					dTotalCost = dThresholdAssociatedCost + dStartingPointCost + dTargetPointCost + dLeftBorderAssociatedCost + dRightBorderAssociatedCost + dDirectionAssociatedCost;

%                   cummulative cost  
					nodeToAdd(1,4) = dTotalCost + nodeToExpand(1,4);			
					
					[bNodeAlready lOpenPointsList] = (IsNodeInTheList(nodeToAdd, lOpenPointsList));

					if ( ~bNodeAlready )
    % 					//add in the open list		
                        lOpenPointsList = [lOpenPointsList; nodeToAdd];

                        if(bLookAhead)
						
							bLookAhead = false;
							iLookAheadLevel = 1;
                            
                        end
                        
                    end
                    
                end
                                
            end
                    
        end




% 		//x-1,y+1
		if( (iY_Index +1) < iY_Dimension && iX_Index > 1)
		
			iPointIntensity = IedgeImage(iX_Index-1, iY_Index+1);
			
			if( IsWithinIntensityRange(iPointIntensity, iEdgeThreshold, iAcceptableThresholdRadius))
			
% 				//check if available the approx. distance from the ref.
				pToLookFor = [];
				pToLookFor = [(iX_Index-1) (iY_Index + 1)];
                
					nodeToAdd(1,1) = iX_Index-1;
                    nodeToAdd(1,2) = iY_Index + 1;
                

         
                iFoundNodeIndex = 0;
                    
%                   store the index of the connected node  
                    if (length(lExpandedNodesList) > 0)
                        
                        iFoundNodeIndex = find((lExpandedNodesList(:,1) + 1000*lExpandedNodesList(:,2)) == (nodeToExpand(1,1) + 1000*nodeToExpand(1,2)));

                        if(length(iFoundNodeIndex) > 1)
                            hello = 0;
                            iFoundNodeIndex = iFoundNodeIndex(1,1);
                        end

                        if (iFoundNodeIndex)
                            nodeToAdd(1,3) = iFoundNodeIndex;
                        else
                            nodeToAdd(1,3) = 0;
                        end
                    end
                

%                   find direction associated cost
                
                dDirectionAssociatedCost = FindDirectionAssociatedCost (nodeToAdd, lExpandedNodesList, bUseDirectionEstimate, InitialDirectionEstimate);
                
                Cond3 = dDirectionAssociatedCost <= 10 ;
                
				if ( Cond3)

					if (iPointIntensity >= iEdgeThreshold)

                        dThresholdAssociatedCost = 2;
                                                                    
					else 
    					
				    		dThresholdAssociatedCost = 5;
                           
                    end
					

                	dStartingPointCost = FindDistance (nodeToAdd(1:2), pStart);

%                     //if the precise end point x-info. is available then only use it
					if (bUseGlobalXInfo) 
						dTargetPointCost = FindDistance (nodeToAdd(1:2), pEnd);
                    end
                    
                    
					dTotalCost = dThresholdAssociatedCost + dStartingPointCost + dTargetPointCost + dLeftBorderAssociatedCost + dRightBorderAssociatedCost + dDirectionAssociatedCost;

%                   cummulative cost  
					nodeToAdd(1,4) = dTotalCost + nodeToExpand(1,4);			
					
					[bNodeAlready lOpenPointsList] = (IsNodeInTheList(nodeToAdd, lOpenPointsList));

					if ( ~bNodeAlready )
% 					//add in the open list		
                        lOpenPointsList = [lOpenPointsList; nodeToAdd];

                        if(bLookAhead)
						
							bLookAhead = false;
							iLookAheadLevel = 1;
                            
                        end
                        
                    end
                    
                end
                                
            end
                    
        end


% 		//Step 2
% 		//if the open list if empty, fail
% 		//else find the lost cost node N from the open list & remove it
% 		//if N = end point, then trace back the path & stop
% 		//else go to step 3

		if ( length(lOpenPointsList) == 0)
		
			bLookAhead = true;
            break;

			if (iLookAheadLevel > 5)
				break;
            end
			
% 			//first look back at last 10 points to get an idea

			nodeToLookBack = nodeToExpand;

			i = 0, iStep = 0;

			szY_Dev = zeros(11,1);

			HighYCount = 0; LowYCount = 0;

			szY_Dev (1) = nodeToLookBack(1,2);

			while( (nodeToLookBack(1,3) ~= 0) && i < 10)
			
				i = i + 1;

				nodeToLookBack = lExpandedNodesList (nodeToLookBack(1,3),:);

				szY_Dev(i) = nodeToLookBack(1,2);

				if (szY_Dev(i) >= szY_Dev(1))
					++HighYCount;
				else
					++LowYCount;
                end
                

            end

%             //if the precise end point x-info. is available then only use it
			if (bUseGlobalXInfo) 
			
				if( (HighYCount > LowYCount) && (nodeToExpand(1,2) < pEnd(1,2)))
					iStep = 1;
				else 
                    if ( (nodeToExpand(1,2) > pEnd(1,2)))
    					iStep = -1;
				    else
    					iStep = 0;
                        
                    end
                    
                end

                
			else
			
				if ( HighYCount < LowYCount)
					iStep = -1;
				else
					iStep = 0;
                end
                
            end


			iLookAheadLevel = iLookAheadLevel + 1;

% 			//x-1,y + iStep
			if(iX_Index -1 > pEnd(1,1))
			
				nodeToAdd(1,2) = iY_Index + iStep;
				
				nodeToAdd(1,1) = iX_Index-1;

				dStartingPointCost = FindDistance (nodeToAdd(1:2), pStart);

%               //if the precise end point x-info. is available then only use it
				if (bUseGlobalXInfo) 
					dTargetPointCost = FindDistance (nodeToAdd(1:2), pEnd);
                end
                

				dThresholdAssociatedCost = 12;
                
                
                iFoundNodeIndex = 0;
                
                % store the index of the connected node  
                    if (length(lExpandedNodesList) > 0)
                        
                        iFoundNodeIndex = find((lExpandedNodesList(:,1) + 1000*lExpandedNodesList(:,2)) == (nodeToExpand(1,1) + 1000*nodeToExpand(1,2)));

                        if(length(iFoundNodeIndex) > 1)
                            hello = 0;
                            iFoundNodeIndex = iFoundNodeIndex(1,1);                            
                        end

                        if (iFoundNodeIndex)
                            nodeToAdd(1,3) = iFoundNodeIndex;
                        else
                            nodeToAdd(1,3) = 0;
                        end
                    end
                
                
%               to do:
%               find direction associated cost
                dDirectionAssociatedCost = FindDirectionAssociatedCost (nodeToAdd, lExpandedNodesList, bUseGlobalXInfo, InitialEstimatedLineDirection);

				dTotalCost = dThresholdAssociatedCost + dStartingPointCost + dTargetPointCost;

				nodeToAdd(1,4) = dTotalCost + nodeToExpand(1,4);							

				[bNodeAlready lOpenPointsList] = (IsNodeInTheList(nodeToAdd, lOpenPointsList));

				if ( ~bNodeAlready )

					lOpenPointsList = [lOpenPointsList; nodeToAdd];
                end
                
            end
    
        end


% 		//get the minimum cost node
		if ( length(lOpenPointsList)> 0 )
			[lExpandedNodesList lOpenPointsList nodeToExpand] = FindLowestCostNode (lOpenPointsList, lExpandedNodesList);
		else
			break;
        end
 
        
    end
		
    
% 		//termination point of the function
% 		//trace back the path
        nodeNextOnEdge = nodeToExpand;

        lEdge = [];

        while (nodeNextOnEdge (1,3) ~= 0)
		
			nodeToAdd = nodeNextOnEdge;

			nodeToAdd(1,3) = 0;

			lEdge = [lEdge; nodeToAdd(1:2)];

			nodeNextOnEdge = lExpandedNodesList (nodeNextOnEdge(1,3), :);

        end
            
        clear lExpandedNodesList lOpenPointsList;
        
% $Log: GetEdgeByGraphSearchTraverseBottomUp.m,v $
% Revision 1.2  2005/11/28 19:45:14  anton
% General Update from Siddharth:
% *: Handling failure cases in fiducial segmentation, when only partial region
%    is selected as region of interest for fiducial...however, note that the
%    region should not be too small
% *: Now the BB numbers, line numbers and ellipse numbers are displayed after
%    establishing correspondence to have a visual check for user that
%    correspondence established is correct; Note tht these are displayed only
%    on whole image, not zoomed up region of interest, because there user can
%    make changes....so just on whole image after segmentation & after user
%    says he accepts the results
% *: Few cases prev. unhandled in user intervention in fiducial segmentation
%    validation
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%