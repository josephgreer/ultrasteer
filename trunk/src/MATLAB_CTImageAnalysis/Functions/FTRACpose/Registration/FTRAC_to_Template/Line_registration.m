%$Id: Line_registration.m,v 1.4 2006/11/29 00:48:49 jain Exp $

%Function to register straight lines  (not segments) in frame A to the same
%lines in frame B. The output is a transformation that transforms a
%point in frame A to its coordinate in frame B. The final_res_err is the
%registration error during translation fitting. The lines are inputted as a
%3D matrix(3x2xN), column1 being a point on the line, column2 being its normalized 
%direction, and each line makes the third dimension. First a rotation is
%computed by trying to match only the direction vectors. Since a line can be
%represented using a vector pointing in any direction, there are 2^N
%possibilities for matching. One of them is the correct match. All are tested 
%and the one with the lowest 'total' registration error is chosen. Once
%rotation has been computed, translation is computed by trying to match the
%line-points in frame A to fit on the 3D lines in frame B (preferably the
%more accurate one). Input are two 3x2xN matrices for the lines and a 4x4
%transform matrix for the output.
function [xform_A_to_B, final_res_err] = Line_registration(lines_B, lines_A)

num_lines = size(lines_A, 3);
if(size(lines_B,3)~=num_lines)
    display 'Number of lines passed to line registration unequal. Quitting!'
    return;
end

%initializations.
xform_A_to_B = -1*ones(4,4);
current_min_residual_error = 100000;
current_min_residual_error_rotation = 100000;
current_min_center_diffs = 100000;
final_res_err = 10000;
for counter_transform = 0 : (2^num_lines - 1)
    
    %First compute rotation using direction vectors of the line. Do not
    %change the direction of vectors in B, but try all 2^N possibilities
    %for lines in A. Use Arun's method for computing this rotation.
    tmp_lines_A = lines_A;
    tmp_lines_B = lines_B;
    residual_encoding = counter_transform;
    for counter_line = 1 : num_lines
        tmp_flag = mod(residual_encoding,2);
        if(tmp_flag==1)
            tmp_lines_A(:,2,counter_line) = -tmp_lines_A(:,2,counter_line);
        else
            tmp_lines_A(:,2,counter_line) = tmp_lines_A(:,2,counter_line);
        end
        residual_encoding = (residual_encoding - tmp_flag)/2;
    end
    
    %Get the rotation matrix using Arun's method. If the determinant is
    %negative, then flip the sign of the smallest singular value column in
    %V. Also store the residual error in rotations.
    dir_line_A = squeeze(tmp_lines_A(:,2,:));
    dir_line_B = squeeze(tmp_lines_B(:,2,:));
    [U D V] = svd(dir_line_B*dir_line_A');
    rotation_A_to_B = U*V';
    if( det(rotation_A_to_B)<0 )
        V(:,3) = -V(:,3);
        rotation_A_to_B = U*V';
    end
    tmp = dir_line_B - rotation_A_to_B*dir_line_A;
    residual_err_rotation = sum(sqrt(sum(tmp.*tmp)));
    
    %Now compute the translation for this rotation. get the least square
    %translation that fits these points in frameA on the corresponding
    %lines in frame B.
    A = [];
    b = [];
    for counter_line = 1 : num_lines
        [eye(3) zeros(3,(counter_line-1)) squeeze(tmp_lines_B(:,2,counter_line)) zeros(3, (num_lines-counter_line))];
        A = [A ; [eye(3) zeros(3,(counter_line-1)) squeeze(tmp_lines_B(:,2,counter_line)) zeros(3, (num_lines-counter_line))]];
        b = [b ; (squeeze(tmp_lines_B(:,1,counter_line)) - rotation_A_to_B*squeeze(tmp_lines_A(:,1,counter_line)))];
    end
    x = A\b;
    translate_A_to_B = x(1:3);
    residual_err = mean(sqrt(sum(reshape(abs(A*x - b), 3, num_lines).^2)));
    
    %In symmetric situations of three lines (in general three lines are
    %sufficient for a unique registration), I use the distances between
    %corresponding centers to make the decision.
    transformed_points_of_A = rotation_A_to_B*squeeze(lines_A(:,1,:)) + repmat(translate_A_to_B, 1, num_lines);
    center_diffs = squeeze(lines_B(:,1,:)) - transformed_points_of_A;
    residual_center_diffs = sqrt(sum(sum(center_diffs.^2)));
    
    %Save the transformation with miminum error (both in rotation and total) as the 
    %final answer. 0.01 is for numerical stability.
    if( (residual_err < current_min_residual_error + 0.0001) && (residual_err_rotation < current_min_residual_error_rotation + 0.0001) && (residual_center_diffs < current_min_center_diffs + 0.0001))
        current_min_residual_error = residual_err;
        current_min_residual_error_rotation = residual_err_rotation;
        current_min_center_diffs = residual_center_diffs;
        xform_A_to_B = [rotation_A_to_B translate_A_to_B ; 0 0 0 1];
        final_res_err = current_min_residual_error;
    end
    
end
    
if(det(xform_A_to_B(1:3,1:3)) < 0)
    display 'Cannot register the 3D lines. Registration *still* yields a reflection - should never happen. Maybe very high noise in line directions. Check answer!'
    return;
end

%$Log: Line_registration.m,v $
%Revision 1.4  2006/11/29 00:48:49  jain
%Have changed the file that loads the new template as compared to the old one.
%Also have updated the line registration function to incorporate a) reflections in Arun's
%method, b) symmetric registrations that can happen with 3 lines, c) also look at
%residual errors after matching just the rotations.
%
%Revision 1.3  2005/12/14 21:36:47  jain
%Added the input and output formats in the comments.
%
%Revision 1.2  2005/12/14 20:11:39  jain
%corrected a minor apostrofy error.
%
%Revision 1.1  2005/12/12 23:16:22  jain
%Commiting the first complete version of the code for registering the FTRAC fiducial to the template. Code works and has been tested.
%
