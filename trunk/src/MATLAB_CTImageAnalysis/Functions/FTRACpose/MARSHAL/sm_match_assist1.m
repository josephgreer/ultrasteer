function [mm,e]=sm_match_assist1(mmm,pt,tr,ex,ey,xo,yo,f)

[k,N]=size(mmm);
e=10000000;
mm=[0,0,0,0];
for i=1:k
    for j=1:k
        [p,d]=sm_recon_core2({pt{1}(mmm(i,1),:);pt{2}(mmm(j,2),:)},{tr{1};tr{2}},ex,ey,xo,yo,f);
        p3=sm_backproj_one(p,tr{3},ex,ey,xo,yo,f);
        for m=1:k
            tmp=norm(abs(p3-pt{3}(mmm(m,3),:)));
            if tmp<e
                e=tmp;
                mm=[mmm(i,1),mmm(j,2),mmm(m,3),-1];
            end
        end
    end
end
