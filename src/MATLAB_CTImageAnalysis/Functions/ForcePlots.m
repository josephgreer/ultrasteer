% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: [ProstateDataSummary KidneyDataSummary LiverDataSummary] = ForcePlots(folder,inVivo)
% -------------------------------------------------------------------------
% Inputs: folder - path to data, inVivo - in vivo (=1) or ex vivo(=0) data set
% Outputs: Force data summaries for the three organs. 
% Sub_Functions Called: xxxx
% Synopsis:
%   This function will generate force plots for the datasets in the folder
%   and return summaries for each organ. 
% =========================================================================

% =========================================================================
% BEGIN FUNCTION: FitCurve()
% =========================================================================

function [ProstateDataSummary KidneyDataSummary LiverDataSummary] = ForcePlots(folder,inVivo)

% -------------------------------------------------------------------------
% Initialization of variables
% -------------------------------------------------------------------------

if inVivo==1                                                               % Folders are organized different, hence the inVivo exVivo selection
    % FOR LIVE ANIMAL
    Pfiles = dir(strcat(folder,'/P*'));
    Kfiles = dir(strcat(folder,'/K*'));
    Lfiles = dir(strcat(folder,'/L*'));
    insertionLimit = [11 5 7];                                             % Max distance of insertion for prostate, kidney, liver
else
    % FOR EX VIVO
    pFolder = dir([folder '/Prostate']);

    if ~isempty(pFolder)
    Pfiles = dir(strcat(folder,'/Prostate/DATA/P*'));
    else
        Pfiles = [];
    end
    Kfiles = dir(strcat(folder,'/Kidney/DATA/K*'));
    Lfiles = dir(strcat(folder,'/Liver/DATA/L*'));
    insertionLimit = [3 7 10];                                             % Max distance of insertion for prostate, kidney, liver
end
datafolder = folder;

% -------------------------------------------------------------------------
% Prostate Data
% -------------------------------------------------------------------------

if isempty(Pfiles)
    ProstateDataSummary = zeros(9,11);                                     % if the prostate folder doesn't exist...
else
    
    ProstateDataSummary = zeros(length(Pfiles),11);                        
    if inVivo == 0
        datafolder = [folder '/Prostate/DATA'];                            % Special path for ex vivo data
    end
    for i = 1:length(Pfiles)
        
        % -----------------------------------------------------------------
        % Extract important information....
        % -----------------------------------------------------------------
        data = load(strcat(strcat(datafolder,'/P'),num2str(i),'/FSENSOR-ADC.txt'));     % Open force data file
        parameters = fopen(strcat(strcat(datafolder,'/P'),num2str(i),'/params.txt'));   % Open insertion/retraction parameter file 
        InputText=textscan(parameters,'%s','delimiter','\n');                           % Read lines
        textFile=InputText{1};

        startTime = findStartTime(textFile);
        [insertionTimes insertionParams] = findInsertionTimes(textFile,startTime);
        [retractionTimes retractionParams] = findRetractionTimes(textFile,startTime);
        [stopTime resetFlag resetDist] = findStopTime(textFile, startTime, insertionTimes, insertionParams, retractionTimes, retractionParams); 

        breakStart = 0;
        breakStop = 0;
         if resetFlag == 1
             [breakStart breakStop] = findBreakStartStopTime(insertionTimes, insertionParams, retractionTimes, retractionParams,stopTime);
         end

        % -----------------------------------------------------------------
        % Plot force data and get fixed data files (some sets have issues)
        % -----------------------------------------------------------------
        [fixedData insertionIDX retractionIDX resetIDX] = plotForceData(data, insertionTimes, insertionParams, retractionTimes, retractionParams, stopTime, breakStart, breakStop,resetFlag,resetDist);
        title(strcat('Prostate Insertion  ',num2str(i)));

        % -----------------------------------------------------------------
        % Generate summary data
        % -----------------------------------------------------------------
        dist = 0;
        for j=1:length(insertionIDX(:,1))
            dist = dist + insertionIDX(j,3);
            if dist >= insertionLimit(1)
                insertionLimitIDX = insertionIDX(j,2);
                j = length(insertionIDX);
            end
        end

        ProstateDataSummary(i,1:6) = [max(fixedData(:,4)) ...                       % Total MAX, MEAN, STD DEV
                                      mean(fixedData(:,4)) ...
                                      std(fixedData(:,4)) ...
                                      max(fixedData(1:insertionLimitIDX,4))...      % MAX, MEAN, STD DEV for FIRST 3 (or 11) CM
                                      mean(fixedData(1:insertionLimitIDX,4)) ...
                                      std(fixedData(1:insertionLimitIDX,4))];     
       if resetFlag == 1                           
       ProstateDataSummary(i,7:9) = [min(fixedData(resetIDX(1):resetIDX(2),4)) ...  % Retraction summary if the needle was retracted. 
                                     mean(fixedData(resetIDX(1):resetIDX(2),4)) ...
                                     std(fixedData(resetIDX(1):resetIDX(2),4))];  
       end
    end
