function Ib_new=sm_removelines2(L,Ib)

Ib_new=Ib;
r=5;
[imax,jmax]=size(Ib);
[N_lines,tmp]=size(L);
for i=1:N_lines
    ps=sm_pointsofline(L(i,1:2),L(i,3:4));
    [k,tmp]=size(ps);
    th=atan2(L(i,3)-L(i,1),L(i,4)-L(i,2));%(p2(1)-p1(1),p2(2)-p1(2));
    pss1=zeros(k,2);
    pss2=zeros(k,2);
	
	c=0;
	for p=1:k
        pss1(p,:)=[ps(p,1)+r*sin(th+pi/2),ps(p,2)+r*cos(th+pi/2)];
        pss2(p,:)=[ps(p,1)-r*sin(th+pi/2),ps(p,2)-r*cos(th+pi/2)];
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
        if N_tmp3==0
            continue;
        elseif N_tmp3==1
            Ib_new(pss(tmp3,1),pss(tmp3,2))=0;
        else    %>=2
            p_tmp3=pss(tmp3,:);
            diff_tmp3=p_tmp3-repmat(ps(p,:),N_tmp3,1);
            d_tmp3=sqrt(diff_tmp3(:,1).^2+diff_tmp3(:,2).^2);
            [v,ind3]=sort(d_tmp3);
            Ib_new(pss(tmp3(ind3(1)),1),pss(tmp3(ind3(1)),2))=0;
            Ib_new(pss(tmp3(ind3(2)),1),pss(tmp3(ind3(2)),2))=0;
        end
	end
end