figure
isosurface(shapeAnalysis(9).Image)
hold on
scatter3(shapeAnalysis(9).Data(:,1),shapeAnalysis(9).Data(:,2),shapeAnalysis(9).Data(:,3),'.r')
hold on
 [Xpts,Ypts,Zpts] = Linepts(shapeAnalysis(9).line(1:3),shapeAnalysis(9).line(4:6));
 plot3(Xpts,Ypts,Zpts)
%circle3(shapeAnalysis(9).curve(2:4),shapeAnalysis(9).curve(5:7),shapeAnalysis(9).curve(1))

figure
isosurface(shapeAnalysis(7).Image)
hold on
scatter3(shapeAnalysis(7).Data(:,1),shapeAnalysis(7).Data(:,2),shapeAnalysis(7).Data(:,3),'.r')
hold on
 [Xpts,Ypts,Zpts] = Linepts(shapeAnalysis(7).line(1:3),shapeAnalysis(7).line(4:6));
 plot3(Xpts,Ypts,Zpts)
%circle3(shapeAnalysis(7).curve(2:4),shapeAnalysis(7).curve(5:7),shapeAnalysis(7).curve(1))