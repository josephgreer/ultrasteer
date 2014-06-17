function [Lf,Lgcr]=sm_fidlinesdetect2(Is,Ig,Ie,Ies)
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

%3.group edge lines using thresholding;
dis_thresh=2;
ang_thresh=pi/180*5;
Ltmp=[zeros(N_lines,3),L];
g=sm_linesgroup3(Ltmp,dis_thresh,ang_thresh,ang_range);

%4.find the central line for each group;
[Ng,tmp]=size(g);
Lgc=zeros(Ng,9);
ps=cell(Ng,1);
for i=1:Ng
    [NL,tmp]=size(g{i});
    pstmp=cell(NL,1);
    for k=1:NL
        [tmp,pstmp{k}]=sm_centraloneline2(g{i}(k,4:5),g{i}(k,6:7),Ig,Ie);
    end
    [Lgc(i,:),ps{i}]=sm_groupreplinefind2(pstmp,imax,jmax);
end

%5.group central lines into groups using thresholding;
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

%6.find the 3 desired central lines;
%work on this !!!
Lf=sm_deslinesfind(Lgcr,Ies);

