s_fid3=zeros(3,1);
pp_fid3=cell(3,1);
intense_fid3=cell(3,1);
m=zeros(3,1);
for i=1:3
    pp_fid3{i}=round(sm_centerpoints4(Ls_fid3(i,7:8),Ls_fid3(i,9:10),I3));
    [m(i),tmp]=size(pp_fid3{i});
    intense_fid3{i}=zeros(m(i),1);
    for j=1:m(i)
        intense_fid3{i}(j,1)=Es(pp_fid3{i}(j,1),pp_fid3{i}(j,2));
    end
%    mn_fid3=mean(intense_fid3{i});
%    thresh_fid3=mn_fid3-0.2*(mn_fid3-min(intense_fid3{i}));
%    for j=1:m
%        if intense_fid3{i}(j)<thresh_fid3
%            s_fid3(i)=s_fid3(i)+intense_fid3{i}(j);
%        end
%    end
end
N=min(m);
for i=1:3
    tmp=sort(intense_fid3{i});
    s_fid3(i)=sum(tmp(1:N));
end