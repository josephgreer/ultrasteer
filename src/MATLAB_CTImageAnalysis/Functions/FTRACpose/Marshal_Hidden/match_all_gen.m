function [p,fout]=match_all_gen(fpt,ftr,ns_im,n_seeds)

% add data directory
fpt = strcat('Dewarped\Dataset_5\', fpt);
ftr = strcat('Dewarped\Dataset_5\', ftr);

%mis
N=length(ns_im);
if N==2
    nn=[1,2;2,1];
end
if N==3
    nn=[1,2;...
        2,3;...
        3,1];
end

fout = [];
N
for i=1:N-1
    i
%    [c,b]=match_def2(fpt,ftr,[ns_im(i),ns_im(i+1)],n_seeds);
    [c,b]=match_def2(fpt,ftr,[ns_im(nn(i,1)),ns_im(nn(i,2))],n_seeds);
    rand('state',100);
    f=match_rand_flow(n_seeds);
    [f,n_c]=match_cycle(f,b,c);
    fout(i,:)=match_flow2_proc(f,n_seeds);
end
%mis=0;
%for j=1:n_seeds
%    for i=1:Nopen
%        if fout(i,j)~=j
%            mis=mis+1;
%            break;
%        end
%    end
%end

p=match_recon_gen_new(fpt,ftr,ns_im,n_seeds,fout(1:N-1,:));
