%sm_dewarp
t0=clock;
xs=0.324;
ys=0.309;
n_equ=5;
x_max=960;
y_max=1280;
%im31
[ev10,evr10,ev11,ev12]=sm_dewarpanalysis('grid31ture.txt','grid31.txt',xs,ys,x_max,y_max,n_equ);
%im32
[ev20,evr20,ev21,ev22]=sm_dewarpanalysis('grid31ture.txt','grid32.txt',xs,ys,x_max,y_max,n_equ);
%im33
[ev30,evr30,ev31,ev32]=sm_dewarpanalysis('grid33ture.txt','grid33.txt',xs,ys,x_max,y_max,n_equ);
%im34
[ev40,evr40,ev41,ev42]=sm_dewarpanalysis('grid34ture.txt','grid34.txt',xs,ys,x_max,y_max,n_equ);
%im35
[ev50,evr50,ev51,ev52]=sm_dewarpanalysis('grid35ture.txt','grid35.txt',xs,ys,x_max,y_max,n_equ);
%im36
[ev60,evr60,ev61,ev62]=sm_dewarpanalysis('grid31ture.txt','grid36.txt',xs,ys,x_max,y_max,n_equ);
evr0=[evr10;evr20;evr30;evr40;evr50;evr60];


[dp_true1,dp_warp1,p_center1,np1]=sm_getcalidata2('grid31ture.txt','grid31.txt',xs,ys);
[dp_true2,dp_warp2,p_center2,np2]=sm_getcalidata2('grid31ture.txt','grid32.txt',xs,ys);
[dp_true3,dp_warp3,p_center3,np3]=sm_getcalidata2('grid33ture.txt','grid33.txt',xs,ys);
[dp_true4,dp_warp4,p_center4,np4]=sm_getcalidata2('grid34ture.txt','grid34.txt',xs,ys);
[dp_true5,dp_warp5,p_center5,np5]=sm_getcalidata2('grid35ture.txt','grid35.txt',xs,ys);
[dp_true6,dp_warp6,p_center6,np6]=sm_getcalidata2('grid31ture.txt','grid36.txt',xs,ys);
dp_true=[dp_true1;dp_true2;dp_true3;dp_true4;dp_true5;dp_true6];
dp_warp=[dp_warp1;dp_warp2;dp_warp3;dp_warp4;dp_warp5;dp_warp6];
p_warp1=dp_warp1+repmat(p_center1,np1,1);
p_warp2=dp_warp2+repmat(p_center2,np2,1);
p_warp3=dp_warp3+repmat(p_center3,np3,1);
p_warp4=dp_warp4+repmat(p_center4,np4,1);
p_warp5=dp_warp5+repmat(p_center5,np5,1);
p_warp6=dp_warp6+repmat(p_center6,np6,1);
p_warp=[p_warp1;p_warp2;p_warp3;p_warp4;p_warp5;p_warp6];
A1=sm_bernstein2d3(dp_true,dp_warp,p_warp,x_max,y_max,n_equ);

p_dewarp=sm_ptdewarp(A1,p_warp,x_max,y_max,n_equ);
p_dewarp1=p_dewarp(1:np1,:);
p_dewarp2=p_dewarp(np1+1:np1+np2,:);
p_dewarp3=p_dewarp(np1+np2+1:np1+np2+np3,:);
p_dewarp4=p_dewarp(np1+np2+np3+1:np1+np2+np3+np4,:);
p_dewarp5=p_dewarp(np1+np2+np3+np4+1:np1+np2+np3+np4+np5,:);
p_dewarp6=p_dewarp(np1+np2+np3+np4+np5+1:np1+np2+np3+np4+np5+np6,:);
p_center1=p_dewarp1(1,:);
p_center2=p_dewarp2(1,:);
p_center3=p_dewarp3(1,:);
p_center4=p_dewarp4(1,:);
p_center5=p_dewarp5(1,:);
p_center6=p_dewarp6(1,:);
dp_dewarp1=p_dewarp1-repmat(p_center1,np1,1);
dp_dewarp2=p_dewarp2-repmat(p_center2,np2,1);
dp_dewarp3=p_dewarp3-repmat(p_center3,np3,1);
dp_dewarp4=p_dewarp4-repmat(p_center4,np4,1);
dp_dewarp5=p_dewarp5-repmat(p_center5,np5,1);
dp_dewarp6=p_dewarp6-repmat(p_center6,np6,1);
dp_dewarp=[dp_dewarp1;dp_dewarp2;dp_dewarp3;dp_dewarp4;dp_dewarp5;dp_dewarp6];
p_true1=dp_true1+repmat(p_center1,np1,1);
p_true2=dp_true2+repmat(p_center2,np2,1);
p_true3=dp_true3+repmat(p_center3,np3,1);
p_true4=dp_true4+repmat(p_center4,np4,1);
p_true5=dp_true5+repmat(p_center5,np5,1);
p_true6=dp_true6+repmat(p_center6,np6,1);
p_true=[p_true1;p_true2;p_true3;p_true4;p_true5;p_true6];

