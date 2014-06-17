%$Id: OverlayLinesOnDewarpImage.m,v 1.1 2006/07/15 21:25:59 gouthami Exp $

function OverlayLinesOnDewarpImage(im, lineEndPoints)

numLines = size(lineEndPoints, 1);
figure; imshow(im);
for i = 1:numLines
    p = [lineEndPoints(i,1:2); lineEndPoints(i,3:4)];
    hold on; plot(p(:,2), p(:,1), 'b-'); hold off
end

%$Log: OverlayLinesOnDewarpImage.m,v $
%Revision 1.1  2006/07/15 21:25:59  gouthami
%Adding to CVS
%