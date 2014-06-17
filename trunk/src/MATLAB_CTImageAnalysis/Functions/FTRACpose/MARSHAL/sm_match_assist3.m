function [mm,e]=sm_match_assist3(mmm,pt,tr,ex,ey,xo,yo,f)

[k,N]=size(mmm);
e=10000000;
mm=[0,0,0,0];
for i=1:k
    for j=1:k
        [p,d]=sm_recon_core2({pt{3}(mmm(i,3),:);pt{1}(mmm(j,1),:)},{tr{3};tr{1}},ex,ey,xo,yo,f);
        p3=sm_backproj_one(p,tr{2},ex,ey,xo,yo,f);
        for m=1:k
            tmp=norm(abs(p3-pt{2}(mmm(m,2),:)));
            if tmp<e
                e=tmp;
                mm=[mmm(j,1),mmm(m,2),mmm(i,3),-3];
            end
        end
    end
end
