function pt_2d=sm_backproj_general(pt_3d,tr,ex,ey,xo,yo,f)
%SM_BACKPROJ_ONE calculates the backprojection for one 3d point.
%
%PT_2D=SM_BACKPROJ_ONE(PT_3D,TR,EX,EY,XO,YO,F), where 
%PT_3D is a 1-by-3 vector for one 3d point, 
%tr is the 4-by-4 transform matrix,
%EX,EY the pixel sizes, 
%XO,YO the image center, 
%f the focal length,
%returns 1-by-2 vector for one 2d point, PT_2D.

[N,tmp]=size(pt_3d);
ptmp=[pt_3d';ones(1,N)];
pctmp=tr*ptmp;
pc=pctmp(1:3,:);

ctop=[-f/ex,0,xo;0,-f/ey,yo;0,0,1];
p2d=ctop*pc;
pt_2d=[p2d(1,:)./p2d(3,:);p2d(2,:)./p2d(3,:)]';

%pt_2d=round(pt_2d);