end

% -------------------------------------------------------------------------
% Kidney Data
% -------------------------------------------------------------------------

KidneyDataSummary = zeros(length(Kfiles),11);
if inVivo == 0
    datafolder = [folder '/Kidney/DATA'];
end

        % -----------------------------------------------------------------
        % Extract important information....
        % -----------------------------------------------------------------
        for i = 1:length(Kfiles)
            data = load(strcat(strcat(datafolder,'/K'),num2str(i),'/FSENSOR-ADC.txt'));
            parameters = fopen(strcat(strcat(datafolder,'/K'),num2str(i),'/params.txt'));
            InputText=textscan(parameters,'%s','delimiter','\n'); % Read lines
            textFile=InputText{1};

            startTime = findStartTime(textFile);
            [insertionTimes insertionParams] = findInsertionTimes(textFile,startTime);
            [retractionTimes retractionParams] = findRetractionTimes(textFile,startTime);
            [stopTime resetFlag resetDist] = findStopTime(textFile, startTime, insertionTimes, insertionParams, retractionTimes, retractionParams); 

            breakStart = 0;
            breakStop = 0;
            %resetFlag = 1;
              if resetFlag == 1
                  [breakStart breakStop] = findBreakStartStopTime(insertionTimes, insertionParams, retractionTimes, retractionParams,stopTime);
              end
              
        % -----------------------------------------------------------------
        % Plot force data and get fixed data files (some sets have issues)
        % -----------------------------------------------------------------

         [fixedData insertionIDX retractionIDX resetIDX] = plotForceData(data, insertionTimes, insertionParams, retractionTimes, retractionParams, stopTime, breakStart, breakStop,resetFlag,resetDist);
         title(strcat('Kidney Insertion  ',num2str(i)),'FontSize',12);

        % -----------------------------------------------------------------
        % Generate summary data
        % -----------------------------------------------------------------
         
         dist = 0;
         for j=1:length(insertionIDX(:,1))
            dist = dist + insertionIDX(j,3);
            if dist >= insertionLimit(2)
                insertionLimitIDX = insertionIDX(j,2);
                j = length(insertionIDX);
            end
         end

         KidneyDataSummary(i,1:9) = [max(fixedData(1:insertionIDX(end,2),4)) ...
                                        mean(fixedData(1:insertionIDX(end,2),4)) ...
                                        std(fixedData(1:insertionIDX(end,2),4)) ...
                                        max(fixedData(1:insertionLimitIDX,4)) ...
                                        mean(fixedData(1:insertionLimitIDX,4)) ...
                                        std(fixedData(1:insertionLimitIDX,4)) ...
                                        min(fixedData(resetIDX(1):resetIDX(2),4)) ...
                                        mean(fixedData(resetIDX(1):resetIDX(2),4)) ...
                                        std(fixedData(resetIDX(1):resetIDX(2),4))]; % MAX, MEAN, STD DEV for retraction    

        
        % -----------------------------------------------------------------
        % Generate breathing data
        % -----------------------------------------------------------------                            
                                    
        if resetFlag == 1 && inVivo == 1
           breakIDX = [insertionIDX(end,2) resetIDX(1)];

            t=fixedData(breakIDX(1)+10000:breakIDX(1)+15000,1)-fixedData(1,1);
            x=fixedData(breakIDX(1)+10000:breakIDX(1)+15000,4);
         %   figure
         %   plot(t,x)

            Xfit = [ones(size(t)) cos((2*pi/6)*t)];
            s_coeffs = Xfit\x;
          %  hold on
          %  Yfit = [ones(size(t)) cos((2*pi/6)*t)]*s_coeffs;
          %  plot(t,Yfit,'r-','LineWidth',2)
          %  title(['Kidney ' int2str(i) ' Break Data with Fitted Breathing. Offset: ' num2str(s_coeffs(1)) ' Amplitude: ' num2str(s_coeffs(2))])
          %  hold off

        %        figure
        %        subplot(2,1,1)
        %        title('Break Data')
        %        plot(fixedData(breakIDX(1):breakIDX(2),1)-fixedData(1,1),fixedData(breakIDX(1):breakIDX(2),4))
        %        xlabel('Time (s)')
        %        ylabel('Force (N)')
        %        subplot(2,1,2)
        %        title('All Data')
        %        plot(fixedData(:,1)-fixedData(1,1),fixedData(:,4))
        %        xlabel('Time (s)')
        %        ylabel('Force (N)')
            KidneyDataSummary(i,10:11) = s_coeffs';
    end
