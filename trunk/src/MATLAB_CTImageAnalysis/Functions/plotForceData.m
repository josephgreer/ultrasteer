% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: [fixedData insertionIDX retractionIDX resetIDX] = plotForceData(data, insertionTimes, insertionParams,retractionTimes, retractionParams, stopTime,breakStart, breakStop,resetFlag, resetDist)
% -------------------------------------------------------------------------
% Inputs: xxx
% Outputs: xxxx
% Sub_Functions Called: xxx
% Synopsis:
%   This function will xxxxx
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: plotForceData()
% =========================================================================

function [fixedData insertionIDX retractionIDX resetIDX] = plotForceData(data, insertionTimes, insertionParams,retractionTimes, retractionParams, stopTime,breakStart, breakStop,resetFlag, resetDist)
    
    % FIX GLITCH IN DATA!
    diffData = diff(data(:,1));                     % there are some weird jumps that need to be fixed. Need to figure out what the problem is with the code. 
    fixedData = data;
    fixedData(:,4) = -1*fixedData(:,4);
    
    hourGlitch = find(diffData < -3.59e3);
    if ~isempty(hourGlitch)
        fixedData(hourGlitch+1:length(diffData)+1) = fixedData(hourGlitch+1:length(diffData)+1)+3.6e3;
    end
    
    glitchIDX = find(diffData < -0.9);
    for i = 1:length(glitchIDX)
        fixedData(glitchIDX(i)+1) = fixedData(glitchIDX(i)+1)+1;
    end
    
    minData = min(fixedData(:,4));

    %plot(data(:,1)-data(1,1),-data(:,4));
    figure('Units','inches','Position', [5 5 10 6]);
    set(gca,'FontSize',16,'FontWeight','demi');


    insertionIDX = zeros(length(insertionTimes),3);
    retractionIDX = zeros(length(retractionTimes),3);
    resetIDX = zeros(1,3);
    
    yvec = [minData-0.25 minData-0.25 8  8];

    %hold on
    if insertionTimes ~= 0
        for j =1:length(insertionTimes)
            previousDist = 0;
            dist = textscan(char(insertionParams{j,1}),'%f');
            vel = textscan(char(insertionParams{j,2}),'%f');
            h=patch([insertionTimes(j) insertionTimes(j)+(dist{1}/vel{1}) insertionTimes(j)+(dist{1}/vel{1}) insertionTimes(j)],yvec,'c'); %'Color','r','FaceAlpha',0.5
            set(h,'FaceAlpha',0.2, 'EdgeColor','c','EdgeAlpha',0.2,'BackFaceLighting','reverselit');
            %set(gca,'Layer','bottom');
            text(double(insertionTimes(j))+((dist{1}/vel{1})/2),double(7.3),{'Insert',char(insertionParams{j,1})},'FontSize',13,'FontWeight','bold','HorizontalAlignment','center');

            % line([insertionTimes(j), insertionTimes(j)],[minData-0.25 7],'Color','k','LineWidth',2,'LineStyle','-.')
           % line([insertionTimes(j)+(dist{1}/vel{1}) insertionTimes(j)+(dist{1}/vel{1})],[minData-0.25 7],'Color','r','LineWidth',1)
            insertionIDX(j,:) = [(find(fixedData(:,1)-fixedData(1,1) <= insertionTimes(j),1,'last')+1) (find(fixedData(:,1)-fixedData(1,1) <= (insertionTimes(j)+(dist{1}/vel{1})),1,'last')+1) (dist{1}+previousDist)];
            previousDist = previousDist + dist{1};
            % hold on
        end
    end
  
    if retractionTimes ~= 0
        for j =1:length(retractionTimes)
            dist = textscan(char(retractionParams{j,1}),'%f');
            vel = textscan(char(retractionParams{j,2}),'%f');
            h=patch([retractionTimes(j) retractionTimes(j)+(dist{1}/vel{1}) retractionTimes(j)+(dist{1}/vel{1}) retractionTimes(j)],yvec,'m'); %'Color','r','FaceAlpha',0.5
            set(h,'FaceAlpha',0.2, 'EdgeColor','m','EdgeAlpha',0.2);
            text(double(retractionTimes(j))+((dist{1}/vel{1})/2),double(7.3),{'Retract',char(retractionParams{j,1})},'FontSize',13,'FontWeight','bold','HorizontalAlignment','center');
            % line([retractionTimes(j), retractionTimes(j)],[minData-0.25 7],'Color','k','LineWidth',2,'LineStyle','-.')
           % line([retractionTimes(j)+(dist{1}/vel{1}) retractionTimes(j)+(dist{1}/vel{1})],[minData-0.25 7],'Color','r','LineWidth',1)
            retractionIDX(j,:) = [(find(fixedData(:,1)-fixedData(1,1) <= retractionTimes(j),1,'last')+1) (find(fixedData(:,1)-fixedData(1,1) <= (retractionTimes(j)+(dist{1}/vel{1})),1,'last')+1) dist{1}];

            %  hold on
        end
    end
    
    hold on 
    width = 1;
    ymin = minData-0.25;
    ymax = 8;
    if resetFlag == 1 && breakStart ~= 0 && breakStop ~= 0   
          dist = textscan(resetDist,'%f');
          h=patch([breakStart+5+width breakStart+5+width+(dist{1}/0.5) breakStart+5+width+(dist{1}/0.5) breakStart+5+width],yvec,'m'); %'Color','r','FaceAlpha',0.5
          set(h,'FaceAlpha',0.2, 'EdgeColor','m','EdgeAlpha',0.2);
          text(double(breakStart+5+width+((dist{1}/0.5)/2)),double(7.3),{'Retract',resetDist},'FontSize',13,'FontWeight','bold','HorizontalAlignment','center');
          % line([breakStart+5+width breakStart+5+width],[minData-0.25 7],'Color','k','LineWidth',2,'LineStyle','-.')
          % line([breakStart+5+width+(dist{1}/0.5) breakStart+5+width+(dist{1}/0.5)],[minData-0.25 7],'Color','r','LineWidth',1)
          xlim([0 breakStart+5+width+(dist{1}/0.5)+3]);
          ylim([(minData-0.25) 8]);
          BreakXAxis(fixedData(:,1)-fixedData(1,1),fixedData(:,4),breakStart,breakStop,width,ymin,ymax); % xmin xmax
          resetIDX = [find(fixedData(:,1)-fixedData(1,1) <= stopTime,1,'last') find(fixedData(:,1)-fixedData(1,1) <= (stopTime+(dist{1}/0.5)),1,'last') dist{1}];
         % ytick=get(gca,'YTick');
    else
          plot(fixedData(:,1)-fixedData(1,1),fixedData(:,4),'k');
          set(gca,'Layer','top');


          xlim([0 (stopTime+3)]);
          ylim([ymin ymax]);
    end
  
    
    
    %title()
    xlabel('Time (s)','FontSize',20)
    ylabel('Force (N)','FontSize',20)
    set(gca,'LineWidth',2)
    set(gca,'Box','on')

    hold off
    
end

% for i=1:(length(time)-1)
%     diff(i) = time(i+1)-time(i);
% end

% =========================================================================
% END FUNCTION: plotForceData()
% =========================================================================