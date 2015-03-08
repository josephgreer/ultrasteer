% %% Debug findCircle.m
% clc; clear all; close all;
% 
% a = randi(100,3,1);
% b = randi(100,3,1);
% v_i = randi(100,3,1);
% v_i = v_i./norm(v_i);
% N = 100;
% 
% [ rho,c,pts,v_e ] = findCircle( a,b,v_i,N );
% 
% i = [a a+v_i.*50];
% e = [b b+v_e.*50];
% 
% figure(4); clf; hold on;
% grid on; set(gcf,'color','w');
% axis equal;
% 
% plot3(a(1),a(2),a(3),'ro','LineWidth',4);
% plot3(b(1),b(2),b(3),'bo','LineWidth',4);
% plot3(pts(1,:),pts(2,:),pts(3,:),'k--','LineWidth',4);
% plot3(c(1),c(2),c(3),'ko','LineWidth',4);
% plot3(i(1,:),i(2,:),i(3,:),'m--','LineWidth',4);
% plot3(e(1,:),e(2,:),e(3,:),'m--','LineWidth',4);
% 
% %%
% 
% for i = 1:10
%     for j = 1:10
%     
%     display(sprintf('i = %d, j = %d',i,j));
%        
%     if j == 5
%         break;
%     end
%     
%     end
%     
% end

%%

myPts = data.pts.coords;
figure(5); clf;
for i = -90:90
    level = i;
    thesePts = myPts(find(myPts(:,3)==level),:);
    if(~isempty(thesePts))
        plot3(thesePts(:,1),thesePts(:,2),thesePts(:,3),'k.')
        axis equal;
        view(180,90);
        pause;
    end
end


%%

C = rand(100,100,100);
ind = find(C(:) < 0.5);
for j = ind'
    size(j)
    pause;
end


%%

colormap(   [   0 0 0;
                1 1 0;
                0 1 0; ] );
    
            
            
%%

% indUnrch = find( data.V(:) < 0 )';     % points already labeled reachable
indUnrch = randi(50,2000,10);
Nbadpts = numel(indUnrch);

badPts = data.pts.coords.XYZ(indUnrch,:)';
badPts = repmat(badPts,[1,1,25]);

arcPts = rand(3,25,1);
arcPts = permute(arcPts,[1,3,2]);
arcPts = repmat(arcPts,[1,Nbadpts,1]);

tic
vecs = arcPts-badPts;
dist = sum(vecs.^2,1);
bad = any(dist(:) < 1);
toc






