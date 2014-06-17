% $Id: IsNodeInTheList.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	IsNodeInTheList() 
% // file:		EdgeExtraction.cpp
% // author:		siddharth
% // date:		
% // task:		looks for a particular node to search for in the provided list of nodes
% //				if the node to search for is already in the list based on point coordinate location,
% //				then two possibilities:
% //				1) the node in the list has a lower associated cost, 
% //					in that case the function does nothing morereturns true
% //				2) the node in the list has a higher associated cost, compared to the one passed, 
% //					then the function removes the existing identical node having high cost & return false
% //					to make room for the same node to be added later but having lower cost, 
% //					possibly because its coming from another node
% // parameter:	node to search, nodes list to look in
% // return value: true or false
% // NOTE: This function is required for implementíng one of the steps of the algorithm
% //		 A-graph search (pseudo code given in the book 
% //		Image processing, analysis & understanding---Milan Sonka, under chapter Edge-based segmentation)
% //		
% //-----------------------------------------------------------------------------
% NODE= [POINT_X POINT_Y ConnectedNodeIndex dAssociatedCummulativeCost]
% NodesList = [ Node1;
%               Node2;
%               Node3......]

function [True, lNodesList] = IsNodeInTheList (nodeToSearch, lNodesList)

	iNumOfNodes = 0;

	iNumOfNodes = length(lNodesList);
    
    True = 0;

	if (iNumOfNodes > 0)
	
        iNodeIndex = 0;
        
        iNodeIndex = find((lNodesList(:,1) + 1000*lNodesList(:,2)) == (nodeToSearch(1,1) + 1000*nodeToSearch(1,2)));

        if (iNodeIndex)
            
            nCurrentNode = lNodesList(iNodeIndex,:);
            
            if	(nCurrentNode(1,4) <= nodeToSearch(1,4))
				True = 1;
			else
% 					//ths means the new node which wants to be added to the list has less associate cost
% 					//therefore the new node must be added & ths old node must be removed
% 					//remove the existing high cost node
			    lNodesList(iNodeIndex, :) = [];
                
            end
            
            
        end
        
        
    end
    
    

    
% $Log: IsNodeInTheList.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%

 