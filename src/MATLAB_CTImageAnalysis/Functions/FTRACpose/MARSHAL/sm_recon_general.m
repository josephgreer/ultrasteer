function [pt_3d,d]=sm_recon_general(pt_2d,trs,ex,ey,xo,yo,f)
%sm_recon_general reconstructs one or several 3D points.
%
%PT_3D = sm_recon_general(PT_2D,TRS,EX,EY,XO,YO,F), where
%PT_2D is a cell containing K n*2 matrices of K images,
%TRS is a cell containing K 4-by-4 transform matrices, 
%EX and EY are the x,y pixel sizes,
%XO and YO are the x,y image centers,
%F is the focal length,
%returns pt_3d as an N*3 matrix of reconstructed 3d points, 
%and d as a N-d vector of accuracy values.

[n_ims,tmp]=size(pt_2d);
[N,tmp]=size(pt_2d{1});

B1=zeros(3*N,3);
B2=zeros(3*N,1);
sa2b2=0;
sa2c2=0;
sb2c2=0;
sab=0;
sac=0;
sbc=0;
for i=1:n_ims
    pp{i}=pt_2d{i}-repmat([xo,yo],N,1);
    pp{i}=pp{i}.*repmat([ex,ey],N,1);
    pp{i}=[pp{i},-f*ones(N,1)]';
    ttoG{i}=inv(trs{i});
    oo{i}=ttoG{i}(1:3,4);
    ptmp=ttoG{i}*[pp{i};ones(1,N)];
    pn=ptmp(1:3,:);
    a=pn(1,:)-oo{i}(1);
    b=pn(2,:)-oo{i}(2);
    c=pn(3,:)-oo{i}(3);
    %method1
    a2b2c2=a.*a+b.*b+c.*c;
    a2b2=((a.*a+b.*b)./a2b2c2)';
    a2c2=((a.*a+c.*c)./a2b2c2)';
    b2c2=((b.*b+c.*c)./a2b2c2)';
    ab=((a.*b)./a2b2c2)';
    ac=((a.*c)./a2b2c2)';
    bc=((b.*c)./a2b2c2)';
%    sk=zeros(3*N,3*N);
%    sk(1:3:3*N,1:3:3*N)=diag(b2c2);
%    sk(1:3:3*N,2:3:3*N)=diag(-ab);
%    sk(1:3:3*N,3:3:3*N)=diag(-ac);
%    sk(2:3:3*N,1:3:3*N)=diag(-ab);
%    sk(2:3:3*N,2:3:3*N)=diag(a2c2);
%    sk(2:3:3*N,3:3:3*N)=diag(-bc);
%    sk(3:3:3*N,1:3:3*N)=diag(-ac);
%    sk(3:3:3*N,2:3:3*N)=diag(-bc);
%    sk(3:3:3*N,3:3:3*N)=diag(a2b2);
%    H{i}=sk;
    skk=zeros(3*N,3);
    skk(1:3:3*N,1)=b2c2;
    skk(1:3:3*N,2)=-ab;
    skk(1:3:3*N,3)=-ac;
    skk(2:3:3*N,1)=-ab;
    skk(2:3:3*N,2)=a2c2;
    skk(2:3:3*N,3)=-bc;
    skk(3:3:3*N,1)=-ac;
    skk(3:3:3*N,2)=-bc;
    skk(3:3:3*N,3)=a2b2;
    H{i}=skk;
    sa2b2=sa2b2+a2b2;
    sa2c2=sa2c2+a2c2;
    sb2c2=sb2c2+b2c2;
    sab=sab-ab;
    sac=sac-ac;
    sbc=sbc-bc;
    B1=B1+H{i};
    B2=B2+sum(skk'.*repmat(oo{i},1,3*N))';
    %method2
%    aa=diag(a);
%    bb=diag(b);
%    cc=diag(c);
%    dd=diag(1./(a.*a+b.*b+c.*c));
%    sk=zeros(3*N);
%    sk(1:3:3*N,2:3:3*N)=-cc;
%    sk(1:3:3*N,3:3:3*N)=bb;
%    sk(2:3:3*N,1:3:3*N)=cc;
%    sk(2:3:3*N,3:3:3*N)=-aa;
%    sk(3:3:3*N,1:3:3*N)=-bb;
%    sk(3:3:3*N,2:3:3*N)=aa;
%    sd=zeros(3*N);
%    sd(1:3:3*N,1:3:3*N)=dd;
%    sd(2:3:3*N,2:3:3*N)=dd;
%    sd(3:3:3*N,3:3:3*N)=dd;
%    H{i}=sk'*sk*sd;
%    B1=B1+H{i};
%    B2=B2+H{i}*repmat(oo{i},N,1);
end
mm=sa2b2.*sa2c2.*sb2c2+2*sab.*sac.*sbc-sa2b2.*sab.*sab-sa2c2.*sac.*sac-sb2c2.*sbc.*sbc;
m11=(sa2b2.*sa2c2-sbc.*sbc)./mm;
m22=(sa2b2.*sb2c2-sac.*sac)./mm;
m33=(sa2c2.*sb2c2-sab.*sab)./mm;
m12=(-sa2b2.*sab+sac.*sbc)./mm;
m13=(-sa2c2.*sac+sab.*sbc)./mm;
m23=(-sb2c2.*sbc+sab.*sac)./mm;
B1inv=zeros(3*N,3);
B1inv(1:3:3*N,1)=m11;
B1inv(1:3:3*N,2)=m12;
B1inv(1:3:3*N,3)=m13;
B1inv(2:3:3*N,1)=m12;
B1inv(2:3:3*N,2)=m22;
B1inv(2:3:3*N,3)=m23;
B1inv(3:3:3*N,1)=m13;
B1inv(3:3:3*N,2)=m23;
B1inv(3:3:3*N,3)=m33;
B22=reshape(repmat(reshape(B2,3,N),3,1),3,3*N);
pt_3d=sum(B1inv'.*B22)';

%pt_3d=inv(B1)*B2;

d=zeros(N,1);
for i=1:n_ims
    xn=pt_3d-repmat(oo{i},N,1);
    xnn=reshape(repmat(reshape(xn,3,N),3,1),3,3*N);
    tmp=sum(H{i}'.*xnn)';
    d=d+sqrt(sum(reshape(xn.*tmp,3,N))');
%    tmp=(xn'*H{i})'.*xn;
%    d=d+sqrt(tmp(1:3:3*N)+tmp(2:3:3*N)+tmp(3:3:3*N));
end

d=d/n_ims;
pt_3d=reshape(pt_3d,3,N)';