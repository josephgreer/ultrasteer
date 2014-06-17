function res=sm_combset_match(ee,k,bb)
%SM_COMBSET_MATCH find the best set of k combos.
%
%RES=SM_COMBSET_MATCH(EE,K,BB), where
%EE is the list of accuracy values,
%K is the number of combos in one set,
%BB is the list of all combos,
%returns the best set of k combos.

kk=sm_combset_all(k);
a=(kk(:,1)-1)*k^2+(kk(:,2)-1)*k+kk(:,3);
d=ee(a);
dd=reshape(d,k,factorial(k)^2);
s=sum(dd);
[v,c]=min(s);
comb=kk((c-1)*k+1:c*k,:);
aa=(comb(:,1)-1)*k^2+(comb(:,2)-1)*k+comb(:,3);
res=bb(aa,:);
costs = sum(ee(aa,:));