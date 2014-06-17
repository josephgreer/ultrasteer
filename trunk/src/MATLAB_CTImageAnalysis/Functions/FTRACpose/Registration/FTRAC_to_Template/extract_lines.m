%$Id: extract_lines.m,v 1.2 2005/12/14 21:36:47 jain Exp $

%function to extract 3D lines from a full 3D data set. inputs are the
%points and the appropriate thresholds, output is a list of clustered
%lines, noise-removed 3D points that remain and the fitting error. The algo
%is very simple. Take the first 10 points....if the fiting has high error,
%through these away and take the next 10 points. The line that is fit to
%these is then used to extract all the 3D points from the full set that
%fall on this line. Use them to fit the next iteration of this 3D line. use
%that to find out all points that fit on the line. Iterate until
%convergence. After convergence, remove the 3D points falling on this line
%from the set and try to find the next line in this way. Have some basic
%error checks for noise/bad data in place. Input is Nx2 for points and
%numbers for the rest. Output is 3x2xM for lines.
function [clustered_lines, final_pts, line_fit_err] = extract_lines(input_pts, LINE_ERR_THRESHOLD, NUM_INITIAL_POINTS)

%initial declarations.
num_total_pts = length(input_pts);
remaining_pts = input_pts;
final_pts = [];
clustered_lines = [];
line_counter = 0;
deleted_counter = 0;
line_fit_err = [];

%while there are still points remaining to fit a line.
while(length(remaining_pts) > NUM_INITIAL_POINTS)

    %plot these.
    plot3(remaining_pts(1:NUM_INITIAL_POINTS,1), remaining_pts(1:NUM_INITIAL_POINTS,2), remaining_pts(1:NUM_INITIAL_POINTS,3), 'rx');
    pause(0.5)

    %fit a line to first 10 points. If bad line , do the needful. If good
    %line, then continue.
    [line residual_err] = fit_least_square_3dline(remaining_pts(1:NUM_INITIAL_POINTS,:));
    if(residual_err > LINE_ERR_THRESHOLD)
        residual_err
        display 'High residual error in fitting initial 3D line. Might be noisy points. Deleting these points.'
        plot3(remaining_pts(1:NUM_INITIAL_POINTS,1), remaining_pts(1:NUM_INITIAL_POINTS,2), remaining_pts(1:NUM_INITIAL_POINTS,3), 'kx');
        pause(1)
        remaining_pts = remaining_pts(NUM_INITIAL_POINTS + 1:end, :);
        deleted_counter = deleted_counter + 1;
        if( ((deleted_counter*NUM_INITIAL_POINTS*100)/num_total_pts) > 2 )
            display 'Over 2% points deleted. Quitting. Check data or Increase error-threshold/number of points used for initial 3D line.'
            return;
        end
        continue;
    end
    
    %Now use this line as a start and iterate to get the best fitting line,
    %till the process converges. Once it converges, save that line, remove
    %its points fom the set and quit to start looking for the next line.
    converge_flag = 0;
    iter_counter = 0;
    num_points_used = NUM_INITIAL_POINTS;
    while(converge_flag==0)
        [line_pts res_pts] = find_allpoints_on_line(line, remaining_pts, LINE_ERR_THRESHOLD);
        new_num_points_used = length(line_pts);
        %if converged. If not to a great line, then throw these away.
        %Else save the line. If not converged, try another iteration.
        if(new_num_points_used == num_points_used)
            if(num_points_used<100)
                display 'Converged to a 3D line with less than 100 points. Not good. Deleting these points and continuing, but check data.'
                plot3(line_pts(:,1),line_pts(:,2),line_pts(:,3), 'kx');
                pause(1)
                break;
            else
                converge_flag = 1;
                final_pts = [final_pts; line_pts];
                [line residual_err] = fit_least_square_3dline(line_pts);
                clustered_lines = cat(3, clustered_lines, line);
                line_fit_err = [line_fit_err ; residual_err];
                plot3(line_pts(:,1),line_pts(:,2),line_pts(:,3), 'b.');
                line_counter = line_counter+1;
                pause(1)
            end
        else
            converge_flag = 0;
            num_points_used = new_num_points_used;
            iter_counter = iter_counter+1;
            [line residual_err] = fit_least_square_3dline(line_pts);
            if(iter_counter>=20)
                display '3D least square line fitting not convering even after 20 iterations. Quitting. Check data.'
                return;
            end
        end
    end
    
    remaining_pts = res_pts;
   
end

if(line_counter~=7)
    line_counter
    display 'The number os lines is not 7. Quitting. Check data.'
end

%$Log: extract_lines.m,v $
%Revision 1.2  2005/12/14 21:36:47  jain
%Added the input and output formats in the comments.
%
%Revision 1.1  2005/12/12 23:16:22  jain
%Commiting the first complete version of the code for registering the FTRAC fiducial to the template. Code works and has been tested.
%
