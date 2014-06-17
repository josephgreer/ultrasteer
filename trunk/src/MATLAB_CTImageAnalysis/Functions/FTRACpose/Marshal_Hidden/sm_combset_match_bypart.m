function res=sm_combset_match_bypart(ee,k,m,bb)
%SM_COMBSET_MATCH_BYPART find the best set of k combos.
%
%RES=SM_COMBSET_MATCH_BYPART(EE,K,M,BB), where
%EE is the list of accuracy values,
%K is the number of combos in one set,
%M is the number of combos in one part,
%BB is the list of all combos,
%returns the best set of k combos, RES.

N=factorial(k);
M=factorial(m);
A=perms(1:k)';
K=N/M;

kk=zeros(k*M^2,3);
vmin=10000;
res=zeros(k,3);
for i=1:K
    i
    for j=1:K
        [y x] = meshgrid((i-1)*M+1:i*M, (j-1)*M+1:j*M);
        pe = [reshape(x, M^2, 1) reshape(y, M^2, 1)];
        kk = [repmat((1:k)',M^2,1),reshape(A(:,pe(:,1)), k*M*M, 1),reshape(A(:,pe(:,2)), k*M*M, 1)];
		a=(kk(:,1)-1)*k^2+(kk(:,2)-1)*k+kk(:,3);
		d=ee(a);
		dd=reshape(d,k,M^2);
		s=sum(dd);
		[v,c]=min(s);
        if v<vmin
            vmin=v;
            comb=kk((c-1)*k+1:c*k,:);
            aa=(comb(:,1)-1)*k^2+(comb(:,2)-1)*k+comb(:,3);
            res=bb(aa,:);
        end
    end
end
