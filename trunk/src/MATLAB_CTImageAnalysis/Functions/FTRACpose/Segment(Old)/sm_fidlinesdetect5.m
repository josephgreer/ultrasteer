function [Lf,Lgcr,L]=sm_fidlinesdetect5(Is,Ig,Ie,Ies)
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

dis_thresh=2;
ang_thresh=pi/180*5;
Ltmp=[zeros(N_lines,3),L];
while 1>0
    %3.find the central line for each edge line;
    [NL,tmp]=size(Ltmp);
	Lc=zeros(NL,9);
	ps=cell(NL,1);
	for i=1:NL
        [Lc(i,:),ps{i}]=sm_centraloneline3(Ltmp(i,4:5),Ltmp(i,6:7),Is,Ie);%Ig
	end
    
    %4.group central lines into groups using thresholding; 
    g=sm_linesgroup3(Lc,dis_thresh,ang_thresh,ang_range);
    
    %5.find the representative central line for each group;
    [Ng,tmp]=size(g);
    Lgcr=zeros(Ng,9);
    psr=cell(Ng,1);
	for i=1:Ng
        [Lgcr(i,:),psr{i}]=sm_groupreplinefind2(ps(g{i}(:,10)),imax,jmax);
	end
    [a,tmp]=size(Ltmp);
    [b,tmp]=size(Lgcr);
    if a==b
        break;
    else
        Ltmp=Lgcr;
        ps=psr;
    end
end

%6.find the 3 desired central lines;
Lf=sm_deslinesfind2(Lgcr,Is,Ies);

