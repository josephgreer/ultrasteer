function ps=sm_getallcontourpoints(Ib)

[m,n]=size(Ib);
c=0;
pstmp=zeros(m*n,2);
for i=1:m
    for j=1:n
        if Ib(i,j)==1
            c=c+1;
            pstmp(c,:)=[i,j];
        end
    end
end
ps=pstmp(1:c,:);
