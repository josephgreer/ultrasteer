function distScore = GenerateErrorScore(imageScoreCorrelation, measurementError, upperScaleFactorMeasurement, measurementVariance, scoreMean, scoreVariance, nmeasurement)
    mu = [measurementVariance imageScoreCorrelation*sqrt(measurementVariance*scoreVariance); 
        imageScoreCorrelation*sqrt(measurementVariance*scoreVariance) scoreVariance]; 
    average = [measurementError scoreMean];


    nLeft = nmeasurement;
    first = true;
    while nLeft > 0
        new = mvnrnd(average, mu, nLeft);
        idxs = find(new(:,1) < 0);
        new = removerows(new, 'ind', idxs);
        idxs = find(new(:,2) < 0);
        new = removerows(new, 'ind', idxs);
        if(first)
            distScore = new;
        else
            distScore = [distScore; new];
        end
        first = false;
        nLeft = nmeasurement-size(distScore,1);
    end
    
    % scale the uppper half
    alter = find(distScore(:, 1) > measurementError);
    delta = [ones(size(alter,1), 1)*measurementError ones(size(alter, 1), 1)*scoreMean];
    distScore(alter,:) = (distScore(alter,:)-delta)*upperScaleFactorMeasurement + delta;

end