end

% -------------------------------------------------------------------------
% Liver Data
% -------------------------------------------------------------------------

LiverDataSummary = zeros(length(Lfiles),11);
if inVivo == 0
    datafolder = [folder '/Liver/DATA'];
end
for i = 1:length(Lfiles)
    
        % -----------------------------------------------------------------
        % Extract important information....
        % -----------------------------------------------------------------
        data = load(strcat(strcat(datafolder,'/L'),num2str(i),'/FSENSOR-ADC.txt'));
        parameters = fopen(strcat(strcat(datafolder,'/L'),num2str(i),'/params.txt'));
        InputText=textscan(parameters,'%s','delimiter','\n'); % Read lines
        textFile=InputText{1};

        startTime = findStartTime(textFile);
        [insertionTimes insertionParams] = findInsertionTimes(textFile,startTime);
        [retractionTimes retractionParams] = findRetractionTimes(textFile,startTime);
        [stopTime resetFlag resetDist] = findStopTime(textFile, startTime, insertionTimes, insertionParams, retractionTimes, retractionParams); 

        breakStart = 0;
        breakStop = 0;
        %resetFlag = 1;
          if resetFlag == 1
              [breakStart breakStop] = findBreakStartStopTime(insertionTimes, insertionParams, retractionTimes, retractionParams,stopTime);
          end


          if i == 7 && inVivo == 1
              bias = mean(data(1:(find(fixedData(:,1)-fixedData(1,1) <= insertionTimes(j),1,'last')+1),4));
              data(:,4) = data(:,4)-bias;
          end

        % -----------------------------------------------------------------
        % Plot force data and get fixed data files (some sets have issues)
        % -----------------------------------------------------------------

        [fixedData insertionIDX retractionIDX resetIDX] = plotForceData(data, insertionTimes, insertionParams, retractionTimes, retractionParams, stopTime, breakStart, breakStop,resetFlag, resetDist);
        title(strcat('Liver Insertion  ',num2str(i)),'FontSize',12);

        % -----------------------------------------------------------------
        % Generate breathing data
        % -----------------------------------------------------------------
        
        if resetFlag == 1 && inVivo == 1
           breakIDX = [insertionIDX(end,2) resetIDX(1)];

            t=fixedData(breakIDX(1)+10000:breakIDX(1)+16000,1)-fixedData(1,1);
            x=fixedData(breakIDX(1)+10000:breakIDX(1)+16000,4);
            %f = 1000;
            %[b,a]= butter(10,10/(f/2));
            %xfiltered = filtfilt(b,a,x);

            ps = 0;
            if i == 2 && inVivo == 1
                ps = 1.5;
            end

            if i == 8 && inVivo == 1
                ps = 1.75;
            end
         %   figure
         %   plot(t,x)

            Xfit = [ones(size(t)) cos((2*pi/6)*(t-ps))];
            s_coeffs = Xfit\x;
         %   hold on
         %   Yfit = [ones(size(t)) cos((2*pi/6)*(t-ps))]*s_coeffs;
         %   plot(t,Yfit,'r-','LineWidth',2)
         %   title(['Liver ' int2str(i) ' Break Data with Fitted Breathing. Offset: ' num2str(s_coeffs(1)) ' Amplitude: ' num2str(s_coeffs(2))])
         %   hold off
            LiverDataSummary(i,10:11) = s_coeffs';
    %        figure
    %        subplot(2,1,1)
    %        title('Break Data')
    %        plot(fixedData(breakIDX(1):breakIDX(2),1)-fixedData(1,1),fixedData(breakIDX(1):breakIDX(2),4))
    %        xlabel('Time (s)')
    %        ylabel('Force (N)')
    %        subplot(2,1,2)
    %        title('All Data')
    %        plot(fixedData(:,1)-fixedData(1,1),fixedData(:,4))
    %        xlabel('Time (s)')
    %        ylabel('Force (N)')
        end

        % -----------------------------------------------------------------
        % Generate summary data
        % -----------------------------------------------------------------
        
        dist = 0;
        if retractionIDX(1,1) == 0
            for j=1:length(insertionIDX(:,1))
                dist = dist + insertionIDX(j,3);
                if dist >= insertionLimit(3)
                    insertionLimitIDX = insertionIDX(j,2);
                    j = length(insertionIDX);
                end
            end
             LiverDataSummary(i,1:9) = [max(fixedData(1:insertionIDX(end,2),4)) ...
                                        mean(fixedData(1:insertionIDX(end,2),4)) ...
                                        std(fixedData(1:insertionIDX(end,2),4)) ...
                                        max(fixedData(1:insertionLimitIDX,4)) ...
                                        mean(fixedData(1:insertionLimitIDX,4)) ...
                                        std(fixedData(1:insertionLimitIDX,4)) ...
                                        min(fixedData(resetIDX(1):resetIDX(2),4)) ...
                                        mean(fixedData(resetIDX(1):resetIDX(2),4)) ...
                                        std(fixedData(resetIDX(1):resetIDX(2),4))]; % MAX, MEAN, STD DEV for retraction    
        else
            LDS1 = [fixedData(1:insertionIDX(2,1),4) ;fixedData(insertionIDX(4,1):insertionIDX(end,2),4)]; % FOR 8 CM
            LDS2 = [fixedData(1:insertionIDX(2,1),4) ;fixedData(insertionIDX(4,1):insertionIDX(4,2),4)]; % FOR FIRST 7 CM

            LiverDataSummary(i,1:9) = [max(LDS1) ...
                                       mean(LDS1) ...
                                       std(LDS1) ...
                                       max(LDS2) ...
                                       mean(LDS2) ...
                                       std(LDS2) ...
                                       min(fixedData(resetIDX(1):resetIDX(2),4)) ...
                                       mean(fixedData(resetIDX(1):resetIDX(2),4)) ...
                                       std(fixedData(resetIDX(1):resetIDX(2),4))]; % MAX, MEAN, STD DEV for retraction    
        end  
end
% =========================================================================
% END FUNCTION: ForcePlots()
% =========================================================================
