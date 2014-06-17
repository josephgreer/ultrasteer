function kk=sm_combset_all(m)
%SM_COMBSET_ALL generates all the m-triplet combo set.
%
%KK=SM_COMBSET_ALL(M), where
%M denotes the number of combos in a set,
%returns the list of all combo sets.

N=factorial(m);
kk=zeros(m*N^2,3);

co23=perms(1:m);

kk(:,1)=repmat((1:m)',N^2,1);
bb=repmat(co23',N,1);
kk(:,2)=reshape(bb,N^2*m,1);
cc=reshape(co23',N*m,1);
kk(:,3)=repmat(cc,N,1);
