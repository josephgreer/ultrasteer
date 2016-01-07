% load particles saved from c++ test
function xp = loadParticlesFromBasePath(basePath, params)
if(params.particleFilterMethod == 1)
    
    
    pos = load(sprintf('%sPos.dat',basePath));
    rhos = load(sprintf('%sRho.dat',basePath));
    Rs = load(sprintf('%sRs.dat',basePath));
    if(exist(sprintf('%sws.dat',basePath),'file'))
        ws = load(sprintf('%sws.dat',basePath));
    else
        ws = ones(1,size(Rs,2))/size(Rs,2);
    end
    
    xp = {};
    for i=1:size(pos,2)
        Rc = reshape(Rs(:,i),3,3);
        xc.q = RotationMatrixToQuat(Rc);
        xc.pos = pos(:,i);
        xc.rho = rhos(i);
        xc.w = ws(i);
        xp = vertcat(xp, {xc});
    end
else
    pos = load(sprintf('%sPos.dat',basePath));
    rhos = load(sprintf('%sRho.dat',basePath));
    Rs = load(sprintf('%sRs.dat',basePath));
    sigmas = load(sprintf('%sSigmas.dat',basePath));
    if(exist(sprintf('%sws.dat',basePath),'file'))
        ws = load(sprintf('%sws.dat',basePath));
    else
        ws = ones(1,size(Rs,2))/size(Rs,2);
    end
    
    xp = {};
    for i=1:size(pos,2)
        xc.pos = pos(:,i);
        xc.rho = rhos(i);
        Rc = reshape(Rs(:,i),3,3);
        sigmac = reshape(sigmas(:,i),3,3);
        xc.qdist = SO3Gaussian(Rc, sigmac);
        xc.w = ws(i);
        xp = vertcat(xp, {xc});
    end
end
end