function ns_im=sm_n33(n)

r=0;
for i=1:n
    for j=i+1:n
        for k=j+1:n
            tmp=[];
            a=0;
            for p=1:n
                if isempty(find([i,j,k]==p))
                    a=a+1;
                    tmp(a,1)=p;
                end
            end
            for b=1:n-3
                for c=b+1:n-3
                    for d=c+1:n-3
                        r=r+1;
                        ns_im(r,:)=[i,j,k,tmp(b),tmp(c),tmp(d)];
                    end
                end
            end
        end
    end
end