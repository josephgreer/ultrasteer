function dist = PointLineDistance(P1,P2,xy)
dist = abs(((P2(2)-P1(2))*xy(1)-(P2(1)-P1(1))*xy(2)+P2(1)*P1(2)-P2(2)*P1(1))/norm(P2-P1));
end