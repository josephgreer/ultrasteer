%%%% creats a list of n polynomial points sampled evenly from given range
%%%% using supplied polynomial
%%%% dRange = [lim1 lim2] range of descriptor variable
%%%% polyModel = [a_{1d+1} a_{1d} ... {a_1}
%%%%              a_{2d+1} a_{2d} ... {a_1}]


function points = PolyPoints(dRange, polyModel, n)

dVar = transpose([dRange(1):(dRange(2)-dRange(1))/n:dRange(2)]);
d = size(polyModel, 2);

var = [];
for i=d-1:-1:0
   var = [var dVar.^i];
end

if size(polyModel,1) == 2
    error('PolyModel expected to be 3 variables');
else
    p1 = var*transpose(polyModel(1,:));
    p2 = var*transpose(polyModel(2,:));
    p3 = var*transpose(polyModel(3,:));
    points = [p1 p2 p3];
end
end

