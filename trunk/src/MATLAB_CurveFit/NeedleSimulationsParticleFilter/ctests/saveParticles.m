function saveParticles(xhist, pathBase, params)
xx = cell2mat(xhist);

if(params.particleFilterMethod == 1)
    pos = [xx.pos];
    rho = [xx.rho];
    qs = [xx.q];
    ws = [xx.w];
    
    Rs = zeros(9, params.np);
    for i=1:params.np
        Rc = QuatToRotationMatrix(qs(:,i));
        Rs(:,i) = Rc(:);
    end
    
    save(strcat(pathBase,'Pos.dat'), 'pos', '-ascii');
    save(strcat(pathBase,'Rho.dat'), 'rho', '-ascii');
    save(strcat(pathBase,'Rs.dat'), 'Rs', '-ascii');
    save(strcat(pathBase,'ws.dat'), 'ws', '-ascii');
elseif(params.particleFilterMethod == 3)
    pos = [xx.pos];
    rho = [xx.rho];
    ws = [xx.w];
    qdists = [xx.qdist];
    
    Rs = zeros(9, params.np);
    sigmas = zeros(9, params.np);
    for i=1:params.np
        Rs(:,i) = qdists(i).mu(:);
        sigmas(:,i) = qdists(i).sigma(:);
    end
    
    save(strcat(pathBase,'Pos.dat'), 'pos', '-ascii');
    save(strcat(pathBase,'Rho.dat'), 'rho', '-ascii');
    save(strcat(pathBase,'ws.dat'), 'ws', '-ascii');
    save(strcat(pathBase,'Rs.dat'), 'Rs', '-ascii');
    save(strcat(pathBase,'sigmas.dat'), 'sigmas', '-ascii');
    
end
end