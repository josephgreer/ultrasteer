function Lf=sm_deslinesfind(Lt,Ies)
%find the 3 fiducial lines;
[N_trend,tmp]=size(Lt);
if N_trend<=3
    Lf=Lt;
else
	s=zeros(N_trend,1);
	pp=cell(N_trend,1);
	intense=cell(N_trend,1);
	m=zeros(N_trend,1);
	for i=1:N_trend
        pp{i}=round(sm_pointsofline(Lt(i,4:5),Lt(i,6:7)));
        [m(i),tmp]=size(pp{i});
        intense{i}=zeros(m(i),1);
        for j=1:m(i)
            intense{i}(j,1)=Ies(pp{i}(j,1),pp{i}(j,2));
        end
	end
	N=min(m);
	for i=1:N_trend
        tmp=sort(intense{i});
        s(i)=sum(tmp(m(i)-N+1:m(i)));
	end
	[tmp,ind]=sort(s);
	Lf=Lt(ind(N_trend-2:N_trend),:);
end
