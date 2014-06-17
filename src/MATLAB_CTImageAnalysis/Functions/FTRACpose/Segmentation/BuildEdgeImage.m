%  $Id: BuildEdgeImage.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	BuildEdgeImage() 
% // file:		BuildEdgeImage.m
% // author:		siddharth
% // date:		
% // description:	form an edge image by implementing derivative magnitude
% filter, with a kernel of size specified using the Prewitt operators

% // input:	I (image buffer) 
%           kernel size (assume a square kernel)

% // output: edge image buffer

% // calling function: called when Select region button is pressed, after
% BB segmentation is done; this edge image formed is a prerequisite 
% for the line segmentation algorithm

% // NOTE: Maximum kernel size handled is 4
% //-----------------------------------------------------------------------------
function [IedgeImage] = BuildEdgeImage(I,iKernelSize)

    iX_Dimension = length(I(:,1));
    
    iY_Dimension = length(I(1,:));

    % variables
    iIntensityVal = 0; iNewIntensityVal = 0;

	iX_TempSum = 0; iY_TempSum = 0;

	iX_Index = 0; iY_Index = 0;
    
    if(iKernelSize == 2)
        
         %   form the convolution kernel
        KernelY = [-1 1; -1 1]; 
    
        KernelX = [-1 -1; 1 1];
  
        for iX_Index = 1:iX_Dimension				
				for iY_Index = 1:iY_Dimension

                    iX_TempSum = 0;

					iY_TempSum = 0;

