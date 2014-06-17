function [mm,e]=sm_match_assist22(mmm,pt,tr,ex,ey,xo,yo,f)

[k,N]=size(mmm);
e=10000000;
mm=[0,0,0,0];
for i=1:k
    for j=1:k
        [p,d]=sm_recon_core2({pt{2}(mmm(i,2),:);pt{3}(mmm(j,3),:)},{tr{2};tr{3}},[ex(2);ex(3)],[ey(2);ey(3)],[xo(2);xo(3)],[yo(2);yo(3)],[f(2);f(3)]);
        p3=sm_backproj_one(p,tr{1},ex(1),ey(1),xo(1),yo(1),f(1));
        for m=1:k
            tmp=norm(abs(p3-pt{1}(mmm(m,1),:)));
            if tmp<e
                e=tmp;
                mm=[mmm(m,1),mmm(i,2),mmm(j,3),-2];
            end
        end
    end
end
