% e is contraction ratio
% mode = 'pleatead" corresponds to pleated actuator
% mode = 'ellipse' corresponds to spheroid
function force = ContractionToForce(e,L0,R0,P,a,mode)
if strcmpi(mode,'pleated') == 1
    f = @(x)([(ellipticE(x(1),x(2))-0.5*ellipticF(x(1),x(2)))/(sqrt(x(2))*cos(x(1)))-L0/(2*R0)*(1-e);...
        ellipticF(x(1),x(2))/(sqrt(x(2))*cos(x(1)))-L0/R0*(1+a/(2*x(2)*cos(x(1))^2))]);
    
    if(e == 0)
        phir = 0;
        m = 0;
    else
        phirm = fsolve(f,ones(2,1));
        phir = phirm(1);
        m = phirm(2);
    end
    
    force = pi*P*R0^2*(1-2*m)/(2*m*cos(phir)^2);
elseif strcmpi(mode,'ellipse') == 1
    arclen = @(a,b)(2*a*ellipticE((1-(b/a)^2)));
    f = @(a,b)(arclen(a,b)-L0);
    ys = @(x,a,b)(R0+b*sqrt(1-(x./a).^2));
    volintegrand = @(x,a,b)(pi*(ys(x,a,b)).^2);
    vol = @(a,b)(integral(@(x)(volintegrand(x,a,b)),-a,a));
    
    dl = 1e-2;
    a1 = L0*(1-e)/2;
    a2 = a1+dl/2;
    
    b1 = fsolve(@(b)(f(a1,b)),a1);
    b2 = fsolve(@(b)(f(a2,b)),b1);
    
    V1 = vol(a1,b1);
    V2 = vol(a2,b2);
    
    force = -P*(V2-V1)/dl;
else
    assert(0);
end
end