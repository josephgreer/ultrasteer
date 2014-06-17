function [mm,e]=sm_match_assist32(mmm,pt,tr,ex,ey,xo,yo,f)

[k,N]=size(mmm);
e=10000000;
mm=[0,0,0,0];
for i=1:k
    for j=1:k
        [p,d]=sm_recon_core2({pt{3}(mmm(i,3),:);pt{1}(mmm(j,1),:)},{tr{3};tr{1}},[ex(3);ex(1)],[ey(3);ey(1)],[xo(3);xo(1)],[yo(3);yo(1)],[f(3);f(1)]);
        p3=sm_backproj_one(p,tr{2},ex(2),ey(2),xo(2),yo(2),f(2));
        for m=1:k
            tmp=norm(abs(p3-pt{2}(mmm(m,2),:)));
            if tmp<e
                e=tmp;
                mm=[mmm(j,1),mmm(m,2),mmm(i,3),-3];
            end
        end
    end
end
