function [pts_3d,ds]=sm_recon(pts_2d,trs,M,ex,ey,xo,yo,f)
%SM_RECON reconstructs 3D points.
%
%[PTS_3D,ds] = SM_RECON(PTS_2D,TRS,M,EX,EY,XO,YO,F), where
%PTS_2D is a cell of K N-by-2 matries of 2d points,
%TRS is a cell of K 4-by-4 transform matrices, 
%M is a 1-by-N value of matching pattern,
%EX and EY are the x,y pixel sizes,
%XO and YO are the x,y image centers,
%F is the focal length,
%returns a N-by-3 matrix of 3d points, PTS_3D, 
%and a N-by-1 vector of accuracy values, DS.

[n_seeds,tmp]=size(M);
[n_ims,tmp]=size(pts_2d);
pts_3d=zeros(n_seeds,3);
ds=zeros(n_seeds,1);
pt_2d=cell(n_ims,1);

for i=1:n_seeds
    for j=1:n_ims
        pt_2d{j}=pts_2d{j}(i,:);
    end
    [pts_3d(i,:),ds(i)]=sm_recon_one(pt_2d,trs,M(i),ex,ey,xo,yo,f);
end
