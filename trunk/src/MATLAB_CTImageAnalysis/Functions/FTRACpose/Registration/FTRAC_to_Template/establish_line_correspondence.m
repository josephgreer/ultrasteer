%$Id: establish_line_correspondence.m,v 1.2 2005/12/14 21:36:47 jain Exp $

%This code establishes the correspondance of the lines. The labelling of
%lines 1-7 is as in the figure with Ameet. It is a little dirty and is only
%for our phantom. First cluster 1-4 and 5-7 from each other using the length
%of their lines segments as a metric. We find line 6 first. Then use that to find
%line 5 and 7, using orthogonality to 6 and the distance from lines 1-4 
%as a metric. Now 
%find lines 1,3 and 2,4 using this orientation. Now using the distance
%metric from lines 5-7, differentiate between 1/3 and 2/4. That ends it.
%The output are the lines resorted in order 1-7 (in dimension 3). Input is
%3x2xN matrix for lines, Mx3 for the points, and a number of threshold.
%outoput is a 3x2xN lines.
function final_segmented_lines = establish_line_correspondence(input_lines, all_pts, LINE_ERR_THRESHOLD);

correspondence = -1*ones(7,1);

%cluster 1-4 from 5-7 using length of line segment. set1 contains the
%indices of lines 1-4 in the input data, while set2 contains indices of
%lines 5-7. First calculate the segment length.
line_properties = [];
for line_count = 1 : length(input_lines)

    line_pts = find_allpoints_on_line(input_lines(:,:,line_count), all_pts, LINE_ERR_THRESHOLD);
    initial_pt = line_pts(1,:);
    distances = sqrt(sum((line_pts - repmat(initial_pt, length(line_pts),1)).^2, 2));
    max_index = find(distances==max(distances));
    one_endpoint = line_pts(max_index(1), :);
    distances = sqrt(sum((line_pts - repmat(one_endpoint, length(line_pts),1)).^2, 2));
    max_index = find(distances==max(distances));
    other_endpoint = line_pts(max_index(1), :);
    segment_length = sqrt(sum((one_endpoint - other_endpoint).^2,2));
    
    line_properties = [line_properties; squeeze(input_lines(:,1,line_count))' segment_length];
    
end

%Now get set1 and set2.
length_threshold = mean(line_properties(:,4));
set1 = find(line_properties(:,4)<=length_threshold);
set2 = find(line_properties(:,4)>length_threshold);
length_threshold = (mean(line_properties(set1, 4)) + mean(line_properties(set2, 4)))/2;
set1 = find(line_properties(:,4)<=length_threshold);
set2 = find(line_properties(:,4)>length_threshold);

%get line 6 and redefine sets.
set2_orientations = [input_lines(:,2, set2(1)) input_lines(:,2, set2(2)) input_lines(:,2, set2(3))];
index = find( abs(sum(abs(set2_orientations'*set2_orientations),2) - 1) < 0.5 );
correspondence(6) = set2(index);
set2_remaining = [set2(1:index-1) ; set2(index+1:end)];

%get lines 5 and 7.
mean_set1 = mean(squeeze(input_lines(:,1,set1)), 2);
dist = sum((cross((squeeze(input_lines(:,1,set2_remaining)) - repmat(mean_set1, 1, 2)), squeeze(input_lines(:,2,set2_remaining)))).^2)';
if(dist(1) <= dist(2))
    correspondence(7) = set2_remaining(1);
    correspondence(5) = set2_remaining(2);
else
    correspondence(5) = set2_remaining(1);
    correspondence(7) = set2_remaining(2);
end

%Differtiate  between clsuters 1,3 and 2,4 using orientation.
index = find(abs(dot(squeeze(input_lines(:,2,set1)), repmat( input_lines(:,2,correspondence(6)), 1, 4))) > 0.5);
set1_parallel_6 = set1(index);
index = find(abs(dot(squeeze(input_lines(:,2,set1)), repmat( input_lines(:,2,correspondence(6)), 1, 4))) <= 0.5);
set1_perpendicular_6 = set1(index);

%Decide for 2 and 4 using distance.
dist = sum((cross((squeeze(input_lines(:,1,set1_parallel_6)) - repmat(input_lines(:,1,correspondence(6)), 1, 2)), squeeze(input_lines(:,2,set1_parallel_6)))).^2)';
if(dist(1) <= dist(2))
    correspondence(2) = set1_parallel_6(1);
    correspondence(4) = set1_parallel_6(2);
else
    correspondence(4) = set1_parallel_6(1);
    correspondence(2) = set1_parallel_6(2);
end

%Decide between 1 and 3 using distance from 5-7.
dist = sum((cross((squeeze(input_lines(:,1,set1_perpendicular_6)) - repmat(input_lines(:,1,correspondence(5)), 1, 2)), squeeze(input_lines(:,2,set1_perpendicular_6)))).^2)';
if(dist(1) <= dist(2))
    correspondence(1) = set1_perpendicular_6(1);
    correspondence(3) = set1_perpendicular_6(2);
else
    correspondence(3) = set1_perpendicular_6(1);
    correspondence(1) = set1_perpendicular_6(2);
end

%final answer.
final_segmented_lines = input_lines(:,:,correspondence);

%$Log: establish_line_correspondence.m,v $
%Revision 1.2  2005/12/14 21:36:47  jain
%Added the input and output formats in the comments.
%
%Revision 1.1  2005/12/12 23:16:22  jain
%Commiting the first complete version of the code for registering the FTRAC fiducial to the template. Code works and has been tested.
%
