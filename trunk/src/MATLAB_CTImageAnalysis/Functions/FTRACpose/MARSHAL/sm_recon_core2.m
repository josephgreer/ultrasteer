function [pt_3d,d]=sm_recon_core2(pt_2d,trs,ex,ey,xo,yo,f)
%ex,ey,xo,yo,f)
%SM_RECON_CORE reconstructs one 3D point.
%
%PT_3D = SM_RECON_CORE(PT_2D,TRS,EX,EY,XO,YO,F), where
%PT_2D is a cell of K 1-by-2 vectors of 2d points,
%TRS is a cell of K 4-by-4 transform matrices, 
%EX and EY are the x,y pixel sizes,
%XO and YO are the x,y image centers,
%F is the focal length,
%returns a 1-by-3 vector of 3d points, and a value of accuracy.

[n_ims,tmp]=size(pt_2d);

B1=0;
B2=0;
for i=1:n_ims
    pp{i}=[(pt_2d{i}(1)-xo(i))*ex(i);(pt_2d{i}(2)-yo(i))*ey(i);-f(i)];
    ttoG{i}=inv(trs{i});
    oo{i}=ttoG{i}(1:3,4);
    ptmp=ttoG{i}*[pp{i};1];
    pn=ptmp(1:3,:);
    a=pn(1)-oo{i}(1);
    b=pn(2)-oo{i}(2);
    c=pn(3)-oo{i}(3);
    sk=[0,-c,b;c,0,-a;-b,a,0];
    H{i}=sk'*sk/(a^2+b^2+c^2);
    B1=B1+H{i};
    B2=B2+H{i}*oo{i};
end
pt_3d=inv(B1)*B2;

d=0;
for i=1:n_ims
    xn=pt_3d-oo{i};
    d=d+sqrt(xn'*H{i}*xn);
end

d=d/n_ims;
pt_3d=pt_3d';
