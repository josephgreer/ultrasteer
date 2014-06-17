function ps_center=sm_centerpoints6(p1,p2,I,Ib)

ps=sm_pointsofline(p1,p2);
[k,tmp]=size(ps);
th=atan2(p2(1)-p1(1),p2(2)-p1(2));
r=5;
pss1=zeros(k,2);
pss2=zeros(k,2);
[imax,jmax]=size(I);

c=0;
for p=1:k
    pss1(p,:)=[ps(p,1)+r*sin(th+pi/2),ps(p,2)+r*cos(th+pi/2)];
    pss2(p,:)=[ps(p,1)-r*sin(th+pi/2),ps(p,2)-r*cos(th+pi/2)];
    pss=round(sm_pointsofline(pss1(p,:),pss2(p,:)));
    [kk,tmp]=size(pss);
    tmp1=zeros(kk,1);
    tmp2=zeros(kk,1);
    for pp=1:kk
        if pss(pp,1)>=1 & pss(pp,1)<=imax & pss(pp,2)>=1 & pss(pp,2)<=jmax
            tmp1(pp)=I(pss(pp,1),pss(pp,2));
            tmp2(pp)=Ib(pss(pp,1),pss(pp,2));
        else
            tmp1(pp)=255;
            tmp2(pp)=0;
        end
    end
    tmp3=find(tmp2==1);
    N_tmp3=length(tmp3);
    if N_tmp3==1
        [v,ind1]=min(tmp1);
        c=c+1;
        ps_center(c,:)=pss(ind1,:);
    else
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
            tmp4=tmp1(tmp3(a):tmp3(b));
            [v,ind4]=min(tmp4);
            c=c+1;
            ps_center(c,:)=pss(tmp3(a)+ind4-1,:);
        end
    end
end
%for i=1:c
%    plot(ps_center(i,2),ps_center(i,1),'*');
%end