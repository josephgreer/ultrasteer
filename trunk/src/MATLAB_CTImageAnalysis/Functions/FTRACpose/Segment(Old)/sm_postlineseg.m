function Ienew=sm_postlineseg(Ie,Lgcr,L)

%remove the major straight lines from edge image;
Ienew=sm_removelines2(Lgcr(:,4:7),Ie);
Ienew=sm_removelines(L(:,1:4),Ienew);
