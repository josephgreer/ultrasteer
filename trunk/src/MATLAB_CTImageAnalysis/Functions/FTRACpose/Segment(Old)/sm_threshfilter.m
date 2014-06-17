function Iout=sm_threshfilter(Iin)

%vmin=min(min(Iin));
%tmp=Iin-vmin;
%vthreshold=vmin+mean(mean(tmp))*2.5;
w=5;
[m,n]=size(Iin);
Iout=zeros(m,n);
for i=1:m
    for j=1:n
        tmp=Iin(i-w:i+w,j-w:j+w);
        vthreshold=mean(tmp);
        if Iin(i,j)>vthreshold
            Iout(i,j)=Iin(i,j);
        end
    end
end
