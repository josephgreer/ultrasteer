% load particles saved from c++ test
function xp = loadTruthsFromBasePath(basePath)
pos = load(sprintf('%sPos.dat',basePath));
rhos = load(sprintf('%sRho.dat',basePath));
Rs = load(sprintf('%sRs.dat',basePath));
ws = ones(1,size(Rs,2));

xp = {};
for i=1:size(pos,1)
    Rc = reshape(Rs(:,i),3,3);
    xc.q = RotationMatrixToQuat(Rc);
    xc.pos = pos(i,:)';
    xc.rho = rhos(i);
    xc.w = ws(i);
    xp = vertcat(xp, {xc});
end
end