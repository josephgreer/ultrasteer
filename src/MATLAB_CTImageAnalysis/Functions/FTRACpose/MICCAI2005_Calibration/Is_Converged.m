%Function to check if a certain series of iterations in the pose estimation
%has converged or not....or converged to the wrong value. To converge, the
%final error has to be less than the maximum allowable value; the last 10
%iterations should have stabilized; the rotation matrix should be valid;
%and the fiducial should be on the same side of the source as the image plane.
function [iteration, fin_err] = Is_Converged(xform, error, max_err, focal_len)

num = max(size(error));
if(error(num)>max_err)
    iteration = -1;
    fin_err = 10000;
elseif(std(error(num-10:end))>0.01*max_err)
    iteration = -1;
    fin_err = 10000;
elseif( abs(det(xform(1:3,1:3))-1)>0.01 )
    iteration = -1;
    fin_err = 10000;
elseif( sign(xform(3,4))~=sign(focal_len) )
    iteration = -1;
    fin_err = 10000;
else
    iteration = max(find(error<max_err));
    fin_err = error(num);
end
    