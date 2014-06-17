%$Id: Plot_3D_Clouds.m,v 1.1 2006/11/21 18:37:26 jain Exp $

%This is a function to just plot the two point clouds and compute the
%difference vectors. Inputs are the two clouds and the matching between
%them.
function Err_Vecs = Plot_3D_Clouds(True_Seeds_wrt_FTRAC, Computed_Seeds_wrt_FTRAC, matching)

% Plot various figures, inparticular the two clouds on top of each other
% with the difference vectors drawn to show the correspondences.
%figure, plot(err)
figure, hold on
plot3(True_Seeds_wrt_FTRAC(:,1), True_Seeds_wrt_FTRAC(:,2), True_Seeds_wrt_FTRAC(:,3), 'go', 'MarkerSize', 6);
plot3(Computed_Seeds_wrt_FTRAC(:,1), Computed_Seeds_wrt_FTRAC(:,2), Computed_Seeds_wrt_FTRAC(:,3), 'r*', 'MarkerSize', 6)

%Plot the lines
Err_Vecs = [];
for counter = 1 : length(True_Seeds_wrt_FTRAC)
    i = counter;
    j = matching(counter);
    p1 = Computed_Seeds_wrt_FTRAC(i,:);
    p2 = True_Seeds_wrt_FTRAC(j,:);
    Err_Vecs =[Err_Vecs; p2 - p1];
    line( [p1(1); p2(1)], [p1(2); p2(2)], [p1(3); p2(3)], 'LineWidth', 1);
end
grid on
mean(Err_Vecs)

% Plot the error vectors separately to better understand the biases
% figure
% hold on
% plot3(0, 0, 0, 'y.')
% for counter = 1 : length(True_Seeds_wrt_FTRAC)
%     i = counter;
%     j = matching(counter);
%     p1 = Computed_Seeds_wrt_FTRAC(i,:);
%     p2 = True_Seeds_wrt_FTRAC(j,:);
%     p = p2 - p1;
%     plot3([p(1)], [p(2)], [p(3)], 'bo' );
% end
% mean_vec = mean(vecs);
% plot3( [mean_vec(1)], [mean_vec(2)], [mean_vec(3)], 'ro')
% grid on

%$Log: Plot_3D_Clouds.m,v $
%Revision 1.1  2006/11/21 18:37:26  jain
%First version of the set of files which can compare Ca-rm based seed reconstructions to Ground Truth obtained from CT.
%This function currently needs both the seeds and the FTRAC fiducial to be visible in
%the CT frame. Will write and add a new one that just works with 'relative reconstruction
%error' computation - i.e. where a post-operative CT is taken of the patient. The
%starting function currently is Compute_Reconstruction_Error.
%
