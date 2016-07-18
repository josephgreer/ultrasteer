% e is contraction ratio
% mode = 'pleatead" corresponds to pleated actuator
% mode = 'ellipse' corresponds to spheroid
function force = ContractionToForce(e,L0,R0,P,mode)
if strcmpi(mode,'plated') == 0
    f = @(x)([ellipticE(x(1),x(2))/(sqrt(x(2))*cos(x(1)))-L0/R0*(1-(e/2));...
        ellipticF(x(1),x(2))/(sqrt(x(2))*cos(x(1)))-L0/R0]);
    
    if(e == 0)
        phir = 0;
        m = 0;
    else
        phirm = fsolve(f,ones(2,1));
        phir = phirm(1);
        m = phirm(2);
    end
    
    force = pi*P*R0^2*(1-2*m)/(2*m*cos(phir)^2);
elseif strcmpi(mode,'ellipse')
    assert(0);
else
    assert(0);
end
end