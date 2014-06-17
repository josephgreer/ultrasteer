function pt_2d=sm_backproj_one(pt_3d,tr,ex,ey,xo,yo,f)
%SM_BACKPROJ_ONE calculates the backprojection for one 3d point.
%
%PT_2D=SM_BACKPROJ_ONE(PT_3D,TR,EX,EY,XO,YO,F), where 
%PT_3D is a 1-by-3 vector for one 3d point, 
%tr is the 4-by-4 transform matrix,
%EX,EY the pixel sizes, 
%XO,YO the image center, 
%f the focal length,
%returns 1-by-2 vector for one 2d point, PT_2D.

ptmp=[pt_3d';1];
pctmp=tr*ptmp;
pc=pctmp(1:3);

pt_2d(1)=-pc(1)*f/pc(3)/ex+xo;
pt_2d(2)=-pc(2)*f/pc(3)/ey+yo;

%pt_2d=round(pt_2d);
