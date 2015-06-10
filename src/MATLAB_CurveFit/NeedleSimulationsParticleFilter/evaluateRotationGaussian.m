% evaluate gaussian on SO(3) 
% p = probability of rotation
% gaussian distributed according to 
% N(mu, sigma) (see Lie Groups for 2D and 3D Transformations)
% (Ethan Eade) (2.5.1)
function p = evaluateRotationGaussian(R, mu, sigma)
expE = R*mu';
eps = SO3Log(expE);
p = mvnpdf(eps, zeros(3,1), sigma);
end