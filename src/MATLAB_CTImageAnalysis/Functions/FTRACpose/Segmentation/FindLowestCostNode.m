% $Id: FindLowestCostNode.m,v 1.1 2005/10/27 19:10:47 svikal Exp $


% //-----------------------------------------------------------------------------
% // function:	FindLowestCostNode () 
% // file:		EdgeExtraction.cpp
% // author:		siddharth
% // date:		
% // task:		looks into the open nodes list & determines the node which has 
% //				got the minimum associated cost, so that it could be further expanded
% //				This minimum cost node is then removed from the list
% //				a node pointer to the node found is returned
% // parameter:	OPEN nodes list
% // return value: Pointer to the minimum cost node
% // NOTE: This function is required for implementíng one of the steps of the algorithm
% //		 A-graph search (pseudo code given in the book 
% //		Image processing, analysis & machine vision---Milan Sonka, under chapter Edge-based segmentation)
% //		
% //-----------------------------------------------------------------------------


% NODE= [POINT_X POINT_Y ConnectedNodeIndex dAssociatedCummulativeCost]
% NodesList = [ Node1;
%               Node2;
%               Node3......]

function [lExpandedNodesList, lOpenNodesList, nMinCostNode] = FindLowestCostNode (lOpenNodesList, lExpandedNodesList)

	iNumOfNodes = 0;

	dMinCost = 0;

	iNumOfNodes = length (lOpenNodesList(:,1));
    
    nMinCostNode = [];
    
    iMinCostNodeIndex = 1;

    if (iNumOfNodes > 0)
        
        nCurrentNode = lOpenNodesList(1,:);

    	nMinCostNode = nCurrentNode;

    	dMinCost = nCurrentNode(1,4);

    	for iNodeIndex = 1: iNumOfNodes
    
    		nCurrentNode = lOpenNodesList(iNodeIndex, :);
		
    		if (nCurrentNode(1,4) < dMinCost)
                nMinCostNode = nCurrentNode;
                iMinCostNodeIndex = iNodeIndex;
            end
                        
        end
        
%       //remove the min cost node from the open list
    	lOpenNodesList(iMinCostNodeIndex, :) = [];
        lExpandedNodesList = [lExpandedNodesList; nMinCostNode];       
        
    end
    
        
    
    
    
    % $Log: FindLowestCostNode.m,v $
    % Revision 1.1  2005/10/27 19:10:47  svikal
    % Added CVS tags
    %