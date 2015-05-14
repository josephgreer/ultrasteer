function [ done ] = checkCompletion( mu_x, t, epsilon )
%CHECKCOMPLETION Returns a boolean flag to indicate whether the simulation
%completion conditions have been met
% Inputs:
% mu_x - the current estimated state ([x y z a b g]')
% p - the target point
% epsilon - the tolerance 
% Outputs:
% done - a boolean flag indicating completion

% Get the relative error in the tip frame
T = mu_x;
R = T(1:3,1:3);
e = R'*(t-mu_x(1:3,4));

if ( abs( e(3) ) <= epsilon || e(3) < 0)
    done = true;
else
    done = false;
end