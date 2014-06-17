% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: summaryMatrix = dataSummary(data,meanOrStd,ProstateDataSummary,KidneyDataSummary,LiverDataSummary)
% -------------------------------------------------------------------------
% Inputs: data - which colunm of data to take, meanOrSTD - mean or standard
% deviation. Data Summaries - 3D matrices with all the force data. 
% Outputs: summaryMatrix 3x3 matrix averaging the results for each needle
% type.
% Sub_Functions Called: mean(), std(), reshape()
% Synopsis:
%   This function will generate a summary matrix for some data of
%   interest like max insertion, mean retraction etc etc. 
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: FitCurve()
% =========================================================================

function summaryMatrix = dataSummary(data,meanOrStd,ProstateDataSummary,KidneyDataSummary,LiverDataSummary)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
if meanOrStd == 1
summaryMatrix = [mean(reshape(ProstateDataSummary(1:3,data,2:4),9,1)) ...
                mean(reshape(ProstateDataSummary(4:6,data,2:4),9,1)) ...
                mean(reshape(ProstateDataSummary(7:9,data,2:4),9,1));
                mean(reshape(KidneyDataSummary(1:3,data,2:4),9,1)) ...
                mean(reshape(KidneyDataSummary(4:6,data,2:4),9,1)) ...
                mean(reshape(KidneyDataSummary(7:9,data,2:4),9,1));
                mean(reshape(LiverDataSummary(1:3,data,2:4),9,1)) ...
                mean(reshape(LiverDataSummary(4:6,data,2:4),9,1)) ...
                mean(reshape(LiverDataSummary(7:9,data,2:4),9,1))];
else
    summaryMatrix = [std(reshape(ProstateDataSummary(1:3,data,2:4),9,1)) ...
                    std(reshape(ProstateDataSummary(4:6,data,2:4),9,1)) ...
                    std(reshape(ProstateDataSummary(7:9,data,2:4),9,1));
                    std(reshape(KidneyDataSummary(1:3,data,2:4),9,1)) ...
                    std(reshape(KidneyDataSummary(4:6,data,2:4),9,1)) ...
                    std(reshape(KidneyDataSummary(7:9,data,2:4),9,1));
                    std(reshape(LiverDataSummary(1:3,data,2:4),9,1)) ...
                    std(reshape(LiverDataSummary(4:6,data,2:4),9,1)) ...
                    std(reshape(LiverDataSummary(7:9,data,2:4),9,1))];

end

% =========================================================================
% END FUNCTION: FitCurve()
% =========================================================================