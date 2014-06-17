function tab_c=sm_cost_matching_bp3(pts_2d,trs,ex,ey,xo,yo,f)
%SM_COST_MATCHING_BP3 generates the matrix of matching cost of back projection.
%
%TAB_C=SM_COST_MATCHING_BP3(FL,FTL,FR,FTR,EX,EY,XO,YO,F), where
%PTS_2D is a cell of 3 N-by-2 matries of 2d points,
%TRS is a cell of 3 4-by-4 transform matrices, 
%EX and EY are the x,y pixel sizes,
%XO and YO are the x,y image centers,
%F is the focal length,
%returns a N-by-N matrix of matching cost, tab_c.

%here pts_2d 1 and 2 are prior images, pts_2d 3 is the assisting image.
[n_seeds,tmp]=size(pts_2d{1});
[n_ims,tmp]=size(pts_2d);
tab_c=zeros(n_seeds);

for i=1:n_seeds
    for j=1:n_seeds
        pt_2d={pts_2d{1}(i,:);pts_2d{2}(j,:)};
        [pt_3d,d]=sm_recon_core(pt_2d,trs(1:2),ex,ey,xo,yo,f);
        for k=1:2
            p2d=sm_backproj_one(pt_3d,trs{k},ex,ey,xo,yo,f);
            err=norm(p2d-pt_2d{k});
            tab_c(i,j)=tab_c(i,j)+err;
        end
%        p2d=sm_backproj_one(pt_3d,trs{3},ex,ey,xo,yo,f);
%        errs=pts_2d{3}-repmat(p2d,n_seeds,1);
%        e_c=sqrt(errs(:,1).^2+errs(:,2).^2);
%        [err,m]=min(e_c);
%        tab_c(i,j)=tab_c(i,j)+err;
    end
end
tab_c=exp(tab_c);
