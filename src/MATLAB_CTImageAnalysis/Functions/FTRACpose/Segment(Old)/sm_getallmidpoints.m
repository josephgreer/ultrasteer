function Ib_new=sm_getallmidpoints(ps,Ib)

dmin=10;
dmax=200;
[m,n]=size(Ib);
Ib_new=zeros(m,n);

[N_p,tmp]=size(ps);
for i=1:N_p
    for j=i+1:N_p
        d=norm(ps(j,:)-ps(i,:));
        if d>=dmin & d<=dmax
            Ib_new(round((ps(i,1)+ps(j,1))/2),round((ps(i,2)+ps(j,2))/2))=1;
        end
    end
end