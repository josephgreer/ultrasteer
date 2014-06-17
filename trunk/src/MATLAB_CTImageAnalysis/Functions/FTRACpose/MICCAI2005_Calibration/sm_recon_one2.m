function [pt_3d,d]=sm_recon_one2(pt_2d,trs,M,ex,ey,xo,yo,f)
%SM_RECON_ONE reconstructs one 3D point.
%
%[PT_3D,d] = SM_RECON_ONE(PT_2D,TRS,M,EX,EY,XO,YO,F), where
%PT_2D is a cell of K 1-by-2 vectors of 2d points,
%TRS is a cell of K 4-by-4 transform matrices, 
%M is a value of matching pattern,
%EX and EY are the x,y pixel sizes,
%XO and YO are the x,y image centers,
%F is the focal length,
%returns a 1-by-3 vector of 3d points, and a value of accuracy.

ppt_2d=cell(2,1);
ttrs=cell(2,1);
if M==0 | M==-4
    [pt_3d,d]=sm_recon_core2(pt_2d,trs,ex,ey,xo,yo,f);
elseif M==-1
    ppt_2d{1}=pt_2d{1};
    ppt_2d{2}=pt_2d{2};
    ttrs{1}=trs{1};
    ttrs{2}=trs{2};
    [pt_3d,d]=sm_recon_core2(ppt_2d,ttrs,ex(1:2),ey(1:2),xo(1:2),yo(1:2),f(1:2));
elseif M==-2
    ppt_2d{1}=pt_2d{2};
    ppt_2d{2}=pt_2d{3};
    ttrs{1}=trs{2};
    ttrs{2}=trs{3};
    [pt_3d,d]=sm_recon_core2(ppt_2d,ttrs,ex(2:3),ey(2:3),xo(2:3),yo(2:3),f(2:3));
else
    ppt_2d{1}=pt_2d{3};
    ppt_2d{2}=pt_2d{1};
    ttrs{1}=trs{3};
    ttrs{2}=trs{1};
    [pt_3d,d]=sm_recon_core2(ppt_2d,ttrs,[ex(3);ex(1)],[ey(3);ey(1)],[xo(3);xo(1)],[yo(3);yo(1)],[f(3);f(1)]);
end
