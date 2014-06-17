function tab_c=sm_cost_matching_bp3_3d(pts_2d,trs,ex,ey,xo,yo,f)
%SM_COST_MATCHING_BP3_3d generates the 3d cost matrix for 3-image matching 
%based on back projection errors of 3 images.

[n_seeds,tmp]=size(pts_2d{1});
%[n_ims,tmp]=size(pts_2d);
tab_c=zeros(n_seeds^3,1);
pt=cell(3,1);

pt{1}=reshape(repmat(pts_2d{1},1,n_seeds^2)',2,n_seeds^3)';
pt{2}=repmat(reshape(repmat(pts_2d{2},1,n_seeds)',2,n_seeds^2)',n_seeds,1);
pt{3}=repmat(pts_2d{3},n_seeds^2,1);

pt_3d=zeros(n_seeds^3,3);
d=zeros(n_seeds^3,1);
for i=1:n_seeds
    i
    ps={pt{1}(n_seeds^2*(i-1)+1:n_seeds^2*i,:);pt{2}(n_seeds^2*(i-1)+1:n_seeds^2*i,:);pt{3}(n_seeds^2*(i-1)+1:n_seeds^2*i,:)};
    [ptmp,dtmp]=sm_recon_general_2(ps,trs,ex,ey,xo,yo,f);
    pt_3d(n_seeds^2*(i-1)+1:n_seeds^2*i,:)=ptmp;
    d(n_seeds^2*(i-1)+1:n_seeds^2*i,:)=dtmp;
end

for k=1:3
    pt2d=sm_backproj_general(pt_3d,trs{k},ex(k),ey(k),xo(k),yo(k),f(k));
    p2dtmp=pt2d-pt{k};
    errs=sqrt((p2dtmp(:,1)*ex(k)).^2+(p2dtmp(:,2)*ey(k)).^2);
    tab_c=tab_c+errs;
end
tab_c=tab_c/3;

%((err(:,1)*ex).^2+(err(:,2)*ey).^2)