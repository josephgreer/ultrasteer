figure;
imshow(I4);
axis on;
hold on;
[N_c,tmp]=size(centers);
e=[];
for p=1:N_c
    p
    e=[e;sm_houghaxislength(centers(p,:),ps)]
end
%N_elip=20;
%[v,ind]=sort(e(:,9));
%[N,tmp]=size(v);
%elips=e(ind(N-N_elip+1:N),:);
elips=e;
[N_elip,tmp]=size(elips);
for k=1:N_elip
    sm_drwelips(elips(k,2),elips(k,1),elips(k,4),elips(k,7),elips(k,8));
end
