% $Id: ComputeRegionEnclosingCenter.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	ComputeRegionEnclosingCenter() 
% // file:		ComputeRegionEnclosingCenter.m
% // author:		siddharth
% // date:		
% // task:		computes a 5x5 window with given center as the center
% //				
% // parameter:  center, x,y bounds of the image
% // return value: defined region 's corner coordiates
% // NOTE:		
% //-----------------------------------------------------------------------------
function [RegionStartCornerX, RegionStartCornerY, RegionEndCornerX, RegionEndCornerY] = ComputeRegionEnclosingCenter(CurrentEllipseCenter, iX_Dimension, iY_Dimension)

RegionStartCornerX = CurrentEllipseCenter(1,1);

RegionEndCornerX = CurrentEllipseCenter(1,1);

RegionStartCornerY = CurrentEllipseCenter(1,2);

RegionEndCornerY = CurrentEllipseCenter(1,2);

% RegionStartCornerX
 if (CurrentEllipseCenter(1,1) - 3 >= 1)
     RegionStartCornerX = CurrentEllipseCenter(1,1) - 3;
 else

 if (CurrentEllipseCenter(1,1) - 2 >= 1)
     RegionStartCornerX = CurrentEllipseCenter(1,1) - 2;
 else
    if (CurrentEllipseCenter(1,1) - 1 >= 1)
         RegionStartCornerX = CurrentEllipseCenter(1,1) - 1;
     else
         if(CurrentEllipseCenter(1,1) >= 1)
             RegionStartCornerX = CurrentEllipseCenter(1,1);
         end
     end
        
 end
 
end

% RegionStartCornerY
 if (CurrentEllipseCenter(1,2) - 3 >= 1)
     RegionStartCornerY = CurrentEllipseCenter(1,2) - 3;
 else

 if (CurrentEllipseCenter(1,2) - 2 >= 1)
     RegionStartCornerY = CurrentEllipseCenter(1,2) - 2;
 else
     if (CurrentEllipseCenter(1,2) - 1 >= 1)
         RegionStartCornerY = CurrentEllipseCenter(1,2) - 1;
     else
         if(CurrentEllipseCenter(1,2) >= 1)
             RegionStartCornerY = CurrentEllipseCenter(1,2);
         end
     end
        
 end

end
% RegionEndCornerX
% 
 if (CurrentEllipseCenter(1,1) + 2 <= iX_Dimension)
     RegionEndCornerX = CurrentEllipseCenter(1,1) + 3;
 else

 if (CurrentEllipseCenter(1,1) + 2 <= iX_Dimension)
     RegionEndCornerX = CurrentEllipseCenter(1,1) + 2;
 else
    if (CurrentEllipseCenter(1,1) + 1 <= iX_Dimension)
         RegionEndCornerX = CurrentEllipseCenter(1,1) + 1;
     else
         if(CurrentEllipseCenter(1,1) <= iX_Dimension)
             RegionEndCornerX = CurrentEllipseCenter(1,1);
         end
     end
        
 end

end

% RegionEndCornerY

 if (CurrentEllipseCenter(1,2) + 2 <= iY_Dimension)
     RegionEndCornerY = CurrentEllipseCenter(1,2) + 3;
 else

 if (CurrentEllipseCenter(1,2) + 2 <= iY_Dimension)
     RegionEndCornerY = CurrentEllipseCenter(1,2) + 2;
 else
    if (CurrentEllipseCenter(1,2) + 1 <= iY_Dimension)
         RegionEndCornerY = CurrentEllipseCenter(1,2) + 1;
     else
         if(CurrentEllipseCenter(1,2) <= iY_Dimension)
             RegionEndCornerY = CurrentEllipseCenter(1,2);
         end
     end
        
 end
end


% $Log: ComputeRegionEnclosingCenter.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%