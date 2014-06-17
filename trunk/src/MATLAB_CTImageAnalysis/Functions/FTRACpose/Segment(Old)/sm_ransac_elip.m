function e=sm_ransac_elip(ps)

N_trial=20;
[N_all,tmp]=size(ps);
N_samp=round(N_all/3);
rand('state',100);
dd=zeros(N_trial,1);
es=zeros(N_trial,12);
for i=1:N_trial
    ind=rand(N_samp,1);
    ind=round(ind*(N_all-1))+1;
    ind=sort(ind);
    samp=ps(ind,:);
    [abcdef(i,:),k,cp,a,b,th]=fit_ellipse(samp);
    es(i,:)=[abcdef(i,:),k,fliplr(cp'),th,b,a];
 %   a=abcdef(i,1);
 %   b=abcdef(i,2);
 %   c=abcdef(i,3);
 %   d=abcdef(i,4);
 %   e=abcdef(i,5);
 %   f=abcdef(i,6);
    for j=1:N_all
        x=ps(j,1);
        y=ps(j,2);
        dd(i)=dd(i)+sm_dist_pointtoellipse(x,y,es(i,8),es(i,9),es(i,10),es(i,11),es(i,12));
%        dd(i)=dd(i)+abs((a*x^2+b*x*y+c*y^2+d*x+e*y+f)*k);
    end
end
[v,id]=min(dd);
e=es(id,:);
