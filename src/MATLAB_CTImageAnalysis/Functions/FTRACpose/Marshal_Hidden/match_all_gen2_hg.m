function [p,fout]=match_all_gen2_hg(fpt,ftr,ns_im,n_seeds)

N=length(ns_im);
k=0;
for i=1:N-1
    for j=i+1:N
        k=k+1;
        cmb(k,:)=[ns_im(i),ns_im(j)];
    end
end

p=zeros(3,n_seeds);
for i=1:k
    i
    %Hungarian for one combo to generate the flow of one combo;
    f=match_hungarian(fpt,ftr,cmb(i,:));
    %make a flow list for all the combos;
    fout(i,:)=f;
    p=p+match_recon_gen_new(fpt,ftr,cmb(i,:),n_seeds,f);
end
p=p/k;
