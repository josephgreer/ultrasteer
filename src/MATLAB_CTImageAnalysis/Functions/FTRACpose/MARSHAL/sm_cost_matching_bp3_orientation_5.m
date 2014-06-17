function tab_c=sm_cost_matching_bp3_orientation_5(pts_2d,ors,trs,ex,ey,xo,yo,f)

%seed length
r=4.92;
%here pts_2d 1 and 2 are prior images, pts_2d 3 is the assisting image.
[n_seeds,tmp]=size(pts_2d{1});
[n_ims,tmp]=size(pts_2d);
tab_c=zeros(n_seeds);

%recon the line center from image 1 and 2;
pt1=reshape(repmat(pts_2d{1},1,n_seeds)',2,n_seeds*n_seeds)';
pt2=repmat(pts_2d{2},n_seeds,1);
[pt_3d,d]=sm_recon_general_2({pt1;pt2},trs(1:2),ex(1:2),ey(1:2),xo(1:2),yo(1:2),f(1:2));
%recon the orientation from image 1 and 2;
os1=reshape(repmat(ors{1},1,n_seeds)',2,n_seeds*n_seeds)';
os2=repmat(ors{2},n_seeds,1);
oss=sm_recon_general_orientation({pt1;pt2},{os1;os2},trs(1:2),ex(1:2),ey(1:2),xo(1:2),yo(1:2),f(1:2));

%get two ends of the seed;
pt_3d1=pt_3d+r*oss/2;
pt_3d2=pt_3d-r*oss/2;
%project the seed to image 3;
pt2d=sm_backproj_general(pt_3d,trs{3},ex(3),ey(3),xo(3),yo(3),f(3));
pt2d1=sm_backproj_general(pt_3d1,trs{3},ex(3),ey(3),xo(3),yo(3),f(3));
pt2d2=sm_backproj_general(pt_3d2,trs{3},ex(3),ey(3),xo(3),yo(3),f(3));

Ltmp1=pt2d-pt2d1;
Ltmp2=pt2d-pt2d2;
L1=sqrt(Ltmp1(:,1).^2+Ltmp1(:,2).^2);
L2=sqrt(Ltmp2(:,1).^2+Ltmp2(:,2).^2);
L=(L1+L2)/2;
%get the planar error;
%center error;
a=repmat(pt2d,1,n_seeds)';
b=repmat(reshape(pts_2d{3}',2*n_seeds,1),1,n_seeds*n_seeds);
p2dtmp=reshape(a-b,2,n_seeds*n_seeds*n_seeds);
errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
tmp=reshape(errs,n_seeds,n_seeds*n_seeds);
%end error;
aa=repmat(pt2d1,1,n_seeds)';
aaa=repmat(pt2d2,1,n_seeds)';
oo=repmat(reshape(ors{3}',2*n_seeds,1),1,n_seeds*n_seeds);
LL=repmat(L,1,2*n_seeds)';
LLL=LL.*oo;
bb=repmat(reshape(pts_2d{3}',2*n_seeds,1),1,n_seeds*n_seeds)+LLL;
bbb=repmat(reshape(pts_2d{3}',2*n_seeds,1),1,n_seeds*n_seeds)-LLL;

if norm(bb(1:2,1)-aa(1:2,1))>norm(bbb(1:2,1)-aa(1:2,1))
    bb=repmat(reshape(pts_2d{3}',2*n_seeds,1),1,n_seeds*n_seeds)-LLL;
    bbb=repmat(reshape(pts_2d{3}',2*n_seeds,1),1,n_seeds*n_seeds)+LLL;
end

p2dtmp=reshape(aa-bb,2,n_seeds*n_seeds*n_seeds);
errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
tmp1=reshape(errs,n_seeds,n_seeds*n_seeds);
p2dtmp=reshape(aaa-bbb,2,n_seeds*n_seeds*n_seeds);
errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
tmp2=reshape(errs,n_seeds,n_seeds*n_seeds);

%find the smallest error in 3;
tmp=tmp+tmp1+tmp2;
[vtmp,ind]=min(tmp);
pt_2d={pt1;pt2;pts_2d{3}(ind,:)};
os_2d={os1;os2;ors{3}(ind,:)};

%do the recon from 3 points;
[pt_3d,d]=sm_recon_general_2(pt_2d,trs,ex,ey,xo,yo,f);
oss=sm_recon_general_orientation(pt_2d,os_2d,trs,ex,ey,xo,yo,f);
pt_3d1=pt_3d+r*oss/2;
pt_3d2=pt_3d-r*oss/2;
%backproject to 3 images and make the cost table;
tt=zeros(n_seeds*n_seeds,1);
e1=zeros(n_seeds*n_seeds,1);
e2=zeros(n_seeds*n_seeds,1);
e3=zeros(n_seeds*n_seeds,1);

for k=1:n_ims
	pt2d=sm_backproj_general(pt_3d,trs{k},ex(k),ey(k),xo(k),yo(k),f(k));
	pt2d1=sm_backproj_general(pt_3d1,trs{k},ex(k),ey(k),xo(k),yo(k),f(k));
	pt2d2=sm_backproj_general(pt_3d2,trs{k},ex(k),ey(k),xo(k),yo(k),f(k));
    p2dtmp=pt2d-pt_2d{k};
    errs=sqrt(p2dtmp(:,1).*p2dtmp(:,1)+p2dtmp(:,2).*p2dtmp(:,2));
	Ltmp1=pt2d-pt2d1;
	Ltmp2=pt2d-pt2d2;
	L1=sqrt(Ltmp1(:,1).^2+Ltmp1(:,2).^2);
	L2=sqrt(Ltmp2(:,1).^2+Ltmp2(:,2).^2);
	L=(L1+L2)/2;
    LLL=[L,L].*os_2d{k};
    pt_2d1=pt_2d{k}+LLL;
    pt_2d2=pt_2d{k}-LLL;
	
	if norm(pt_2d1(1,:)-pt2d1(1,:))>norm(pt_2d1(1,:)-pt2d2(1,:))
        pt_2d1=pt_2d{k}-LLL;
        pt_2d2=pt_2d{k}+LLL;
	end

    p2dtmp=pt2d1-pt_2d1;
    d1=sqrt(p2dtmp(:,1).*p2dtmp(:,1)+p2dtmp(:,2).*p2dtmp(:,2));
    p2dtmp=pt2d2-pt_2d2;
    d2=sqrt(p2dtmp(:,1).*p2dtmp(:,1)+p2dtmp(:,2).*p2dtmp(:,2));
    tt=tt+errs+d1+d2;
    e1=e1+errs;
    e2=e2+d1;
    e3=e3+d2;
end
tab_c=reshape(tt,n_seeds,n_seeds)';

