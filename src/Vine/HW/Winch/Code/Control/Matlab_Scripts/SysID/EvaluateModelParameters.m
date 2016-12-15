function error = EvaluateModelParameters(xs,Ps,psis,pos0,R0,radius,verticalOffset,params,kleq)

kmults = kleq(1:3);
kmain = kleq(4);
ktor = kleq(5);
leqs = kleq(6:8);

offset = 9;

if(params.fitMainEqLength)
    leqsWithMain = vertcat(leqs,kleq(offset));
    offset = offset+1;
else
    leqsWithMain = vertcat(leqs,verticalOffset);
end

if(params.fitGamma == 1)
    gammas = kleq(offset:offset+2);
    offset = offset+3;
elseif(params.fitGamma == 2)
    gammas = kleq(offset)*ones(3,1);
    offset = offset+1;
else
    gammas = ones(3,1);
end

if(params.fitTorGamma)
    torGamma = kleq(offset);
    offset = offset+1;
else
    torGamma = 1;
end

error = 0;
for i=1:length(xs)
    PP = Ps(i,:).';
    PP = PP.^gammas;
    ks = kmults.*PP;
    ks(4) = kmain;
    ks(5) = ktor;
    systemFull = @(x)(FullSpringSystemEquations(x, ks, leqsWithMain, radius, psis,torGamma));
    [ls, fval] = fsolve(systemFull, leqs(1:3));
    [theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,radius,psis);
    
    xcurr = kinematicParametersToTipPoint(l,phi,theta,R0,pos0);
    error = error+norm(xcurr-xs{i}.pos);
end
end