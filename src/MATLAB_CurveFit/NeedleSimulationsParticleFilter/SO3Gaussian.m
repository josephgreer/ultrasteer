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
        
        function samples = sampleGaussian(obj, npoints)
            logSamples = mvnrnd(zeros(3,1), obj.sigma, npoints);
            for i=1:npoints
                samples{i} = SO3Exp(obj.mu)*SO3Exp(logSamples(i,:)');
            end
        end
        
        % Compose SO(3) Gaussians
        % apply b then obj
        % b*obj
        function objc = composeGaussians(obj, b)
            objc = SO3Gaussian(b.mu*obj.mu, b.sigma+b.mu*a.sigma*b.mu');
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
        
        function drawdensity(obj, f, npoints)
            hold on;
            if(npoints > 0)
                samples = sampleGaussian(obj, npoints);
                samples = cell2mat(samples);
            end
            [x,y,z] = sphere(100);
            lightGrey = 0.35*[1 1 1]; % It looks better if the lines are lighter
            %surface(x,y,z,'FaceColor', [1 1 1],'EdgeColor', 'none');
            nskip = 10;
            surface(x(1:nskip:end,:), y(1:nskip:end,:), z(1:nskip:end,:), 'FaceColor', 'none', 'EdgeColor',lightGrey, 'MeshStyle', 'row')
            surface(x(:,1:nskip:end), y(:,1:nskip:end), z(:,1:nskip:end), 'FaceColor', 'none', 'EdgeColor',lightGrey, 'MeshStyle', 'column')
            if(npoints > 0)
                scatter3(samples(1,1:3:end)', samples(2,1:3:end)', samples(3,1:3:end)', 10,'r','filled');
                scatter3(samples(1,2:3:end)', samples(2,2:3:end)', samples(3,2:3:end)', 10,'g','filled');
                scatter3(samples(1,3:3:end)', samples(2,3:3:end)', samples(3,3:3:end)', 10,'b','filled');
            end
            mu = SO3Exp(obj.mu);
            plot3([0 mu(1,1)], [0 mu(2,1)], [0 mu(3,1)], 'r', 'LineWidth', 2);
            plot3([0 mu(1,2)], [0 mu(2,2)], [0 mu(3,2)], 'g', 'LineWidth', 2);
            plot3([0 mu(1,3)], [0 mu(2,3)], [0 mu(3,3)], 'b', 'LineWidth', 2);
            daspect([1 1 1]);
            view([-45 45]);
            grid on;
            axis off;
            
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