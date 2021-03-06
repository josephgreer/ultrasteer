function match_errtst_set(fdir,fpt,ftr,fpg)
%find all the matching triplets;

t0=clock;
ns_im=[1,2,3;...
        1,2,4;...
        1,2,5;...
        1,3,4;...
        1,3,5;...
        1,4,5;...
        2,3,4;...
        2,3,5;...
        2,4,5;...
        3,4,5];
deg=[-30,-15,0,15,30];
pg=load(strcat(fdir,fpg))';
[tmp,n_seeds]=size(pg);

%for i=1:10
    i=3
%    i=1
    [res0,res00,t_tab,res,res2]=match_errtst(strcat(fdir,fpt),strcat(fdir,ftr),ns_im(i,:),n_seeds,pg);
%    head=[n_set,n_seeds,deg(ns_im(i,:))];
    head=[n_seeds,deg(ns_im(i,:))];
    save(strcat(fdir,'res_combo_',int2str(i),'.txt'),'head','res0','res00','t_tab','res','res2','-ASCII');
    %end
etime(clock,t0)