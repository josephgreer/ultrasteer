function ns_im=sm_n31(n)

r=0;
for i=1:n
    for j=i+1:n
        for k=j+1:n
            for p=1:n
                if isempty(find([i,j,k]==p))
                    r=r+1;
                    ns_im(r,:)=[i,j,k,p];
                end
            end
        end
    end
end