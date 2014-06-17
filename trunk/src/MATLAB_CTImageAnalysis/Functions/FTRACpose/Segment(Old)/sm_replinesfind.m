function g_rep=sm_replinesfind(g)
%find two representative for each group;

[N_g,tmp]=size(g);
g_rep=cell(N_g,1);
for k=1:N_g
    tmp1=g{k};
    [n,tmp]=size(tmp1);
    if n<=2
        g_rep{k}=tmp1;
    else
        tmp2=tmp1(:,7);
        [v,ind]=sort(tmp2);
        g_rep{k}=tmp1(ind(n-1:n),:);
    end
end

