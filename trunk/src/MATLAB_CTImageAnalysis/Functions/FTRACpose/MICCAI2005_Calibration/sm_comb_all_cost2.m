function ee=sm_comb_all_cost2(bb,pts_2d,trs,ex,ey,xo,yo,f)
%SM_COMB_ALL_COST generates the list of matching accuracy values for the input list.
%
%EE=SM_COMB_ALL_COST(BB,PT_2D,TRS,EX,EY,XO,YO,F), where
%BB is the input list of matches,
%PTS_2D is a cell containing K N*2 matries for K images,
%TRS is a cell containing K 4*4 transform matrices for K images, 
%EX and EY are the x,y pixel sizes,
%XO and YO are the x,y image centers,
%F is the focal length,
%returns EE as the list of accuracy values corresponding to bb.  matching
%accurary

[n_ims,tmp]=size(pts_2d);
[n_comb,tmp]=size(bb);
ee=zeros(n_comb,1);
pt_2d=cell(n_ims,1);

for i=1:n_comb
    for j=1:n_ims
        pt_2d{j}=pts_2d{j}(bb(i,j),:);
    end
    [tmp,ee(i)]=sm_recon_core2(pt_2d,trs,ex,ey,xo,yo,f);
end
