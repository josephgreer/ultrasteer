function tab_c=sm_cost_matching_2im(pts_2d,trs,ex,ey,xo,yo,f)
%SM_COST_MATCHING_2IM generates the matrix of matching cost.
%
%TAB_C=SM_COST_MATCHING_2IM(FL,FTL,FR,FTR,EX,EY,XO,YO,F), where
%PTS_2D is a cell of 2 N-by-2 matries of 2d points,
%TRS is a cell of 2 4-by-4 transform matrices, 
%EX and EY are the x,y pixel sizes,
%XO and YO are the x,y image centers,
%F is the focal length,
%returns a N-by-N matrix of matching cost, tab_c.

pL_all=pts_2d{1}';
pR_all=pts_2d{2}';
tL=trs{1};
tR=trs{2};
tRtoL=tL*inv(tR);
R=tRtoL(1:3,1:3);
T=tRtoL(1:3,4);

[ntmp,n_pL]=size(pL_all);
[ntmp,n_pR]=size(pR_all);
pL_all=[(pL_all(1,:)-xo)*ex;(pL_all(2,:)-yo)*ey;-ones(1,n_pL)*f];
pR_all=[(pR_all(1,:)-xo)*ex;(pR_all(2,:)-yo)*ey;-ones(1,n_pR)*f];

tab_c=zeros(n_pL,n_pR);
for i=1:n_pL
    for j=1:n_pR
        abc=recon_triag(pL_all(:,i),pR_all(:,j),R,T);
        tab_c(i,j)=abc(3);
    end
end
tab_c=abs(tab_c)/2;
