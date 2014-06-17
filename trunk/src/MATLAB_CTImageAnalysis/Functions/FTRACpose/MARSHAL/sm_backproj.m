function pts_2d=sm_backproj(pts_3d,tr,ex,ey,xo,yo,f)
%SM_BACKPROJ calculates the backprojection for a list of 3d points.
%
%PTS_2D=SM_BACKPROJ(PTS_3D,TR,EX,EY,XO,YO,F), where
%PTS_3D is a N-by-3 matrix for a list of 3d points, 
%tr is the 4-by-4 transform matrix, 
%EX,EY the pixel sizes, 
%XO,YO the image center,
%f the focal length, 
%returns N-by-2 matrix for 2d points.

[n_seeds,tmp]=size(pts_3d);
pts_2d=zeros(n_seeds,2);

for i=1:n_seeds
    pts_2d(i,:)=sm_backproj_one(pts_3d(i,:),tr,ex,ey,xo,yo,f);
end
