function Lf=sm_deslinesfind2(Lt,Ig,Ies)
%find the 3 fiducial lines;
r=5;
[imax,jmax]=size(Ies);

[N_trend,tmp]=size(Lt);
if N_trend<=3
    Lf=Lt;
else
	c=zeros(N_trend,1);
	its=cell(N_trend,1);
%    es=cell(N_trend,1);
	for i=1:N_trend
        p1=Lt(i,4:5);
        p2=Lt(i,6:7);
        th=atan2(p2(1)-p1(1),p2(2)-p1(2));

        ps=sm_pointsofline(p1,p2);
        [m,tmp]=size(ps);
        its{i}=[];
        es{i}=[];
        for p=1:m
            pss1=[ps(p,1)+r*sin(th+pi/2),ps(p,2)+r*cos(th+pi/2)];
            pss2=[ps(p,1)-r*sin(th+pi/2),ps(p,2)-r*cos(th+pi/2)];
            pss=round(sm_pointsofline(pss1,pss2));
            [kk,tmp]=size(pss);
            tmp1=zeros(kk,1);
            for pp=1:kk
                if pss(pp,1)>=1 & pss(pp,1)<=imax & pss(pp,2)>=1 & pss(pp,2)<=jmax
                    tmp1(pp)=Ies(pss(pp,1),pss(pp,2));
                else
                    tmp1(pp)=0;
                end
            end
            gradmean=mean(tmp1);
            grad_thresh=(max(tmp1)-mean(tmp1))*0.2+mean(tmp1);
            ind1=find(tmp1<gradmean);
            tmp2=pss(ind1,:);
            tmp3=round(ps(p,:));
            ind2=find(tmp2(:,1)==tmp3(1));
            if ~isempty(ind2)
                ind3=find(tmp2(ind2,2)==tmp3(2));
                if ~isempty(ind3);
                    ind=ind1(ind2(ind3));
                    if ~isempty(find(tmp1(1:ind)>=grad_thresh)) & ~isempty(find(tmp1(ind:length(tmp1))>=grad_thresh)) 
                        c(i)=c(i)+1;
                        its{i}(c(i))=Ig(tmp3(1),tmp3(2));
%                        es{i}(c(i))=Ies(tmp3(1),tmp3(2));
                    end
                end
            end
        end
	end
	N=min(c);
    s1=zeros(N_trend,1);
%    s2=zeros(N_trend,1);
	for i=1:N_trend
        tmp1=sort(its{i});
        s1(i)=sum(tmp1(1:N));
%        tmp2=sort(es{i});
%        s2(i)=sum(tmp2(1:N));
	end
    s1
	[tmp,ind]=sort(s1);
	Lf=Lt(ind(1:3),:);
end
