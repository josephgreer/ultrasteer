% $Id: FindDirectionAssociatedCost.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	FindDirectionAssociatedCost() 
% // file:		FindDirectionAssociatedCost.m
% // author:		siddharth
% // date:		
% // description: finds a direction associated cost for adding a node, in
% two situations
% situation 1: initial direction estimate known: then it computes what
% would the new direction be if this node added to current path of nodes.
% If this node's addition causes the overall direction to be very
% different from estimated direction then, add a greater cost, if its very
% close, then add less cost. Note that when ver small number of points on
% line are known, then the new point addition is going to be more senistive
% that even one pixel off would cause a significant difference in overall
% direction, as compared to when alread some 50-100 points are known, then
% the same one piuxel off will not cause that significant change of
% direction; so this fact is taken into account in allowing the direction
% variations
% 
% situation 2: if the initial direction estimate is not known, then local
% directional variation is taken into account i.e. a direction is
% calculated by considering 3 points along with point in question & the
% direction of the remaining points, then the difference in direction
% contributes to the cost
% 
% // input:	nodeToAdd, Nodes List from where the pathe could be retrieved,
% flag indicating whether to use Initial direction estimate, Initial
% estimated line direction (used only if the flag is true)

% // output: Direction associated cost

% // calling function: GetEdgeByGraphSearchTraversingBottomUp,
% GetEdgeByGraphSearchTraversingUpBottom

% // NOTE: 
% //-----------------------------------------------------------------------------
function [dDirectionAssociatedCost] = FindDirectionAssociatedCost (nodeToAdd, lExpandedNodesList, bUseInitialEstimate, InitialEstimatedLineDirection)

dDirectionAssociatedCost = 0;

iNumOfNodes = length(lExpandedNodesList);

if (iNumOfNodes > 1)
    
    nodeTraverseIndex = nodeToAdd(1,3);
    
    iNodeCount = 0;

    if (nodeTraverseIndex > 0)
        nodeToTraverse = lExpandedNodesList(nodeTraverseIndex, :);

        StartNode = nodeToTraverse;
        
        IntermediateNode = StartNode;
        
        NextIntermediateNode = StartNode;
        
        while (nodeToTraverse(1,3) ~= 0)
            nodeTraverseIndex = nodeToTraverse(1,3);
            nodeToTraverse = lExpandedNodesList(nodeTraverseIndex, :);
            iNodeCount = iNodeCount + 1;
            if(iNodeCount == 3)
                IntermediateNode = nodeToTraverse;
                if(IntermediateNode(1,3)~=0)
                    NextIntermediateNode = lExpandedNodesList(IntermediateNode(1,3),:);
                else
                    NextIntermediateNode = IntermediateNode;
                end
            end
            
        end

        EndNode = nodeToTraverse;

        if (iNodeCount > 2)
            
            if (bUseInitialEstimate)
                
                InitialDirection = InitialEstimatedLineDirection;
                
                if( (EndNode(1,2) - nodeToAdd(1,2)) == 0)
                    FinalDirection = 90;
                else
                    FinalDirection = (atan( (EndNode(1,1) - nodeToAdd(1,1))/ (EndNode(1,2) - nodeToAdd(1,2))))*180/pi;
                    
                    if (FinalDirection < 0)
                        FinalDirection = 90 + (90+FinalDirection);
                    end
                end
                
                if (iNodeCount < 50)
                    if (abs(FinalDirection - InitialDirection) < 1)
                        dDirectionAssociatedCost = 3;  
                    else 
                        if (abs(FinalDirection - InitialDirection) < 2)
                            dDirectionAssociatedCost = 5;
                        else
                           if (abs(FinalDirection - InitialDirection) < 7)
                               dDirectionAssociatedCost = 10;
                           else                
                               dDirectionAssociatedCost = 15;
                           end
                        end
                    
                    end

                    
                else
                    if (abs(FinalDirection - InitialDirection) < 1)
                        dDirectionAssociatedCost = 3;  
                    else 
                        if (abs(FinalDirection - InitialDirection) < 2)
                           dDirectionAssociatedCost = 7;
                        else
                           if (abs(FinalDirection - InitialDirection) < 2.2)
                               dDirectionAssociatedCost = 10;
                           else                
                               dDirectionAssociatedCost = 15;
                           end
                        end
                
                    end

                    
                end
                          
                            
            else
                if( (EndNode(1,2) - NextIntermediateNode(1,2)) == 0)
                    InitialDirection = 90;
                else
                    InitialDirection = abs((atan( (EndNode(1,1) - NextIntermediateNode(1,1))/ (EndNode(1,2) - NextIntermediateNode(1,2))))*180/pi);
                end

                if( (IntermediateNode(1,2) - nodeToAdd(1,2)) == 0)
                    FinalDirection = 90;
                else
                    FinalDirection = abs((atan( (IntermediateNode(1,1) - nodeToAdd(1,1))/ (IntermediateNode(1,2) - nodeToAdd(1,2))))*180/pi);
                end
                
                          
                if (abs(FinalDirection - InitialDirection) < 10)
                    dDirectionAssociatedCost = 3;  
                else 
                    if (abs(FinalDirection - InitialDirection) < 20)
                       dDirectionAssociatedCost = 10;
                    else
                       if (abs(FinalDirection - InitialDirection) < 25)
                           dDirectionAssociatedCost = 12;
                       else
                           if (abs(FinalDirection - InitialDirection) < 27)
                               dDirectionAssociatedCost = 12;
                           else                
                               dDirectionAssociatedCost = 15;
                           end
                       end
                    end
                
                end
                
            end
        
        end
               
    
    end

end



% $Log: FindDirectionAssociatedCost.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%