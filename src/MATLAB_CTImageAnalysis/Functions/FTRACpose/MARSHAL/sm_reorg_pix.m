function pts_2d_new=sm_reorg_pix(pts_2d_old,M)
%SM_REORG_PIX reorganizes the pixel matices according to matching results.
%
%PTS_2D_NEW=SM_REORG_PIX(PTS_2D_OLD,M), where
%PTS_2D_OLD is a cell of K N-by-2 matrices of the original pixels,
%M is a N-by-K matrix of matching,
%returns a cell of K N-by-2 matrices of the reorganized pixels, PTS_2D_NEW.

[n_seeds,n_ims]=size(M);
%n_ims=n_ims-1;
pts_2d_new=cell(n_ims,1);

for i=1:n_ims
    pts_2d_new{i}=pts_2d_old{i}(M(1:n_seeds,i),:);
end