% 					//x-convolution & y-convolution

                    % 					//x-1,y-1
					if ((iY_Index > 1) && (iX_Index > 1))
                        
					    iX_TempSum = iX_TempSum + KernelX (1,1) * I(iX_Index-1, iY_Index-1);

						iY_TempSum = iY_TempSum + KernelY (1,1) * I(iX_Index-1, iY_Index-1);
                    end

                    

                    % 					//x-1,y+1
					if ( (iX_Index > 1) && ((iY_Index+1) < iY_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (1,2) * I(iX_Index-1, iY_Index+1);

						iY_TempSum = iY_TempSum + KernelY (1,2) * I(iX_Index-1, iY_Index+1);
                    end

                   

                    % 					//x+1, y-1
					if ((iY_Index > 1) && ((iX_Index+1) < iX_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (2,1) * I(iX_Index+1, iY_Index-1);
					
						iY_TempSum = iY_TempSum + KernelY (2,1) * I(iX_Index+1, iY_Index-1);
                        
                    end
                        

                    % 					//x+1,y+1
					if ( ((iX_Index+1) < iX_Dimension) && ((iY_Index+1) < iY_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (2,2) * I(iX_Index+1, iY_Index+1);
						
						iY_TempSum = iY_TempSum + KernelY (2,2) * I(iX_Index+1, iY_Index+1);
                    end

					dSumOfSquares = (iX_TempSum*iX_TempSum) + (iY_TempSum*iY_TempSum);

					iNewIntensityVal = sqrt( dSumOfSquares);

                    if (iNewIntensityVal > 255)
                        iNewIntensityVal = 255;
                    end
                    
					IedgeImage(iX_Index, iY_Index) = round(iNewIntensityVal);	

                end
                
            end

        
        
    end
    
    
    
    if(iKernelSize == 3)
        
        %   form the convolution kernel
        KernelY = [-1 0 1; -1 0 1; -1 0 1]; 
    
        KernelX = [ 1 1 1; 0 0 0; -1 -1 -1];

            for iX_Index = 1:iX_Dimension				
				for iY_Index = 1:iY_Dimension

                    iX_TempSum = 0;

					iY_TempSum = 0;

% 					//x-convolution & y-convolution

                    % 					//x-1,y-1
					if ((iY_Index > 1) && (iX_Index > 1))
                        
					    iX_TempSum = iX_TempSum + KernelX (1,1) * I(iX_Index-1, iY_Index-1);

						iY_TempSum = iY_TempSum + KernelY (1,1) * I(iX_Index-1, iY_Index-1);
                    end

                    % 					//x-1,y
					if (iX_Index > 1)
					
						iX_TempSum = iX_TempSum + KernelX (1,2) * I(iX_Index-1, iY_Index);

						iY_TempSum = iY_TempSum + KernelY (1,2) * I(iX_Index-1, iY_Index);
                    end


                    % 					//x-1,y+1
					if ( (iX_Index > 1) && ((iY_Index+1) < iY_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (1,3) * I(iX_Index-1, iY_Index+1);

						iY_TempSum = iY_TempSum + KernelY (1,3) * I(iX_Index-1, iY_Index+1);
                    end

                    % 					//x,y-1
					if (iY_Index > 1)
					
						iX_TempSum = iX_TempSum + KernelX (2,1) * I(iX_Index, iY_Index-1);
					
						iY_TempSum = iY_TempSum + KernelY (2,1) * I(iX_Index, iY_Index-1);
                    end

                    % 					//x,y
					if (1)
					
						iX_TempSum = iX_TempSum + KernelX (2,2) * I(iX_Index, iY_Index);
					
						iY_TempSum = iY_TempSum + KernelY (2,2) * I(iX_Index, iY_Index);
                        
                    end

                    % 					//x,y+1
					if ((iY_Index+1) < iY_Dimension)
					
						iX_TempSum = iX_TempSum + KernelX (2,3) * I(iX_Index, iY_Index+1);

						iY_TempSum = iY_TempSum + KernelY (2,3) * I(iX_Index, iY_Index+1);
                        
                    end

                    % 					//x+1, y-1
					if ((iY_Index > 1) && ((iX_Index+1) < iX_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (3,1) * I(iX_Index+1, iY_Index-1);
					
						iY_TempSum = iY_TempSum + KernelY (3,1) * I(iX_Index+1, iY_Index-1);
                        
                    end
                        

                    % 					//x+1,y
					if ((iX_Index+1) < iX_Dimension)
					
						iX_TempSum = iX_TempSum + KernelX (3,2) * I(iX_Index+1, iY_Index);
					
						iY_TempSum = iY_TempSum + KernelY (3,2) * I(iX_Index+1, iY_Index);
                        
                    end

                    % 					//x+1,y+1
					if ( ((iX_Index+1) < iX_Dimension) && ((iY_Index+1) < iY_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (3,3) * I(iX_Index+1, iY_Index+1);
						
						iY_TempSum = iY_TempSum + KernelY (3,3) * I(iX_Index+1, iY_Index+1);
                    end

					dSumOfSquares = (iX_TempSum*iX_TempSum) + (iY_TempSum*iY_TempSum);

					iNewIntensityVal = sqrt( dSumOfSquares);

                    if (iNewIntensityVal > 255)
                        iNewIntensityVal = 255;
                    end
                    
					IedgeImage(iX_Index, iY_Index) = round(iNewIntensityVal);	

                end
                
            end

    end
    
    
    if (iKernelSize == 4)
        %     Kernel X
        KernelX = [ 3 3 3 3; 1 1 1 1; -1 -1 -1 -1; -3 -3 -3 -3];
        %     Kernel Y
        KernelY = [ -3 -1 1 3; -3 -1 1 3; -3 -1 1 3; -3 -1 1 3];
        
             for iX_Index = 1:iX_Dimension				
				for iY_Index = 1:iY_Dimension

                    iX_TempSum = 0;

					iY_TempSum = 0;

% 					//x-convolution & y-convolution

                    % 					//x-2,y-2
					if ((iY_Index > 2) && (iX_Index > 2))
                        
					    iX_TempSum = iX_TempSum + KernelX (1,1) * I(iX_Index-1, iY_Index-1);

						iY_TempSum = iY_TempSum + KernelY (1,1) * I(iX_Index-1, iY_Index-1);
                    end

                    % 					//x-2,y-1
					if (iX_Index > 2 && iY_Index > 1)
					
						iX_TempSum = iX_TempSum + KernelX (1,2) * I(iX_Index-2, iY_Index-1);

						iY_TempSum = iY_TempSum + KernelY (1,2) * I(iX_Index-2, iY_Index-1);
                    end


                    % 					//x-2,y+1
					if ( (iX_Index > 2) && ((iY_Index+1) < iY_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (1,3) * I(iX_Index-2, iY_Index+1);

						iY_TempSum = iY_TempSum + KernelY (1,3) * I(iX_Index-2, iY_Index+1);
                    end

                    % 					//x-2,y+2
					if ( (iX_Index > 2) && ((iY_Index+2) < iY_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (1,4) * I(iX_Index-2, iY_Index+2);

						iY_TempSum = iY_TempSum + KernelY (1,4) * I(iX_Index-2, iY_Index+2);
                    end
                    
                    % 					//x-1,y-2
					if (iX_Index > 1 && iY_Index > 2)
					
						iX_TempSum = iX_TempSum + KernelX (2,1) * I(iX_Index-1, iY_Index-2);
					
						iY_TempSum = iY_TempSum + KernelY (2,1) * I(iX_Index-1, iY_Index-2);
                    end

                    % 					//x-1,y-1
					if (iX_Index > 1 && iY_Index > 1)
					
						iX_TempSum = iX_TempSum + KernelX (2,2) * I(iX_Index-1, iY_Index-1);
					
						iY_TempSum = iY_TempSum + KernelY (2,2) * I(iX_Index-1, iY_Index-1);
                        
                    end

                    % 					//x-1,y+1
					if ((iY_Index+1) < iY_Dimension && iX_Index > 1)
					
						iX_TempSum = iX_TempSum + KernelX (2,3) * I(iX_Index-1, iY_Index+1);

						iY_TempSum = iY_TempSum + KernelY (2,3) * I(iX_Index-1, iY_Index+1);
                        
                    end

                    % 					//x-1,y+2
					if ((iY_Index+2) < iY_Dimension && iX_Index > 1)
					
						iX_TempSum = iX_TempSum + KernelX (2,4) * I(iX_Index-1, iY_Index+2);

						iY_TempSum = iY_TempSum + KernelY (2,4) * I(iX_Index-1, iY_Index+2);
                        
                    end
                    
                    % 					//x+1, y-1
					if ((iY_Index > 1) && ((iX_Index+1) < iX_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (3,1) * I(iX_Index+1, iY_Index-1);
					
						iY_TempSum = iY_TempSum + KernelY (3,1) * I(iX_Index+1, iY_Index-1);
                        
                    end
                        

                    % 					//x+1,y-2
					if ((iX_Index+1) < iX_Dimension  && iY_Index > 2)
					
						iX_TempSum = iX_TempSum + KernelX (3,2) * I(iX_Index+1, iY_Index-2);
					
						iY_TempSum = iY_TempSum + KernelY (3,2) * I(iX_Index+1, iY_Index-2);
                        
                    end

                    % 					//x+1,y+1
					if ( ((iX_Index+1) < iX_Dimension) && ((iY_Index+1) < iY_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (3,3) * I(iX_Index+1, iY_Index+1);
						
						iY_TempSum = iY_TempSum + KernelY (3,3) * I(iX_Index+1, iY_Index+1);
                    end


                    % 					//x+1,y+2
					if ( ((iX_Index+1) < iX_Dimension) && ((iY_Index+2) < iY_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (3,4) * I(iX_Index+1, iY_Index+2);
						
						iY_TempSum = iY_TempSum + KernelY (3,4) * I(iX_Index+1, iY_Index+2);
                    end

 % 					//x+2, y-1
					if ((iY_Index > 1) && ((iX_Index+2) < iX_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (4,1) * I(iX_Index+2, iY_Index-1);
					
						iY_TempSum = iY_TempSum + KernelY (4,1) * I(iX_Index+2, iY_Index-1);
                        
                    end
                        

                    % 					//x+2,y-2
					if ((iX_Index+2) < iX_Dimension  && iY_Index > 2)
					
						iX_TempSum = iX_TempSum + KernelX (4,2) * I(iX_Index+2, iY_Index-2);
					
						iY_TempSum = iY_TempSum + KernelY (4,2) * I(iX_Index+2, iY_Index-2);
                        
                    end

                    % 					//x+2,y+1
					if ( ((iX_Index+2) < iX_Dimension) && ((iY_Index+1) < iY_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (4,3) * I(iX_Index+1, iY_Index+1);
						
						iY_TempSum = iY_TempSum + KernelY (4,3) * I(iX_Index+1, iY_Index+1);
                    end


                    % 					//x+2,y+2
					if ( ((iX_Index+2) < iX_Dimension) && ((iY_Index+2) < iY_Dimension))
					
						iX_TempSum = iX_TempSum + KernelX (4,4) * I(iX_Index+1, iY_Index+2);
						
						iY_TempSum = iY_TempSum + KernelY (4,4) * I(iX_Index+1, iY_Index+2);
                    end

                    dSumOfSquares = (iX_TempSum*iX_TempSum) + (iY_TempSum*iY_TempSum);

					iNewIntensityVal = sqrt( dSumOfSquares);

                    if (iNewIntensityVal > 255)
                        iNewIntensityVal = 255;
                    end
                    
					IedgeImage(iX_Index, iY_Index) = round(iNewIntensityVal);	

                end
                
            end


        
    
    end
    


	
            

% $Log: BuildEdgeImage.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%

