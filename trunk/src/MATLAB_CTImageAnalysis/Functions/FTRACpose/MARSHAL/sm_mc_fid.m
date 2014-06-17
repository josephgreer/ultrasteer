%SM_MC_Fid is a batch program of processing all the fiducial data sets.
%The results for each data set is stored in a file res*.txt in the data set directory.

cc=[1];
n_sets=length(cc);
n_comb=1;
for i=1:n_sets
    res_ra=zeros(n_comb,4);
    res_re=zeros(n_comb,4);
    res_bpe=zeros(n_comb,4);
    for j=1:n_comb
        j
        [res_ra(j,:),res_re(j,:),res_bpe(j,:)]=sm_main_fid5(strcat('fiducial',int2str(cc(i)),'\'),j);
    end
    tmp=[res_ra,res_re,res_bpe];
    save(strcat('fiducial',int2str(cc(i)),'\res5.txt'),'tmp','-ASCII');
end