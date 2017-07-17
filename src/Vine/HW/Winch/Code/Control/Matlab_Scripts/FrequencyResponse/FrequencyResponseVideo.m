clear; clc; close all;

% idx0 = 435;

freqs = [0 0 [0.05:0.06:2.03]];
freqs = freqs(1:end-1);

paths = {'/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/FrequencyResponse/FR_17_Output.txt',...
    '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/FrequencyResponse/FR_23_Output.txt',...
    '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/FrequencyResponse/FR_31_Output.txt',...
    '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/FrequencyResponse/FR_41_Output.txt'};

lengths = [17, 23, 31, 41]-5;

breakpoints = zeros(4,3);

for jj=1:length(paths)
    path = paths{jj};
    data = load(path, '-ascii');
    
    time = linspace(1,size(data,1),size(data,1))/29.97;
    rawLights = data(:,end);
    
    % idx0 = min(find(data(:,end) > 0));
    
    chunks = {};
    chunks{1} = [];
    chunks{1}.beginIdx = min(find(data(1:end,end) == 1));
    chunks{1}.endIdx = min(find(data(chunks{1}.beginIdx:end,end) == 0))+chunks{1}.beginIdx-2;
    for i=2:length(freqs)
        chunks{i} = [];
        chunks{i}.beginIdx = min(find(data(chunks{i-1}.endIdx+1:end,end) == 1))+chunks{i-1}.endIdx;
        chunks{i}.endIdx = min(find(data(chunks{i}.beginIdx:end,end) == 0))+chunks{i}.beginIdx-2;
    end
    
    % p1s = [data(:,1) data(:,2)];
    % p2s = [data(:,3) data(:,4)];
    %
    % deltas = p1s-p2s;
    % deltas = num2cell(deltas,2);
    %
    % tanFun = @(x)(-atan2(x(1),x(2)));
    % thetas = rad2deg(wrapToPi(cellfun(tanFun,deltas)));
    
    thetas = data(:,end-1);
    
    magnitudes = zeros(length(freqs),1);
    phases = zeros(length(freqs),1);
    
    for i=1:length(freqs)
        bIdx = chunks{i}.beginIdx;
        eIdx = chunks{i}.endIdx;
        t0 = time(bIdx);
        cThetas = thetas(bIdx:eIdx);
        cThetas = (cThetas-mean(cThetas));
        cTime = time(bIdx:eIdx);
        refSignal = cos(2*pi*freqs(i)*(cTime-t0));
        
        [acor,lag] = xcorr(refSignal,cThetas);
        
        mag = 2*rms(cThetas);
        
        zeroPoint = find(lag == 0);
        period = 1/freqs(i);
        periodsPerSample = (1/29.97)/period;
        if periodsPerSample > 0
            lookDist = min(ceil(0.5/periodsPerSample),zeroPoint-1);
            
            [~,I] = max(abs(acor(zeroPoint-lookDist:zeroPoint)));
            I = I+zeroPoint-lookDist-1;
            lagDiff = lag(I);
            lagDiff = lagDiff/29.97;
            phases(i) = rad2deg(2*pi*lagDiff/period);
        else
            lookDist = zeroPoint-1;
            phases(i) = 0;
        end
        
        magnitudes(i) = mag;
        
        
        plotTheData = false;
        if plotTheData
            plotyy(cTime,cThetas,cTime,refSignal);
            hold on;
            figure;
            plot(cTime,refSignal);
            hold on;
            plot(cTime,cThetas);
            shiftedCThetas = cThetas(-lagDiff+1:end);
            plot(cTime(1:length(shiftedCThetas)),shiftedCThetas);
        end
    end
    
    freqs(1) = 1e-2;
    freqs(2) = 1e-2;
    magnitudes(1) = magnitudes(3);
    magnitudes(2) = magnitudes(3);
    magnitudes = deg2rad(magnitudes)/lengths(jj);
    
    breakpoint = min(find(magnitudes < 1/sqrt(2)*magnitudes(1)));
    breakpoints(jj,1) = freqs(breakpoint);
    breakpoints(jj,2) = magnitudes(breakpoint)/magnitudes(1);
    breakpoints(jj,3) = phases(breakpoint);
    
    figure(1);
    subplot(2,1,1);
    plot(freqs,magnitudes/magnitudes(1),'LineWidth',4);
    box on;
    hold on;
    set(gca,'xscale','log');
    set(gca,'yscale','log');
    xlim([0 2]);
    ylim([0.0316 1.3]);
    subplot(2,1,2);
    plot(freqs,phases,'LineWidth',4);
    box on;
    hold on;
    set(gca,'xscale','log');
    xlim([0 2]);
    ylim([-90 0]);
end

subplot(2,1,1);
grid on;
legend(cellstr(num2str(round(lengths'*2.54), 'L=%d cm')));
ylabel('\kappa/\kappa_{DC}');
subplot(2,1,2);
% legend(cellstr(num2str(round(lengths'*2.54), 'L=%d cm')));
ylabel('degree');
xlabel('Frequency (Hz)');
for jj=1:length(paths)
    subplot(2,1,1);
    scatter(breakpoints(jj,1), breakpoints(jj,2), 150, 'k', 'LineWidth', 2);
    subplot(2,1,2);
    grid on;
    scatter(breakpoints(jj,1), breakpoints(jj,3), 150, 'k', 'LineWidth', 2);
end

subplot(2,1,1);
set(gca,'FontSize',12,'FontName','Times New Roman');
subplot(2,1,2);
set(gca,'FontSize',12,'FontName','Times New Roman');
h = figure(1);
set(h, 'Position', [100 100 320 320]);
pause(1);
tightfig;
% set(gca,'yscale','log');