e=p_dewarp-p_true;
ee=sqrt(e(:,1).^2+e(:,2).^2);
ev=[mean(ee),std(ee),min(ee),max(ee)];
ee1=ee(1:np1,:);
ee2=ee(np1+1:np1+np2,:);
ee3=ee(np1+np2+1:np1+np2+np3,:);
ee4=ee(np1+np2+np3+1:np1+np2+np3+np4,:);
ee5=ee(np1+np2+np3+np4+1:np1+np2+np3+np4+np5,:);
ee6=ee(np1+np2+np3+np4+np5+1:np1+np2+np3+np4+np5+np6,:);
ev1=[mean(ee1),std(ee1),min(ee1),max(ee1)];
ev2=[mean(ee2),std(ee2),min(ee2),max(ee2)];
ev3=[mean(ee3),std(ee3),min(ee3),max(ee3)];
ev4=[mean(ee4),std(ee4),min(ee4),max(ee4)];
ev5=[mean(ee5),std(ee5),min(ee5),max(ee5)];
ev6=[mean(ee6),std(ee6),min(ee6),max(ee6)];

er=abs(sqrt(dp_dewarp(:,1).^2+dp_dewarp(:,2).^2)-sqrt(dp_true(:,1).^2+dp_true(:,2).^2));
erv=[mean(er),std(er),min(er),max(er)];
er1=er(1:np1,:);
er2=er(np1+1:np1+np2,:);
er3=er(np1+np2+1:np1+np2+np3,:);
er4=er(np1+np2+np3+1:np1+np2+np3+np4,:);
er5=er(np1+np2+np3+np4+1:np1+np2+np3+np4+np5,:);
er6=er(np1+np2+np3+np4+np5+1:np1+np2+np3+np4+np5+np6,:);
erv1=[mean(er1),std(er1),min(er1),max(er1)];
erv2=[mean(er2),std(er2),min(er2),max(er2)];
erv3=[mean(er3),std(er3),min(er3),max(er3)];
erv4=[mean(er4),std(er4),min(er4),max(er4)];
erv5=[mean(er5),std(er5),min(er5),max(er5)];
erv6=[mean(er6),std(er6),min(er6),max(er6)];

ev_ind=[ev10;ev11;ev12;ev1;erv1;...
        ev20;ev21;ev22;ev2;erv2;...
        ev30;ev31;ev32;ev3;erv3;...
        ev40;ev41;ev42;ev4;erv4;...
        ev50;ev51;ev52;ev5;erv5;...
        ev60;ev61;ev62;ev6;erv6];


%A2=sm_bernstein2d3(dp_warp,dp_dewarp,p_dewarp,x_max,y_max,n_equ);

%I_new1=sm_dewarpshow('dataset_grid\im_00031.jpeg',A2,p_warp1,p_dewarp1,dp_true1,n_equ);
%I_new1=sm_dewarpshow('dataset_grid\im_00032.jpeg',A2,p_warp2,p_dewarp2,dp_true2,n_equ);
%I_new1=sm_dewarpshow('dataset_grid\im_00033.jpeg',A2,p_warp3,p_dewarp3,dp_true3,n_equ);
%I_new1=sm_dewarpshow('dataset_grid\im_00034.jpeg',A2,p_warp4,p_dewarp4,dp_true4,n_equ);
%I_new1=sm_dewarpshow('dataset_grid\im_00035.jpeg',A2,p_warp5,p_dewarp5,dp_true5,n_equ);
%I_new1=sm_dewarpshow('dataset_grid\im_00036.jpeg',A2,p_warp6,p_dewarp6,dp_true6,n_equ);

%I_new1=sm_imdewarp2('dataset_grid\im_00031.jpeg',A2,n_equ);
%I_new2=sm_imdewarp2('dataset_grid\im_00032.jpeg',A2,n_equ);
%I_new3=sm_imdewarp2('dataset_grid\im_00033.jpeg',A2,n_equ);
%I_new4=sm_imdewarp2('dataset_grid\im_00034.jpeg',A2,n_equ);
%I_new5=sm_imdewarp2('dataset_grid\im_00035.jpeg',A2,n_equ);
%I_new6=sm_imdewarp2('dataset_grid\im_00036.jpeg',A2,n_equ);

etime(clock,t0)