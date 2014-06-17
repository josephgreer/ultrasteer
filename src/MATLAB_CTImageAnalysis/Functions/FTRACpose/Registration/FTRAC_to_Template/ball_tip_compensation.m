%$Id: ball_tip_compensation.m,v 1.2 2005/12/14 21:36:47 jain Exp $
%A function to compensate for the radius of the ball tip and to move all
%the lines by an appropriate amount in the correct direction. Also fits the
%lines on their plane and get a more accurate 3D equation of the lines.
%Basicaly first fit the lines on their respective planes and change the
%equation of the line. Now compute the migration directions (orthogonal to
%the line & plane) and migrate the tart point on the line in these
%directions by the radius of the ball tip. This migrates the while line. To
%figure out the migration direction of te plane, choose the normal vector
%of the plane to be always pointing away from the 'other' plane; and then
%migrate in the direction of the normal vector. For choosing the direction
%of the other vector, compare with the direction of the vector pointing
%from the point on the line to the center of the 'other' plane. First the
%FTRAC plane is migrated and then the template plane. Input is 3x2xN matrix for
%the lines and number of the radius. Output is 3x2xN lines.
function compensated_lines = ball_tip_compensation(input_lines, BALL_RADIUS);

%To figure out generic direction, for comparision.
ftrac_to_template_vec = mean(squeeze(input_lines(:,1,5:7)), 2) - mean(squeeze(input_lines(:,1,1:4)), 2);

%Fit the ftrac lines on a plane. Change the line equations. Migrate the
%lines.
plane_ftrac = fit_least_square_plane(input_lines(:,:,1:4));
if( dot(plane_ftrac(1:3), ftrac_to_template_vec) > 0 )
    plane_ftrac = -plane_ftrac;
end
for line_counter = 1 : 4
    input_lines(:,1,line_counter) = input_lines(:,1,line_counter) - dot(plane_ftrac, [squeeze(input_lines(:,1,line_counter)) ; 1])*plane_ftrac(1:3);
    input_lines(:,2,line_counter) = input_lines(:,2,line_counter) - dot(plane_ftrac(1:3), input_lines(:,2,line_counter))*plane_ftrac(1:3);
    input_lines(:,2,line_counter) = input_lines(:,2,line_counter)/sqrt(sum(input_lines(:,2,line_counter).^2));
end
one_migration_dir = plane_ftrac(1:3);
for line_counter = 1 : 4
    other_migration_dir = cross(input_lines(:,2,line_counter), plane_ftrac(1:3));
    if( dot(other_migration_dir, mean(squeeze(input_lines(:,1,1:4)),2) - input_lines(:,1,line_counter)) < 0 )
        other_migration_dir = -other_migration_dir;
    end
    input_lines(:,1,line_counter) = input_lines(:,1,line_counter) + BALL_RADIUS*one_migration_dir + BALL_RADIUS*other_migration_dir;
end    

%Repeat the above procedure for the template lines.
plane_template = fit_least_square_plane(input_lines(:,:,5:7));
if( dot(plane_template(1:3), -ftrac_to_template_vec) > 0 )
    plane_template = -plane_template;
end
for line_counter = 5 : 7
    input_lines(:,1,line_counter) = input_lines(:,1,line_counter) - dot(plane_template, [squeeze(input_lines(:,1,line_counter)) ; 1])*plane_template(1:3);
    input_lines(:,2,line_counter) = input_lines(:,2,line_counter) - dot(plane_template(1:3), input_lines(:,2,line_counter))*plane_template(1:3);
    input_lines(:,2,line_counter) = input_lines(:,2,line_counter)/sqrt(sum(input_lines(:,2,line_counter).^2));
end
one_migration_dir = plane_template(1:3);
for line_counter = 5 : 7
    other_migration_dir = cross(input_lines(:,2,line_counter), plane_template(1:3));
    if( dot(other_migration_dir, mean(squeeze(input_lines(:,1,5:7)),2) - input_lines(:,1,line_counter)) < 0 )
        other_migration_dir = -other_migration_dir;
    end
    input_lines(:,1,line_counter) = input_lines(:,1,line_counter) + BALL_RADIUS*one_migration_dir + BALL_RADIUS*other_migration_dir;
end    

%The final output.
compensated_lines = input_lines;
%$Log: ball_tip_compensation.m,v $
%Revision 1.2  2005/12/14 21:36:47  jain
%Added the input and output formats in the comments.
%
%Revision 1.1  2005/12/12 23:16:22  jain
%Commiting the first complete version of the code for registering the FTRAC fiducial to the template. Code works and has been tested.
%
