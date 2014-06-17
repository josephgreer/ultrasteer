function ps_center=sm_centerpoints(p1,p2,I)

ps=sm_pointsofline(p1,p2);
[k,tmp]=size(ps);
th=atan2(p2(1)-p1(1),p2(2)-p1(2));
r=5;
pss1=zeros(k,2);
pss2=zeros(k,2);
ps_center=zeros(k,2);
[imax,jmax]=size(I);

for p=1:k
    pss1(p,:)=round([ps(p,1)+r*sin(th+pi/2),ps(p,2)+r*cos(th+pi/2)]);
    pss2(p,:)=round([ps(p,1)-r*sin(th+pi/2),ps(p,2)-r*cos(th+pi/2)]);
    pss=round(sm_pointsofline(pss1(p,:),pss2(p,:)));
    [kk,tmp]=size(pss);
    tmp=zeros(kk,1);
    for pp=1:kk
        if pss(pp,1)>=1 & pss(pp,1)<=imax & pss(pp,2)>=1 & pss(pp,2)<=jmax
            tmp(pp)=I(pss(pp,1),pss(pp,2));
        else
            tmp(pp)=255;
        end
    end
    [v,ind]=min(tmp);
    ps_center(p,:)=pss(ind,:);
%    plot(ps_center(p,2),ps_center(p,1));
%    plot([pss1(p,2),pss2(p,2)],[pss1(p,1),pss2(p,1)]);
end
