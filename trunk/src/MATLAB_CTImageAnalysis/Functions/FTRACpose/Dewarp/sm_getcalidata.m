function [dp_true,dp_warp,p_center]=sm_getcalidata(ftrue,fwarp,xpx,ypx,xL,yL)

p_true=load(ftrue);
p_true(:,1)=p_true(:,1)*xpx/xL;
p_true(:,2)=p_true(:,2)*ypx/yL;
p_warp=load(fwarp);
p_center=p_warp(1,:);

[np,tmp]=size(p_warp);
dp_true=p_true-repmat(p_true(1,:),np,1);
dp_warp=p_warp-repmat(p_warp(1,:),np,1);
