function g2=sm_linesgroup3(L,dis_thresh,ang_thresh,ang_range)
%seperate straight lines into different groups using threshold;

[tmp,ind]=sort(L(:,8));
L1=[L(ind,:),ind];
[NL,tmp]=size(L1);

Ng=1;
g{Ng,1}=L1(1,:);
rmin=L1(1,8);
for i=2:NL
    dr=abs(L1(i,8)-rmin);
    if dr<=dis_thresh
        g{Ng,1}=[g{Ng,1};L1(i,:)];
    else
        Ng=Ng+1;
        g{Ng,1}=L1(i,:);
        rmin=L1(i,8);
    end
end

Ng2=0;
for i=1:Ng
    L2=g{i};
    L2(:,9)=mod(L2(:,9),pi);
    if max(L2(:,9))-min(L2(:,9))>ang_range*4
        L2(:,9)=L2(:,9)-round(L2(:,9)/pi)*pi;
    end
    %ind=find(L2(:,9)>pi/2);
    %if ~isempty(ind)
    %    L2(ind,9)=L2(ind,9)-pi;
    %end
    [tmp,ind]=sort(L2(:,9));
    L3=L2(ind,:);
    [NL,tmp]=size(L3);
    
    Ng2=Ng2+1;
    g2{Ng2,1}=L3(1,:);
    thmin=L3(1,9);
    for k=2:NL
        dth=abs(L3(k,9)-thmin);
        if dth<=ang_thresh
            g2{Ng2,1}=[g2{Ng2,1};L3(k,:)];
        else
            Ng2=Ng2+1;
            g2{Ng2,1}=L3(k,:);
            thmin=L3(k,9);
        end
    end
end
