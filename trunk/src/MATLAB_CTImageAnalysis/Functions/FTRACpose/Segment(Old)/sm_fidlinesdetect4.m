function [Lf,Lgcr]=sm_fidlinesdetect4(Is,Ig,Ie,Ies)
%detect straight lines in the fiducial;

disp('Detecting the straight lines of the fiducial...');

%1.Limit the range of the search angles using radon transform;
angle=sm_radon(Ie);

%2.detect edge lines using hough transform;
ang_range=pi/180*5;
dis_stp=0.5;
ang_stp=pi/360;
N_lines=20;
res=sm_houghline4(Ie,angle,ang_range,dis_stp,ang_stp,N_lines);
[imax,jmax]=size(Ie);
[N_lines,tmp]=size(res);
L=zeros(N_lines,4);
for k=1:N_lines
    [L(k,1:2),L(k,3:4)]=sm_drawline(imax,jmax,res(k,1),res(k,2));
    [L(k,5),L(k,6)]=sm_rthofline(L(k,1:2),L(k,3:4));
end

%3.find the central line for each edge line;
Ltmp=[zeros(N_lines,3),L];
Lgc=zeros(N_lines,9);
ps=cell(N_lines,1);
for i=1:N_lines
    [Lgc(i,:),ps{i}]=sm_centraloneline2(Ltmp(i,4:5),Ltmp(i,6:7),Ig,Ie);
end

%4.group central lines into groups using thresholding, and find the representative for each group;
dis_thresh=2;
ang_thresh=pi/180*5;
Lctmp=Lgc;
pstmp=ps;
while 1>0
    g=sm_linesgroup3(Lctmp,dis_thresh,ang_thresh,ang_range);
	[Ng,tmp]=size(g);
	Lgcr=zeros(Ng,9);
    psr=cell(Ng,1);
	for i=1:Ng
        [Lgcr(i,:),psr{i}]=sm_groupreplinefind2(pstmp(g{i}(:,10)),imax,jmax);
	end
    [a,tmp]=size(Lctmp);
    [b,tmp]=size(Lgcr);
    if a==b
        break;
    else
        Lctmp=Lgcr;
        pstmp=psr;
    end
end

%5.find the 3 desired central lines;
%work on this !!!
Lf=sm_deslinesfind(Lgcr,Ies);

