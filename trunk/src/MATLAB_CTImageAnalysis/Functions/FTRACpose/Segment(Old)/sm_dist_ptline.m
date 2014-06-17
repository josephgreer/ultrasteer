function d=sm_dist_ptline(pts,ors,ptc)

a=ors(:,2);
b=-ors(:,1);
c=ors(:,1).*ptc(:,2)-ors(:,2).*ptc(:,1);
d=abs(a.*pts(:,1)+b.*pts(:,2)+c)./sqrt(a.^2+b.^2);
