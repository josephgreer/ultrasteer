%sm_seg
%1. get the whole image;
I0 = imread('images1\IM_00005.jpeg');
%I0=imread('test.jpg');
%I0=rgb2gray(I0);
figure;
imshow(I0);
axis on;
hold on;

%2. get the area of interest;
[x,y]=ginput(2);
line([x(1),x(1),x(2),x(2),x(1)],[y(1),y(2),y(2),y(1),y(1)]);
I1=I0(y(1):y(2),x(1):x(2));
%figure;
%imshow(I1);
%axis on;
%hold on;

%3. gauss filter;
I15=sm_gaussfilter(I0);
I2=I15(y(1):y(2),x(1):x(2));
%I2=medfilt2(I1);
%[I2,noise]=wiener2(I2);
%figure;
%imshow(I2);
%axis on;
%hold on;

%4. edge detection;
I3=edge(I2,'canny');
figure;
imshow(I3);
axis on;
hold on;

%5. radon transform;
N_top=10;
dth=1;
N_th=180/dth;
theta=0:dth:180;
R=radon(I3,theta(1:N_th));
ss=zeros(N_th,1);
for k=1:180
    tmp=flipud(sort(R(:,k)));
    ss(k)=sum(tmp(1:N_top));
end
[v,ind]=max(ss);
angle=(ind-1)/180*pi;

disp('Starting to search for straight lines. Wait...');
%6. hough lines;
res=sm_houghline3(I3,angle);
[imax,jmax]=size(I3);
[N_lines,tmp]=size(res);
L=zeros(N_lines,4);
for k=1:N_lines
    [L(k,1:2),L(k,3:4)]=sm_drawline(imax,jmax,res(k,1),res(k,2));
    [L(k,5),L(k,6)]=sm_rthofline(L(k,1:2),L(k,3:4));
end

%7. find the centerline from each line;
ps=cell(N_lines,1);
Ls=zeros(N_lines,3);
Ls_center=zeros(N_lines,4);
Ls_rth=zeros(N_lines,2);
for k=1:N_lines
%    k
    ps{k}=sm_centerpoints2(L(k,1:2),L(k,3:4),I2,I3);
    [m,tmp]=size(ps{k});
%    for p=1:m
%        plot(ps{k}(p,2),ps{k}(p,1),'*');
%    end
    Ls(k,:)=sm_ransac_line(ps{k});
    [Ls_center(k,1:2),Ls_center(k,3:4)]=sm_drawline2(imax,jmax,Ls(k,1),Ls(k,2),Ls(k,3));
    [Ls_rth(k,1),Ls_rth(k,2)]=sm_rthofline(Ls_center(k,1:2),Ls_center(k,3:4));
end

%8. group lines;
N_g=0;
dr_threshold=8;%3
dth_threshold=pi/180*5;
for k=1:N_lines
    if N_g==0
        N_g=1;
        g{1,1}(1,:)=[Ls_rth(k,:),k];
    else
        tag=0;
        for i=1:N_g
            [m,tmp]=size(g{i});
            for j=1:m
                dr=abs(Ls_rth(k,1)-g{i}(j,1));
                dth1=abs(mod(Ls_rth(k,2)-g{i}(j,2),pi));
                dth2=abs(mod(g{i}(j,2)-Ls_rth(k,2),pi));
                dth=min(dth1,dth2);
                if dr<dr_threshold & dth<dth_threshold
                    p=m+1;
                    g{i}(p,:)=[Ls_rth(k,:),k];
                    tag=1;
                    break;
                end
            end
            if tag==1
                break;
            end
        end
        if tag==0
            N_g=N_g+1;
            g{N_g,1}(1,:)=[Ls_rth(k,:),k];
        end
    end
end

%9. remove repeated line;
%figure;
%imshow(I3);
%hold on;
%axis on;
ps_all=cell(N_g,1);
Ls_fin=zeros(N_g,10);
for i=1:N_g
    [m,tmp]=size(g{i});
    ps_all{i}=ps{g{i}(1,3)};
    for j=2:m
        ps_all{i}=sm_pscat(ps_all{i},ps{g{i}(j,3)});
    end
    [N_p,tmp]=size(ps_all{i});
%    for p=1:N_p
%        plot(ps_all{i}(p,2),ps_all{i}(p,1),'*');
%    end
    d=zeros(m,1);
    for k=1:m
        kk=g{i}(k,3);
        for p=1:N_p
            d(k)=d(k)+sm_dist_pointtoline(ps_all{i}(p,1),ps_all{i}(p,2),Ls(kk,1),Ls(kk,2),Ls(kk,3));
        end
    end
    [v,ind]=min(d);
    Ls_fin(i,1)=g{i}(ind,3);
    Ls_fin(i,2:4)=Ls(g{i}(ind,3),:);
    Ls_fin(i,5:6)=Ls_rth(g{i}(ind,3),:);
    Ls_fin(i,7:10)=Ls_center(g{i}(ind,3),:);
