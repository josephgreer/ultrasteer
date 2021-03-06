%t0=clock;
ns_im1=[1,2,3;...
        1,2,4;...
        1,2,5;...
        1,2,6;...
        1,3,4;...
        1,3,5;...
        1,3,6;...
        1,4,5;...
        1,4,6;...
        1,5,6;...
        2,3,4;...
        2,3,5;...
        2,3,6;...
        2,4,5;...
        2,4,6;...
        2,5,6;...
        3,4,5;...
        3,4,6;...
        3,5,6;...
        4,5,6];
%ns_im=sm_n31(6);
%ns_im=sm_n21(6);
res_all=[];
rr=[];
ra=[];
ram=[];
rau=[];
bpe=[];
bpem=[];
bpeu=[];
re=[];
rem=[];
reu=[];
ren=[];
renm=[];
renu=[];
eo=[];
eom=[];
eou=[];
tt=[];
N=20;
for k=1:N
%    k=19
%    k=1
    t0=clock;
    [pts_3d,r,res_ra,res_ra_m,res_ra_u,res_bpe,res_bpe_m,res_bpe_u,res_re,res_re_m,res_re_u,res_ren,res_ren_m,res_ren_u,res_eo,res_eo_m,res_eo_u]=sm_main_seeds_orientation('Dewarped\Dataset_4\',k);%2m,2m1,..,3m1,3m2,3m3
    t=etime(clock,t0)
    res=[k,0,0,r;...%ns_im(k,:),
        res_ra;...
        res_ra_m;...
        res_ra_u;...
        res_bpe;...
        res_bpe_m;...
        res_bpe_u;...
        res_re;...
        res_re_m;...
        res_re_u;...
        res_ren;...
        res_ren_m;...
        res_ren_u;...
        res_eo;...
        res_eo_m;...
        res_eo_u]
%    res_all=[res_all;res];
tt=[tt;t];
    rr=[rr;r];
	ra=[ra;res_ra];
	ram=[ram;res_ra_m];
%	ram=[ram;res_ra_m*r];
	rau=[rau;res_ra_u];
%	rau=[rau;res_ra_u*(1-r)];
	bpe=[bpe;res_bpe];
	bpem=[bpem;res_bpe_m];
%	bpem=[bpem;res_bpe_m*r];
	bpeu=[bpeu;res_bpe_u];
%	bpeu=[bpeu;res_bpe_u*(1-r)];
	re=[re;res_re];
	rem=[rem;res_re_m];
%	rem=[rem;res_re_m*r];
	reu=[reu;res_re_u];
%	reu=[reu;res_re_u*(1-r)];
	ren=[ren;res_ren];
	renm=[renm;res_ren_m];
%	renm=[renm;res_ren_m*r];
	renu=[renu;res_ren_u];
%	renu=[renu;res_ren_u*(1-r)];
eo=[eo;res_eo];
eom=[eom;res_eo_m];
eou=[eou;res_eo_u];
end
mr=mean(rr);
mra=mean(ra);
rr4=repmat(rr,1,4);
mram=sum(ram.*rr4)/sum(rr);
mrau=sum(ram.*(1-rr4))/sum(1-rr);
mbpe=mean(bpe);
mbpem=sum(bpem.*rr4)/sum(rr);
mbpeu=sum(bpeu.*(1-rr4))/sum(1-rr);
mre=mean(re);
mrem=sum(rem.*rr4)/sum(rr);
mreu=sum(reu.*(1-rr4))/sum(1-rr);
mren=mean(ren);
mrenm=sum(renm.*rr4)/sum(rr);
mrenu=sum(renu.*(1-rr4))/sum(1-rr);
meo=mean(eo);
meom=sum(eom.*rr4)/sum(rr);
meou=sum(eou.*(1-rr4))/sum(1-rr);


mean_all=[[tt;mean(tt);0],[rr;mean(rr);0],[ra(:,1);mra(1);0],[ram(:,1);mram(1);0],[rau(:,1);mrau(1);0],...
        [bpe(:,1);mbpe(1);0],[bpem(:,1);mbpem(1);0],[bpeu(:,1);mbpeu(1);0],...
        [re(:,1);mre(1);0],[rem(:,1);mrem(1);0],[reu(:,1);mreu(1);0],...
        [ren(:,1);mren(1);0],[renm(:,1);mrenm(1);0],[renu(:,1);mrenu(1);0],...
        [eo(:,1);meo(1);0],[eom(:,1);meom(1);0],[eou(:,1);meou(1);0]];
std_all=[[tt;std(tt);0],[rr;std(rr);0],[ra(:,2);mra(2);0],[ram(:,2);mram(2);0],[rau(:,2);mrau(2);0],...
        [bpe(:,2);mbpe(2);0],[bpem(:,2);mbpem(2);0],[bpeu(:,2);mbpeu(2);0],...
        [re(:,2);mre(2);0],[rem(:,2);mrem(2);0],[reu(:,2);mreu(2);0],...
        [ren(:,2);mren(2);0],[renm(:,2);mrenm(2);0],[renu(:,2);mrenu(2);0],...
        [eo(:,2);meo(2);0],[eom(:,2);meom(2);0],[eou(:,2);meou(2);0]];
min_all=[[tt;min(tt);0],[rr;min(rr);0],[ra(:,3);mra(3);min(ra(:,3))],[ram(:,3);mram(3);min(ram(:,3))],[rau(:,3);mrau(3);min(rau(:,3))],...
        [bpe(:,3);mbpe(3);min(bpe(:,3))],[bpem(:,3);mbpem(3);min(bpem(:,3))],[bpeu(:,3);mbpeu(3);min(bpeu(:,3))],...
        [re(:,3);mre(3);min(re(:,3))],[rem(:,3);mrem(3);min(rem(:,3))],[reu(:,3);mreu(3);min(reu(:,3))],...
        [ren(:,3);mren(3);min(ren(:,3))],[renm(:,3);mrenm(3);min(renm(:,3))],[renu(:,3);mrenu(3);min(renu(:,3))],...
        [eo(:,3);meo(3);0],[eom(:,3);meom(3);0],[eou(:,3);meou(3);0]];
max_all=[[tt;max(tt);0],[rr;max(rr);0],[ra(:,4);mra(4);max(ra(:,4))],[ram(:,4);mram(4);max(ram(:,4))],[rau(:,4);mrau(4);max(rau(:,4))],...
        [bpe(:,4);mbpe(4);max(bpe(:,4))],[bpem(:,4);mbpem(4);max(bpem(:,4))],[bpeu(:,4);mbpeu(4);max(bpeu(:,4))],...
        [re(:,4);mre(4);max(re(:,4))],[rem(:,4);mrem(4);max(rem(:,4))],[reu(:,4);mreu(4);max(reu(:,4))],...
        [ren(:,4);mren(4);max(ren(:,4))],[renm(:,4);mrenm(4);max(renm(:,4))],[renu(:,4);mrenu(4);max(renu(:,4))],...
        [eo(:,4);meo(4);0],[eom(:,4);meom(4);0],[eou(:,4);meou(4);0]];
res_all=[mean_all,std_all,min_all,max_all];
%etime(clock,t0)
%res_all=[res_all;...
%        0,0,0,sum(rr)/N;...
%        sum(ra)/N;...
%        sum(ram)/sum(rr);...
%        sum(rau)/sum(1-rr);...
%        sum(bpe)/N;...
%        sum(bpem)/sum(rr);...
%        sum(bpeu)/sum(1-rr);...
%        sum(re)/N;...
%        sum(rem)/sum(rr);...
%        sum(reu)/sum(1-rr);...
%        sum(ren)/N;...
%        sum(renm)/sum(rr);...
%        sum(renu)/sum(1-rr)];
