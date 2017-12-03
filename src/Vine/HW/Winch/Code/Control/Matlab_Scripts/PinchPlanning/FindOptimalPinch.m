clear; clc; close all;

addpath(genpath('Geom2d'));

rng(1);

turnMags = unifrnd(0,90,9,1);
turnMags = [0; turnMags];
turnSpacings = unifrnd(10,150,10,1);
drawStuff = false;

nTrials = 1000;

ii = 1;

for turnMag = turnMags.'
    for turnSpacing = turnSpacings.'
        display(sprintf('Parameter %d of 100', ii));
        
        currResult.turnMag = turnMag;
        currResult.turnSpacing = turnSpacing;
        currResult.angleErrors = zeros(nTrials,1);
        
        for trial = 1:nTrials
%             display(['trial',num2str(trial)]);
            map = GenerateForestMap(12);
            
            initTheta = deg2rad(unifrnd(0,360));
            
%             if(ii == 93)
%                 save('currState','map','turnMag','turnSpacing', 'initTheta');
%             end
            
            x = [0;0;0;0;cos(initTheta);sin(initTheta)];
            y = [0;0;0;1;0];
            xs = [x(3) x(4); x(5) x(6)];
            
            if(drawStuff)
                figure(1);
                grid on;
                hold on;
                
                handles.robot = [];
                
                xlim([-500 500]);
                ylim([-500 500]);
                daspect([1 1 1]);
                grid on;
                hold on;
            
                DrawMap(map);
                
                handles = DrawRobotXs(xs,-1,handles);
            end
            
            wallIndex = -1;
            
            ls = [];
            len = RobotLength(xs);
            
            remainingLength = 500;
            
            jj = 1;
            thetas = zeros(0,2);
            while(norm(x(5:6)) < remainingLength)
                dl  = turnSpacing;
                [x, y, xs,wallIndex] = MoveRobotByDl(x, y, dl, map, thetas, len, wallIndex, xs);
                len = RobotLength(xs);
                
                if(turnMag > 0)
                    if(mod(jj,2) == 0)
                        thetas = [len -deg2rad(turnMag)];
                    else
                        thetas = [len deg2rad(turnMag)];
                    end
                end
                jj = jj+1;
                
                if(drawStuff)
                    handles = DrawRobotXs(xs,-1,handles);
                    pause(0.1);
                    display(jj);
                end
            end
            
            endTheta = atan2(x(6),x(5));
            currResult.angleErrors(trial) = rad2deg(angleDiff(initTheta,endTheta));
            
            if(drawStuff)
                close(1);
            end
        end
        save(['Results/result',num2str(ii)],'currResult');
        ii = ii+1;
    end
end

    