%    line([Ls_fin(i,8),Ls_fin(i,10)],[Ls_fin(i,7),Ls_fin(i,9)]);
end

%10. find the general trend;
ths=Ls_fin(:,6);
cc=zeros(N_g,1);
for i=1:N_g
    tmp=ths-ths(i);
    for j=1:N_g
        dth1=abs(mod(tmp(j),pi));
        dth2=abs(mod(-tmp(j),pi));
        dth=min(dth1,dth2);
        if dth<dth_threshold
            cc(i)=cc(i)+1;
        end
    end
end
[v,ind]=max(cc);
th_trend=Ls_fin(ind,6);
th_trendthreshold=pi/180*45;
k=0;
tmp=ths-th_trend;
for j=1:N_g
    dth1=abs(mod(tmp(j),pi));
    dth2=abs(mod(-tmp(j),pi));
    dth=min(dth1,dth2);
    if dth<th_trendthreshold
        k=k+1;
        Ls_intrend(k,:)=Ls_fin(j,:);
%        line([Ls_intrend(k,8),Ls_intrend(k,10)],[Ls_intrend(k,7),Ls_intrend(k,9)]);
    end
end

%11. find the 3 fiducial lines;
N_trend=k;
if N_trend<=3
    Ls_fid3=Ls_intrend;
else
	%edge enhancer;
	[Es0,Eo]=sm_enhancer2(I15);
	Es=Es0(y(1):y(2),x(1):x(2));
	%figure;
	%imshow(mat2gray(Es));
	%axis on;
	%hold on;
	
	s=zeros(N_trend,1);
	pp=cell(N_trend,1);
	intense=cell(N_trend,1);
	m=zeros(N_trend,1);
	for i=1:N_trend
	%    i
        pp{i}=round(sm_pointsofline(Ls_intrend(i,7:8),Ls_intrend(i,9:10)));
        [m(i),tmp]=size(pp{i});
        intense{i}=zeros(m(i),1);
        for j=1:m(i)
            intense{i}(j,1)=Es(pp{i}(j,1),pp{i}(j,2));
        end
	end
	N=min(m);
	for i=1:N_trend
        tmp=sort(intense{i});
        s(i)=sum(tmp(m(i)-N+1:m(i)));
	end
	[tmp,ind]=sort(s);
	Ls_fid3=Ls_intrend(ind(N_trend-2:N_trend),:);
end
for i=1:3
    line([Ls_fid3(i,8),Ls_fid3(i,10)],[Ls_fid3(i,7),Ls_fid3(i,9)]);
end
%12. remove major straight lines, prepare for elipse detection;
%I4=sm_removelines(L,I3);
I4=sm_removelines2(Ls_fin(:,7:10),I3);
%figure;
%imshow(I4);
%hold on;
%axis on;

disp('Start to search for ellipses. Wait...');
%13. find symmetric axes;
res=sm_houghsymaxis(I4);
[N_lines,tmp]=size(res);
%L=zeros(N_lines,4);
%for k=1:N_lines
%    [L(k,1:2),L(k,3:4)]=sm_drawline(imax,jmax,res(k,1),res(k,2));
%    line([L(k,2),L(k,4)],[L(k,1),L(k,3)]);
%end

%14. find centers of elipses;
%figure;
%imshow(I4);
%axis on;
%hold on;
c=0;
for i=1:N_lines
    [p1,p2]=sm_drawline(imax,jmax,res(i,1),res(i,2));
    Li=least_squares_line([p1;p2]);
    for j=i+1:N_lines
        dth=mod(res(j,2)-res(i,2),2*pi);
        if abs(dth-pi/2)<1.0e-5 | abs(dth-pi*3/2)<1.0e-5
            c=c+1;
            [p3,p4]=sm_drawline(imax,jmax,res(j,1),res(j,2));
            Lj=least_squares_line([p3;p4]);
            p5=inv([Li(1),Li(2);Lj(1),Lj(2)])*[-Li(3);-Lj(3)];
            centers(c,:)=[p5(1),p5(2),res(i,1),res(i,2),res(j,1),res(j,2)];
%            plot(p5(2),p5(1),'*');
%            line([p1(2),p2(2)],[p1(1),p2(1)]);
%            line([p3(2),p4(2)],[p3(1),p4(1)]);
        end
    end    
end

%15. find elipses;
ps=sm_getallcontourpoints(I4);
%figure;
%imshow(I4);
%axis on;
%hold on;
[N_c,tmp]=size(centers);
e=[];
for p=1:N_c
    p
    e=[e;sm_houghaxislength(centers(p,:),ps)];
end
%N_elip=20;
%[v,ind]=sort(e(:,9));
%[N,tmp]=size(v);
%elips=e(ind(N-N_elip+1:N),:);
elips=e;
[N_elip,tmp]=size(elips);
%for k=1:N_elip
%    sm_drwelips(elips(k,2),elips(k,1),elips(k,4),elips(k,7),elips(k,8));
%end

