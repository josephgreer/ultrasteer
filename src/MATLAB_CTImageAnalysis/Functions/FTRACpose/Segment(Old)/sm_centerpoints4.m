function ps_center=sm_centerpoints4(p1,p2,Ib)

ps=sm_pointsofline(p1,p2);
[k,tmp]=size(ps);
th=atan2(p2(1)-p1(1),p2(2)-p1(2));
r=5;
pss1=zeros(k,2);
pss2=zeros(k,2);
[imax,jmax]=size(Ib);

c=0;
for p=1:k
    pss1(p,:)=round([ps(p,1)+r*sin(th+pi/2),ps(p,2)+r*cos(th+pi/2)]);
    pss2(p,:)=round([ps(p,1)-r*sin(th+pi/2),ps(p,2)-r*cos(th+pi/2)]);
    pss=round(sm_pointsofline(pss1(p,:),pss2(p,:)));
    [kk,tmp]=size(pss);
    tmp=zeros(kk,1);
    for pp=1:kk
        if pss(pp,1)>=1 & pss(pp,1)<=imax & pss(pp,2)>=1 & pss(pp,2)<=jmax
            tmp(pp)=Ib(pss(pp,1),pss(pp,2));
        else
            tmp(pp)=0;
        end
    end
    tmp3=find(tmp==1);
    N_tmp3=length(tmp3);
    if N_tmp3>=2
        p_tmp3=pss(tmp3,:);
        diff_tmp3=p_tmp3-repmat(ps(p,:),N_tmp3,1);
        d_tmp3=sqrt(diff_tmp3(:,1).^2+diff_tmp3(:,2).^2);
        [v,ind3]=sort(d_tmp3);
        a=ind3(1);
        b=ind3(2);
        if a>b
            a=ind3(2);
            b=ind3(1);
        end
        dd_tmp3=norm(pss(tmp3(a),:)-pss(tmp3(b),:));
        if dd_tmp3>=d_tmp3(a) & dd_tmp3>=d_tmp3(b)
            c=c+1;
            ps_center(c,:)=ps(p,:);
            plot(ps_center(c,2),ps_center(c,1),'*');
        end
    end
end
