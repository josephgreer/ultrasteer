function summaryData = generateSummaryData(shapeAnalysis,scansNumberSorted,inVivo)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
[m n] = size(shapeAnalysis);

summaryData = zeros(5,length(shapeAnalysis));
index = 0;
for i = 1:m
    for j=1:n
        index = index+1;
        if inVivo == 1
        summaryData(1,index) = scansNumberSorted(j);
        else
        summaryData(1,index) = j;
        end
        if ~isempty(shapeAnalysis(i,j).line)
            summaryData(2,index) = 1;
            summaryData(3,index) = shapeAnalysis(i,j).lineRESNORM/sqrt(length(shapeAnalysis(i,j).Data)-2);
        else
            if isempty(shapeAnalysis(i,j).curve)
            %summaryData(1,index) = 0;
            summaryData(2,index) = 0;
            summaryData(3,index) = 0;
            summaryData(4,index) = 0;
            summaryData(5,index) = 0;

            else
            summaryData(2,index) = 2; 
            summaryData(4,index) = shapeAnalysis(i,j).curve(1);
            summaryData(5,index) = shapeAnalysis(i,j).curveSTDERR;
            end
        end
    end

end

if inVivo == 1
    needleDiameter = [0.74 0.74 0.74 0.58 0.58 0.58 0.74 0.74 0.74 0.74 0.74 0.74 0.74 0.74 0.74];
    needleType = [1 2 3 1 2 3 1 1 1 2 2 2 3 3 3];

else
    needleDiameter = repmat([repmat(0.74,1,9),repmat(0.58,1,9),repmat(0.74,1,9)],1,m);
    needleType = repmat([repmat(1,1,3),repmat(2,1,3),repmat(3,1,3)],1,3*m);
end
    summaryData = [summaryData; needleDiameter; needleType];


end

