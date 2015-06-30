% load particles saved from c++ test
function xp = loadParticles(params, paths)
if(params.particleFilterMethod == 1)
    pos = load(paths{1});
    rhos = load(paths{2});
    Rs = load(paths{3});
    
    xp = {};
    for i=1:size(pos,2)
        xc.pos = pos(:,i);
        xc.rho = rhos(i);
        Rc = reshape(Rs(:,i),3,3);
        xc.q = RotationMatrixToQuat(Rc);
        xc.w = 1;
        xp = vertcat(xp, {xc});
    end
else
    pos = load(paths{1});
    rhos = load(paths{2});
    Rs = load(paths{3});
    sigmas = load(paths{4});
    
    xp = {};
    for i=1:size(pos,2)
        xc.pos = pos(:,i);
        xc.rho = rhos(i);
        Rc = reshape(Rs(:,i),3,3);
        sigmac = reshape(sigmas(:,i),3,3);
        xc.qdist = SO3Gaussian(Rc, sigmac);
        xc.w = 1;
        xp = vertcat(xp, {xc});
    end
end
end