function s=delreppt(ss)

[a,b]=size(ss);

k=1;
for i=1:a-1
    if ss(i,1)==0
        continue;
    end
    s(k,1)=ss(i,1);
    s(k,2)=ss(i,2);
    k=k+1;
    for j=i+1:a
        if ss(j,1)==ss(i,1) & ss(j,2)==ss(i,2)
            ss(j,1)=0;
            ss(j,2)=0;
        end
    end
end
if ss(a,1)~=0
    s(k,1)=ss(a,1);
    s(k,2)=ss(a,2);
end
