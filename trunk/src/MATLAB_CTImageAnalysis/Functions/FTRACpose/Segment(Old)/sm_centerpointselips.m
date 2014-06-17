function ps_center2=sm_centerpointselips(ps,I)

[imax,jmax]=size(I);
[N_p,tmp]=size(ps);
r=5;
ps_center=zeros(N_p,2);
c=0;
for k=1:N_p
    i=ps(k,1);
    j=ps(k,2);
    orient=ps(k,3);
    p1=round([i+r*sin(orient+pi/2),j+r*cos(orient+pi/2)]);
    p2=round([i+r*sin(orient-pi/2),j+r*cos(orient-pi/2)]);
%    line([p1(2),p2(2)],[p1(1),p2(1)]);
    pp=round(sm_pointsofline(p1,p2));
    [N_pp,tmp]=size(pp);
    tmp=zeros(N_pp,1);
    for kk=1:N_pp
        if pp(kk,1)>=1 & pp(kk,1)<=imax & pp(kk,2)>=1 & pp(kk,2)<=jmax
            tmp(kk)=I(pp(kk,1),pp(kk,2));
        else
            tmp(kk)=255;
        end
    end
    [v,ind]=min(tmp);
    pc=pp(ind,:);
    iok=find(ps_center(:,1)==pc(1));
    tag=0;
    if isempty(iok)
        tag=1;
    elseif isempty(find(ps_center(iok,2)==pc(2)))
        tag=1;
    else
        tag=0;
    end
    if tag==1
        c=c+1;
        ps_center(c,:)=pc;
    end
end

ps_center2=ps_center(1:c,:);
%for kk=1:c
%    plot(ps_center2(kk,2),ps_center2(kk,1),'*');
%end
