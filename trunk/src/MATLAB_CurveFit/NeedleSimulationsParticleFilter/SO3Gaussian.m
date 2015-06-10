%% Represents a gaussian with mean mu (3x3 orthogonal matrix)
% and covariance sigma. (see Lie Groups for 2D and 3D Transformations
% by Ethan Eade)
classdef SO3Gaussian
    properties
        mu % 3x3 matrix
        sigma % 3x3 matrix
    end
    methods
        % construct a guassian with mean mu, covariance sigma
        function obj = SO3Gaussian(mu, sigma)
            obj.mu = mu;
            obj.sigma = sigma;
        end
        
        % see 2.5.1 of Eade paper
        function p = evaluateDensity(obj, R)            
            p = evaluateDensityMuSigma(R, obj.mu, obj.sigma);
        end
        
        function drawit(obj, f, npoints)
            figure(f);
            
            % for drawing on the sphere we have only two parameters
            % in this case we will use yaw, pitch. We will assume the roll
            % of our rotations matches the roll of our mu for the purposes
            % of plotting the distribution
            
            % to do this, "remove the effect" of roll on our mu
            
            % spherical coordinates are R_Z(yaw)R_X(pitch)R_Y(roll) (yaw, pitch)
            % this is what we do our plotting in
            % so calculate mu = R_Z(yaw)R_X(pitch)R_Y(roll)
            % and calculate mup = R_Z(yaw)R_X(pitch)R_Y(roll)R_Y(roll)^T
            % so that we have removed the effect of roll
            xang = XAngleFromZYXRotation(obj.mu);
            mup = obj.mu*Rx(xang)';
            
            [theta psi] = meshgrid([-pi:2*pi/npoints:pi], [-pi/2:pi/npoints:pi/2]);
            ps = ones(size(theta));
            
            for i=1:size(theta,1)
                for j=1:size(theta,2)
                    Rc = Rz(theta(i,j))*Ry(psi(i,j));
                    ps(i,j) = evaluateDensityMuSigma(Rc, mup, obj.sigma);
                end
            end
            
            
            sphere3d(ps,-pi,pi,-pi/2,pi/2,1,0.5,'surf','spline', 1e-3);
            colormap jet;
        end
    end
end



% assume we're given a rotation matrix
% formed by intrinsic rotations about Z,X,Y
% find the roll angle (i.e. the rotation about Y)
function xang = XAngleFromZYXRotation(R)
xang = atan2(R(3,2),R(3,3));
end

function p = evaluateDensityMuSigma(R, mu, sigma)
expE = R*mu';
eps = SO3HatInverse(SO3Log(expE));
p = mvnpdf(eps, zeros(3,1), sigma);
end