%figure;
%imshow(I4);
%axis on;
%hold on;

ee=zeros(N_elip,5);
for k=1:N_elip
    if elips(k,7)<=elips(k,8)
        ee(k,:)=[elips(k,2),elips(k,1),elips(k,4),elips(k,7),elips(k,8)];
    else
        ee(k,:)=[elips(k,2),elips(k,1),elips(k,6),elips(k,8),elips(k,7)];
    end
end

%16. group ellipses
N_ge=0;
dxy_threshold=5;
dab_threshold=10;
dth_threshold=pi/180*5;
for k=1:N_elip
    if N_ge==0
        N_ge=1;
        ge{1,1}(1,:)=[ee(k,:),k];
    else
        tag=0;
        for i=1:N_ge
            [m,tmp]=size(ge{i});
            for j=1:m
                dx=abs(ee(k,1)-ge{i}(j,1));
                dy=abs(ee(k,2)-ge{i}(j,2));
%                da1=abs(ee(k,4)-ge{i}(j,4));
%                db1=abs(ee(k,5)-ge{i}(j,5));
%                da2=abs(ee(k,4)-ge{i}(j,5));
%                db2=abs(ee(k,5)-ge{i}(j,4));
%                dth1=min(abs(mod(ee(k,3)-ge{i}(j,3),pi)),abs(mod(ge{i}(j,3)-ee(k,3),pi)));
%                dth2=min(abs(mod(ee(k,3)+pi/2-ge{i}(j,3),pi)),abs(mod(ge{i}(j,3)-ee(k,3)-pi/2,pi)));
                if dx<dxy_threshold & dy<dxy_threshold %& ...
%                        ((da1<dab_threshold & db1<dab_threshold & dth1<dth_threshold) |...
%                        (da1<dab_threshold & db1<dab_threshold & dth1<dth_threshold))
                    p=m+1;
                    ge{i}(p,:)=[ee(k,:),k];
                    tag=1;
                    break;
                end
            end
            if tag==1
                break;
            end
        end
        if tag==0
            N_ge=N_ge+1;
            ge{N_ge,1}(1,:)=[ee(k,:),k];
        end
    end
end

%17. choose 2 representatives from each set
for k=1:N_ge
    ind1=ge{k}(:,6);
    ints=elips(ind1,9);
    [v1,ind2]=sort(ints);
    ind3=flipud(ind2);
    v2=flipud(v1);
    ger{k,1}=[ge{k,1}(ind3(1:2),:),v2(1:2)];
end

%18. choose 2 sets out of M sets
s=zeros(N_ge,1);
for k=1:N_ge
    for c=1:2
%        sm_drwelips(ger{k}(c,1),ger{k}(c,2),ger{k}(c,3),ger{k}(c,4),ger{k}(c,5));
        a=ger{k}(c,4);
        b=ger{k}(c,5);
        u=pi*(1.5*(a+b)-sqrt(a*b));
        s(k)=s(k)+ger{k}(c,7)/u;
    end
end
[v,ind1]=sort(s);
ind2=flipud(ind1);
gefin=ger(ind2(1:2),:);

%for k=1:2
%    for c=1:2
%        sm_drwelips(gefin{k}(c,1),gefin{k}(c,2),gefin{k}(c,3),gefin{k}(c,4),gefin{k}(c,5));
%    end
%end

%19. find center ellipses
%figure;
%imshow(I4);
%axis on;
%hold on;
for k=1:2
    for c=1:2
        ps=sm_pointsofelipse(gefin{k}(c,1),gefin{k}(c,2),gefin{k}(c,3),gefin{k}(c,4),gefin{k}(c,5));
        pstmp{c,1}=sm_centerpointselips(ps,I2);
    end
    psc{k,1}=sm_pscat(pstmp{1},pstmp{2});
%    [Np,tmp]=size(psc{k});
%    for kk=1:Np
%        plot(psc{k}(kk,2),psc{k}(kk,1),'*');
%    end
end

%20. remove bad points
%figure;
%imshow(I4);
%axis on;
%hold on;
[imax,jmax]=size(I2);
I2v=reshape(I2',imax*jmax,1);
for k=1:2
    ind1=psc{k}(:,1)*jmax+psc{k}(:,2);
    vtmp=I2v(ind1);
    vm=mean(vtmp);
    ind2=find(vtmp>=vm);
    pss{k}=psc{k}(ind2,:);
    [Np,tmp]=size(pss{k});
%    for kk=1:Np
%        plot(pss{k}(kk,2),pss{k}(kk,1),'*');
%    end
end

%21.ransac
%figure;
%imshow(I4);
%axis on;
%hold on;
for i=1:2
    efin(i,:)=sm_ransac_elip(pss{i});
    sm_drwelips(efin(i,8),efin(i,9),efin(i,10),efin(i,11),efin(i,12));
end
