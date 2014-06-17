% $Id: FindEntropyThreshold.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	FindEntropyThreshold() 
% // file:		FindEntropyThreshold.m
% // author:		siddharth
% // date:		
% // description:	function that calculates the automatic threshold value
% for a gray level image using entropy minimization [1]
% [1] Sahoo et al, " Threshold selection using minimal histogram entropy
% difference", Opt. Eng. 36(7), 1997.
% // input:	I (image buffer for region of interest)
%           
% // output: iAutoThresh: calculated threshold value
% 
% // calling function: BrachtherapySeedSegmentation
% 
% // NOTE: This algorithm still does not always provide an optimal
% threshold value
% //-----------------------------------------------------------------------------
function [iAutoThresh] = FindEntropyThreshold(I)

[p, X] = imhist(I);


p = p/sum(p);

H = 0;

iThresh1 = 0;

iThresh2 = 0;

for k = 1:256
    if (p(k) > 0)
        H = H + p(k)*log2(p(k));
    end
end

H = -H;

Array = [];

for j = 1: 256


    
    if (p(j) > 0)
        
        p1 = 0;
        p2 = 0;
        Ht = 0;    
        H1 = 0;
        H2 = 0;
        
        for i = 1: j
            if (p(i) > 0)
                p1 = p1 + p(i);
            end
        end
        
        p2 = 1- p1;
        

        for i = 1: j
            if (p(i) > 0 && p1 > 0)
                H1 = H1 + (p(i)/p1)*(log2(p(i)/p1));
            end
            
        end

        for i = j+1: 256
            if (p(i) > 0 && p2 > 0)
                H2 = H2 + (p(i)/p2)*(log2(p(i)/p2));
            end           
            
        end
        

         Ht = H1;
     
         p2 = 1- p1;
     

         

        if (p1 > 0 && p2 > 0)
        
            Obj = (H1-H2)^2;

            Array = [Array; [Obj j-1]];
            
        end
        
        
        
        
    end
    
    
end

iMaxIndex = find( Array == max(Array(:,1)));

iThresh1 = Array(iMaxIndex(1),2);

iMinIndex = find( Array == min(Array(:,1)));

iThresh2 = Array(iMinIndex(1),2);

iAutoThresh = (iThresh2 + iThresh1)/2;



% $Log: FindEntropyThreshold.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%