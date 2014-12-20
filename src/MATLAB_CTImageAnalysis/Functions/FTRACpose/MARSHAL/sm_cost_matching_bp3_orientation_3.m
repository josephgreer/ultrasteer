function tab_c=sm_cost_matching_bp3_orientation_3(pts_2d,ors,trs,ex,ey,xo,yo,f)

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
%[pt_3d,d]=sm_recon_general_2({pts_2d{1};pts_2d{2}},trs(1:2),ex(1:2),ey(1:2),xo(1:2),yo(1:2),f(1:2));
%recon the orientation from image 1 and 2;
os1=reshape(repmat(ors{1},1,n_seeds)',2,n_seeds*n_seeds)';
os2=repmat(ors{2},n_seeds,1);
oss=sm_recon_general_orientation({pt1;pt2},{os1;os2},trs(1:2),ex(1:2),ey(1:2),xo(1:2),yo(1:2),f(1:2));
%ostst=zeros(100,3);
%for i=1:100
%    ostst(i,:)=oss(100*(i-1)+i,:);
%end
%oss2=sm_recon_general_orientation({pts_2d{1};pts_2d{2}},{ors{1};ors{2}},trs(1:2),ex(1:2),ey(1:2),xo(1:2),yo(1:2),f(1:2));

%get two ends of the seed;
pt_3d1=pt_3d+r*oss/2;
pt_3d2=pt_3d-r*oss/2;
%project the seed to image 3;
pt2d=sm_backproj_general(pt_3d,trs{3},ex(3),ey(3),xo(3),yo(3),f(3));
pt2d1=sm_backproj_general(pt_3d1,trs{3},ex(3),ey(3),xo(3),yo(3),f(3));
pt2d2=sm_backproj_general(pt_3d2,trs{3},ex(3),ey(3),xo(3),yo(3),f(3));
%pts31=pts_2d{3}+ors{3}*6;
%pts32=pts_2d{3}-ors{3}*6;
%    for k=1:100
%        plot([pt2d1(k,1),pt2d2(k,1)],[pt2d1(k,2),pt2d2(k,2)]);
%        plot(pt2d(k,1),pt2d(k,2),'*');
%        plot(pts_2d{3}(k,1),pts_2d{3}(k,2),'*r');
%        plot([pts31(k,1),pts32(k,1)],[pts31(k,2),pts32(k,2)],'-r');
%    end
%get the planar error;
%center error;
a=repmat(pt2d,1,n_seeds)';
b=repmat(reshape(pts_2d{3}',2*n_seeds,1),1,n_seeds*n_seeds);
p2dtmp=reshape(a-b,2,n_seeds*n_seeds*n_seeds);
errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
tmp=reshape(errs,n_seeds,n_seeds*n_seeds);
%end error;
aa=repmat(pt2d1,1,n_seeds)';
bb=repmat(reshape(ors{3}',2*n_seeds,1),1,n_seeds*n_seeds);
%d1=sm_dist_ptline(reshape(aa,n_seeds^3,2),reshape(bb,n_seeds^3,2),reshape(b,n_seeds^3,2));
d1=sm_dist_ptline(reshape(aa,2,n_seeds^3)',reshape(bb,2,n_seeds^3)',reshape(b,2,n_seeds^3)');
tmp1=reshape(d1,n_seeds,n_seeds*n_seeds);
aaa=repmat(pt2d2,1,n_seeds)';
d2=sm_dist_ptline(reshape(aaa,2,n_seeds^3)',reshape(bb,2,n_seeds^3)',reshape(b,2,n_seeds^3)');
tmp2=reshape(d2,n_seeds,n_seeds*n_seeds);
%
p2dtmp=reshape(aa-b,2,n_seeds*n_seeds*n_seeds);
errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
tmp3=reshape(errs,n_seeds,n_seeds*n_seeds);
%
p2dtmp=reshape(aaa-b,2,n_seeds*n_seeds*n_seeds);
errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
tmp4=reshape(errs,n_seeds,n_seeds*n_seeds);

%find the smallest error in 3;
tmp=tmp+tmp1+tmp2+tmp3+tmp4;
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
e4=zeros(n_seeds*n_seeds,1);
e5=zeros(n_seeds*n_seeds,1);

for k=1:n_ims
	pt2d=sm_backproj_general(pt_3d,trs{k},ex(k),ey(k),xo(k),yo(k),f(k));
	pt2d1=sm_backproj_general(pt_3d1,trs{k},ex(k),ey(k),xo(k),yo(k),f(k));
	pt2d2=sm_backproj_general(pt_3d2,trs{k},ex(k),ey(k),xo(k),yo(k),f(k));
    p2dtmp=pt2d-pt_2d{k};
    errs=sqrt(p2dtmp(:,1).*p2dtmp(:,1)+p2dtmp(:,2).*p2dtmp(:,2));
    d1=sm_dist_ptline(pt2d1,os_2d{k},pt_2d{k});
    d2=sm_dist_ptline(pt2d2,os_2d{k},pt_2d{k});
    p2dtmp=pt2d1-pt_2d{k};
    errs2=sqrt(p2dtmp(:,1).*p2dtmp(:,1)+p2dtmp(:,2).*p2dtmp(:,2));
    p2dtmp=pt2d2-pt_2d{k};
    errs3=sqrt(p2dtmp(:,1).*p2dtmp(:,1)+p2dtmp(:,2).*p2dtmp(:,2));
    tt=tt+errs+d1+d2+errs2+errs3;
    e1=e1+errs;
    e2=e2+d1;
    e3=e3+d2;
    e4=e4+errs2;
    e5=e5+errs3;
end
tab_c=reshape(tt,n_seeds,n_seeds)';
