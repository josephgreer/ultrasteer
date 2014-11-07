%%%%% input is poly = [a_degreex*x^degree+...+a_0x;
%%%%                   a_degreey*y^degree+...+a_0y;
%%%%                   a_degreez*z^degree+...+a_0z];
%%%%           descriptor = descriptor axis [x; y; z], x0;
%%%%           degree = degree of polynomial
%%%%           samples = current samples
%%%%           return value erros = [  |sample1-predicted1|;
%%%%                                    ...                                                        
%%%%                                   |samplen-predictedn|;];

function errors =  EvaluateModelOnPoints(poly, descriptor, degree, samples)
A = [];
if(size(descriptor,1) ~= 3 && size(descriptor,2) ~= 1)
    error('wrong format descriptor');
end

tvals = TvalsFromDescriptor(samples, descriptor);
for i=degree:-1:0
    A = [A tvals.^i];
end

x = A*transpose(poly(1,:));
y = A*transpose(poly(2,:));
z = A*transpose(poly(3,:));

ex = x-samples(:,1);
ey = y-samples(:,2);
ez = z-samples(:,3);

errors = (ex.^2+ey.^2+ez.^2).^(1/2);
end