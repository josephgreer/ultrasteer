function ns_im=sm_n21(n)

r=0;
for i=1:n
    for j=i+1:n
        for p=1:n
            if isempty(find([i,j]==p))
                r=r+1;
                ns_im(r,:)=[i,j,p];
            end
        end
    end
end