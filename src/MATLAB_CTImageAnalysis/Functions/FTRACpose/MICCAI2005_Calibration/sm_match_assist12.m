function [mm,e]=sm_match_assist1(mmm,pt,tr,ex,ey,xo,yo,f)

[k,N]=size(mmm);
e=10000000;
mm=[0,0,0,0];
for i=1:k
    for j=1:k
        [p,d]=sm_recon_core2({pt{1}(mmm(i,1),:);pt{2}(mmm(j,2),:)},{tr{1};tr{2}},[ex(1);ex(2)],[ey(1);ey(2)],[xo(1);xo(2)],[yo(1);yo(2)],[f(1);f(2)]);
        p3=sm_backproj_one(p,tr{3},ex(3),ey(3),xo(3),yo(3),f(3));
        for m=1:k
            tmp=norm(abs(p3-pt{3}(mmm(m,3),:)));
            if tmp<e
                e=tmp;
                mm=[mmm(i,1),mmm(j,2),mmm(m,3),-1];
            end
        end
    end
end
