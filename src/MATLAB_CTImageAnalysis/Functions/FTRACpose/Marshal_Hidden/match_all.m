function [p,n_c]=match_all(fp,ft,n_im,n_seeds,n_tst)

[c,b]=match_def2(fp,ft,n_im,n_seeds);
%rand('state',100);
for k=1:n_tst
%    k
	f=match_rand_flow(n_seeds);
	[f,n_c(k)]=match_cycle(f,b,c);
	total=0;
	for i=1:2*n_seeds+2
        for j=1:2*n_seeds+2
            if f(i,j)==1
                total=total+c(i,j);
            end
        end
	end
	p=match_recon_gen(fp,ft,n_im,n_seeds,f);
end

%t0=clock;
%[c,b]=match_def2('im25','tr25',[1,2],25);
%f=match_mf_ltf(b);
%rand('state',100);
%for i=1:10
%f=match_rand_flow(25);
%f=match_cycle(f,b,c);
%[s,t,ct,ft]=match_tree(f,c);
%etime(clock,t0)

%total=0;
%for i=1:52
%    for j=1:52 %52,102,16
%        if f(i,j)==1
%            total=total+c(i,j);
%        end
%    end
%end

%t2=0;
%for i=1:25
%    t2=t2+c(i+1,i+1+25)+c(i+1+50,i+1+75);%+c(i+1+36,i+1+45)+c(i+1+54,i+1+63);%%c(i+1,i+1+7)+c(i+1+14,i+1+21);%
%end
%total
%p=match_recon('im25','tr25',[1,2],25,f)
%t2
%end
