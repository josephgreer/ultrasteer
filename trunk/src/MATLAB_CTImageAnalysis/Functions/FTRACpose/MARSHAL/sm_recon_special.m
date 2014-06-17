function [pts_3d,d]=sm_recon_special(pt_2d,trs,ex,ey,xo,yo,f)

[N,tmp]=size(pt_2d{1});
pt_3d=zeros(N,3);
d=zeros(N,1);
K=500;
if N<=K
    [pt_3d,d]=sm_recon_general(pt_2d,trs,ex,ey,xo,yo,f);
else
    cc=0;
    while cc<N
        if cc+K<=N
            [pt_3d(cc+1:cc+K,:),d(cc+1:cc+K)].....
                =sm_recon_general({pt_2d{1}(cc+1:cc+K,:);pt_2d{2}(cc+1:cc+K,:)},trs,ex,ey,xo,yo,f);
            cc=cc+K;
        else
            [pt_3d(cc+1:N,:),d(cc+1:N)].....
                =sm_recon_general({pt_2d{1}(cc+1:N,:);pt_2d{2}(cc+1:N,:)},trs,ex,ey,xo,yo,f);
            cc=N;
        end
    end
end

