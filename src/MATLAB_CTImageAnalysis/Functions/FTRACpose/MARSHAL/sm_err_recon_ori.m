function [e_mean,e_std,e_min,e_max,e_all]=sm_err_recon_ori(ors_recon,ors_true)

[n,tmp]=size(ors_recon);
e=zeros(n,1);
for i=1:n
    e(i)=acos(dot(ors_recon(i,:),ors_true(i,:)));
    if e(i)>pi/2
        e(i)=pi-e(i);
    end
end
e_all=e/pi*180;
%Calculate the statistics.
e_mean=mean(e_all);
e_std=std(e_all);
e_min=min(e_all);
e_max=max(e_all);
