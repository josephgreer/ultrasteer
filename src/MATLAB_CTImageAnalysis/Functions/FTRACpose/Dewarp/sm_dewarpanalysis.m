function [ev0,evr0,ev1,ev2]=sm_dewarpanalysis(fpt,fpw,xs,ys,x_max,y_max,n_equ)

[dp_true,dp_warp,p_center]=sm_getcalidata2(fpt,fpw,xs,ys);
[np,tmp]=size(dp_true);
p_warp=dp_warp+repmat(p_center,np,1);
e0=dp_warp-dp_true;
ee0=sqrt(e0(:,1).^2+e0(:,2).^2);
e0mean=mean(ee0);
e0std=std(ee0);
e0min=min(ee0);
e0max=max(ee0);
ev0=[e0mean,e0std,e0min,e0max];

er0=abs(sqrt(dp_warp(:,1).^2+dp_warp(:,2).^2)-sqrt(dp_true(:,1).^2+dp_true(:,2).^2));
evr0=[mean(er0),std(er0),min(er0),max(er0)];

A1=sm_bernstein2d3(dp_true,dp_warp,p_warp,x_max,y_max,n_equ);
p_dewarp1=sm_ptdewarp(A1,p_warp,x_max,y_max,n_equ);
p_center1=p_dewarp1(1,:);
p_true1=dp_true+repmat(p_center1,np,1);
e1=p_dewarp1-p_true1;
ee1=sqrt(e1(:,1).^2+e1(:,2).^2);
e1mean=mean(ee1);
e1std=std(ee1);
e1min=min(ee1);
e1max=max(ee1);
ev1=[e1mean,e1std,e1min,e1max];

p_true=dp_true+repmat(p_center,np,1);
A2=sm_bernstein2d4(p_true,p_warp,x_max,y_max,n_equ);
p_dewarp2=sm_ptdewarp(A2,p_warp,x_max,y_max,n_equ);
p_center2=p_dewarp2(1,:);
p_true2=p_true;%dp_true+repmat(p_center2,np,1);
e2=p_dewarp2-p_true2;
ee2=sqrt(e2(:,1).^2+e2(:,2).^2);
e2mean=mean(ee2);
e2std=std(ee2);
e2min=min(ee2);
e2max=max(ee2);
ev2=[e2mean,e2std,e2min,e2max];
