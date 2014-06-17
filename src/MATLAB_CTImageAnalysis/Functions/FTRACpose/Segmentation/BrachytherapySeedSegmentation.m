% $Id: BrachytherapySeedSegmentation.m,v 1.10 2007/12/03 21:48:18 anton Exp $
% //-----------------------------------------------------------------------------
% // function:	BrachytherapySeedSegmentation() 
% // file:		BrachytherapySeedSegmentation.m
% // author:		siddharth
% // date:		
% // description:	function that performs the seed segmentation, in the
% following steps:
%                  1) Perform morphological top hat transform for basic
%                  segmentation
%                  2)Threshold the morphologically processed image
%                  3) clear border associated noise,
%                  4) Label, identify objects/regions
%                  5) classify regions/objects into either single seeds or
%                  multiple seed cluster, in which case, determine the
%                  number of seeds within the cluster,
%                  6)After all classification, extract single seeds out of determined 
%                   multiple seed clusters
%                  7) Record the results
% // input:	
%           1) I (image buffer for region of interest)
%           2) bUseManualThreshold: flag that decides whether to use
%           externally provided threshold or calculate threshold
%           automatically
%           3)iMorphThreshold: external threshold value, used only if
%           bUseManualThreshold is true
% 
% // output: 
% 
% 1) Final Information Data set in the form
% final data set: [CGx CGy Orientation OrientationValidity PartOfCluster]
% 2) iCurrentValueOfThreshold: the threshold value that was used
% 
% // calling function: 1) SeedSelectROIButton_Callback, 2) Threshold_Callback
% 
% // NOTE: Two important & sensitive parts of the algorithm are
%          1) structuring element :  determines the shape & size of the object extracted, 
%           this should be of similar shape as the object sought & a little bigger in size, 
%           in way that it completely engulfs the object of interest. Also
%           important to note that, noise which is similar in shape &
%           smaller in size will also show up in final image.
%          2) threshold : the threshold value which will be used to
%          threshold the morphologically processed image. If the threshold
%          is too high than parts of the object might get lost, this is
%          because all parts of the object after morphological processing
%          are not of same gray value, since its grey level top-hat.; on
%          the other hand if the threshold is too low, then, some noise may
%          show up as object. So one has to be very careful while doing
%          thresholding. 
%          An automatic thresholding algorithm has been written which a
%          slight modification of the thresholding by entropy minimization.
%          However, it has also been found with the experience of the
%          images so far, that the value 20 is a reasonably good threshold
%          value.
%       Very important to know, that the coordinate system of the MATLAB
%       display or values returned/rquired by MATLAB internal functions
%       have a coordinate system which is reversed i.e. x & y are switched.
%       In other words, plot (x, y,..) (MATLAB function) will correspond to
%       I(y,x) for accessing our image.
%       Last but not the least, the results of this algorithm i.e seed
%       locations are in the coordinate system of the region of interest.
%       i.e. these will be converted to image coordinate system (by adding
%       translation vector), when user clicks 'Accept results' button on
%       GUI
% //-----------------------------------------------------------------------------
function [FinalInformationDataSet, iCurrentThreshold] = BrachytherapySeedSegmentation (I, bUseManualThreshold, iMorphThreshold, resRatioToNTSC)
%read the image on to a matrix buffer
% I = imread(InputFilename);

%since the seeds are dark in the image, we need to invert the image
Ineg = imcomplement(I);

%convert uint into double
InegD = double(Ineg);

% shorter notation for resolution ratio
resRatio = resRatioToNTSC;
resRatio2 = resRatio * resRatio;

% Step 1: Morphological Segmentation

% perform the morphological top hat operation
% a) perform opening with 5x5 square
filterSize = round(5 * resRatio);
Iop = imopen(Ineg, strel('rectangle', [filterSize filterSize]));
% This would be for iodine: Iop = imopen(Ineg, strel('rectangle', [13 7]));

% b) subtract from the original image to get the final gray scale image
Iopth = imsubtract(Ineg, Iop);


%  Step 2: Threshold the image

% threshold the image & convert into binary
% Iopthbin = Iopth >= iMorphThreshold;
if(bUseManualThreshold)
    iCurrentThreshold = iMorphThreshold;
else
    %  perform automatic thresholding
    iAutoThresh = FindEntropyThreshold(Iopth);

    iCurrentThreshold = iAutoThresh;
end


Iopthbin = Iopth >= iCurrentThreshold;
% clear the borders of border associated noise
Iopthbin = imclearborder(Iopthbin); 
% perform the region labelling operation with 8-connectivity
[IbinLab, nSeeds] = bwlabel(Iopthbin, 8);
% display the original inverted image(seeds bright)
% figure, imshow(Ineg), title('Original inverted image')
% display the morphologically segmented image
% figure, imshow(IbinLab), title('Morphologically Segmented Image');

% clear few variables
clear Iop Iopthbin;



%  Step 3: move on to the step of classifying single seeds and clusters
KLab = IbinLab;

% final data set: [CGx CGy Orientation OrientationValidity PartOfCluster]
FinalInformationDataSet = []; 

nNewSeeds = nSeeds;

if (nNewSeeds > 0)
    
    % get the region statistics/properties
    stats  = regionprops(KLab, 'area', 'centroid', 'majoraxislength', 'minoraxislength', 'boundingbox', 'orientation');

    % extract the areas out of the stats structure
    Areas = cat(1,stats.Area);

    %  extract the centroid out of the stats structure
    Centroids = cat(1, stats.Centroid);

    % extract the Major axis out of the stats structure
    MajorAxisLengths = cat(1, stats.MajorAxisLength);

    % extract the Minor axis out of the stats structure
    MinorAxisLengths = cat(1, stats.MinorAxisLength);

    % extract the bounding box out of the stats structure
    InitialBoundingBox = cat(1, stats.BoundingBox);

    % extract the orienations out of the stats structure
    Orientations = cat(1, stats.Orientation);


    Perimeter = [];

    %  calculate perimeter for each region & statistical distribution
    for iRegionNumber = 1:nNewSeeds
        IperiRegion = (KLab == iRegionNumber);
        IperiRegion = bwperim(IperiRegion);
        p = sum(sum(IperiRegion > 0));
        Perimeter = [Perimeter;p];
    end

    clear IperiRegion;




    % buffers for initial classfication

    % buffer for suspected seed clusters
    Kcluster = zeros (size(Ineg));

    % buffer for single seeds
    Ksingles = zeros (size(Ineg));

    % buffer for uncertain seeds
    Kuncertain = zeros(size(Ineg));


    % initialize an array which identifies whether seed has been classified yet
    % or not 0: unclassified; 1: single seed; 2: multiple seed cluster

    % initially all seeds are unclassified
    SeedClassificationArray = [];

    for i= 1:nNewSeeds
        SeedClassificationArray (i) = 0; 
        IndicesOfUnclassifiedSeeds (i) = i;
    end

    % array for maintaining region numbers of suspected clusters & estimated
    % number of seeds within the cluster
    ClusterRegionAndSeedsInformation = [];


    NumOfUnclassifiedItems = sum(SeedClassificationArray == 0);


    % calculate quantiles, averages, standard deviations

    [AvgLength LengthDeviation] = CalculateLengthStats(MajorAxisLengths, IndicesOfUnclassifiedSeeds);
    [AvgArea AreaDeviation] = CalculateAreaStats(Areas, IndicesOfUnclassifiedSeeds);
    [AvgPerimeter PerimeterDeviation] = CalculatePerimeterStats(Perimeter, IndicesOfUnclassifiedSeeds);
    [AvgWidth WidthDeviation] = CalculateWidthStats(MinorAxisLengths, IndicesOfUnclassifiedSeeds);

    Length95 = prctile(MajorAxisLengths, 95);
    Width95 = prctile(MinorAxisLengths, 95);
    Area95 = prctile(Areas, 95);
    Perimeter95 = prctile(Perimeter, 95);


    % LOGIC:
    % We are going with the assumption that most of the seeds are single seeds &
    % there are at least 5% population as clusters
    % The classification step is a two pass procedure, initially during the
    % first pass, the seeds having the metrics (Area, length, width, perimeter)
    % abnormally larger, i.e. metrics larger than 95% of the total population
    % are marked as suspected clusters, and the seeds whose metrics are well
    % within the average of the population (the assumption that the more than
    % 90% of the seeds segmented are single seeds) provides the basis the
    % average metrics represent the average metrics of a single seed,
    % therefore, for any seed whose metrics are well within the average are
    % marked as single seeds. All the remaining seeds which could not satisfy
    % the two extreme criteria are marked as suspected(left unclassified) to be
    % further probed
    % in the second pass, further information is utilized by employing more
    % images: various images considered are:
    % a) image segmented by left tilt 5x3 structuring element
    % b) image segmented by right tilt 5x3 structuring element
    % c) image created by running shrink operator on the morphologically
    % segmented image
    % d) image created by going back to original image in the areas
    % morphologically segmented to erode less brighter pixels, called 80
    % prctile image
    % All the above 4 images created may help in separating out clusters, by
    % yielding more than one object in the suspected uncertain region
    % In case, which would happen only if the suspected region is a cluster,
    % when the number of objects are more than one, then that suspected region
    % is classified as a cluster
    % However, it may also happen that none of the 4 images are able to
    % separate out the cluster or actually the suspected region may just be a
    % single seed extraordinarily segmented. In this case metrics are used with
    % 80 prctile, image, In the 80 prctile image, all the single seeds are
    % reduced to the size of 1-3 pixels in area, more importantly fairly
    % compact structures, the clusters which were not separated are most
    % probably presented as less compact structures. As a result, the area of
    % the bounding box of a probable cluster is much greater than the bounding
    % box area of a single seed (since they show up as compact), thus this is
    % then used as a classifiying step
    % All the above mentioned logic ensures that no single seed shows up as a
    % cluster in the final segmented image i.e. 'spurios seed', which is the requirement for the
    % ultimate step of MARSHAL algorithm. However, it is quite possible that a
    % cluster may be classified as single seed, resulting in 'hidden seed'
    % Note: it is also estimated in this step of classification, that
    % approximately how many single seeds are there in the classified cluster,
    % this is an important input for the next step of extracting single seed
    % out of cluster




    % Step: first pass of classification
    % as discussed above if the metrics are above 95% population, then it is
    % classified as a cluster, if not then a scoring is done, if the metrics
    % are well within the avg. metrics, then it is scored for being a single
    % seed, if metrics are greater than avg + 2*deviation then scored for 
    % multiple seed cluster, otherwise it is scored for being uncertain region
    % at the end scores are checked & classification is done accordingly.
    % if a region is classified as a cluster, then its metrics are compared
    % with avg single seed metrics to estimate the number of single seeds
    % within the cluster
        for iRegionNumber = 1:nNewSeeds
        %     if the CG pixel is black, then definitely a cluster
        %     else if not
        %     compare areas, lengths etc
        % get the region centroid
            if (SeedClassificationArray(iRegionNumber) == 0)   
                Iregion = (KLab == iRegionNumber);

    %             figure, imshow(Iregion);

                iRegionSingleProbableScore = 0;
                iRegionClusterProbableScore = 0;
                iRegionUncertainityScore = 0;

                iApproxSeeds = 0;

                RegionOrientation = Orientations (iRegionNumber);

                RegionCentroid = round(Centroids(iRegionNumber,:));    
                yCG = RegionCentroid(1,1);
                xCG = RegionCentroid(1,2);

                yRegionCorner = round(InitialBoundingBox(iRegionNumber, 1));
                xRegionCorner = round(InitialBoundingBox(iRegionNumber, 2));
                yRegionWidth = round(InitialBoundingBox(iRegionNumber, 3));
                xRegionWidth = round(InitialBoundingBox(iRegionNumber, 4));


                SumXK = 0;
                SumYK = 0;
                SumK = 0;
                for iYIndex = yRegionCorner : yRegionCorner + yRegionWidth - 1
                    for iXIndex = xRegionCorner : xRegionCorner + xRegionWidth - 1
                        SumXK = SumXK + iXIndex*InegD(iXIndex, iYIndex);
                        SumYK = SumYK + iYIndex*InegD(iXIndex, iYIndex);
                        SumK = SumK + InegD(iXIndex, iYIndex);
                    end
                end

                xActualCG = round(SumXK/SumK);
                yActualCG = round(SumYK/SumK);
                RegionPerimeter = Perimeter(iRegionNumber);
                RegionArea = Areas (iRegionNumber);
                RegionMajAxisLength = MajorAxisLengths (iRegionNumber);
                RegionMinAxisLength = MinorAxisLengths (iRegionNumber);


                % perform classification algorithm    

                % as discussed above if the metrics are above 95% population, then it is
                % classified as a cluster, if not then a scoring is done, if the metrics
                % are well within the avg. metrics, then it is scored for being a single
                % seed, if metrics are greater than avg + 2*deviation then scored for 
                % multiple seed cluster, otherwise it is scored for being uncertain region
                % at the end scores are checked & classification is done accordingly.
                % if a region is classified as a cluster, then its metrics are compared
                % with avg single seed metrics to estimate the number of single seeds
                % within the cluster
                if( (KLab(xCG,yCG) == 0) || ((RegionMajAxisLength > Length95) && (RegionPerimeter > Perimeter95) )|| ((RegionArea > Area95) && (RegionMinAxisLength > Width95) && (RegionPerimeter > Perimeter95)) || ((RegionArea > Area95) && (RegionMajAxisLength > Length95)))
                    Kcluster = Kcluster + Iregion;
                    IndicesOfUnclassifiedSeeds (iRegionNumber) = 0;
                    SeedClassificationArray(iRegionNumber) = 2;            
                    NumOfUnclassifiedItems = NumOfUnclassifiedItems - 1;
                    if( ((RegionMajAxisLength/AvgLength >= (3.75 * resRatio)) && (RegionMajAxisLength/AvgLength < (4.5 * resRatio))) || ((RegionArea/AvgArea >= (3.75 * resRatio2)) && (RegionArea/AvgArea < (4.5 * resRatio2))) || ((RegionMinAxisLength/AvgWidth >= (3.75 * resRatio)) && (RegionMinAxisLength/AvgWidth < (4.5 * resRatio))))
                       iApproxSeeds = 4;
                    else
                       if( ((RegionMajAxisLength/AvgLength >= (3 * resRatio)) && (RegionMajAxisLength/AvgLength < (3.75 * resRatio))) || ((RegionArea/AvgArea >= (3 * resRatio2)) && (RegionArea/AvgArea < (3.75 * resRatio2))) || ((RegionMinAxisLength/AvgWidth >= (3 * resRatio)) && (RegionMinAxisLength/AvgWidth < (3.75 * resRatio))))
                            iApproxSeeds = 3;
                        else
                            iApproxSeeds = 2;
                        end
                    end
                    ClusterRegionAndSeedsInformation = [ClusterRegionAndSeedsInformation; iRegionNumber iApproxSeeds];
                else
                    if (RegionMajAxisLength > AvgLength + 2*LengthDeviation) 
                        iRegionClusterProbableScore = iRegionClusterProbableScore + 1;
                    else 
                        if (RegionMajAxisLength <= AvgLength + LengthDeviation)
                            iRegionSingleProbableScore = iRegionSingleProbableScore + 1;
                        else              
                            iRegionUncertainityScore = iRegionUncertainityScore + 1;
                        end
                    end


                    if (RegionArea > AvgArea + 2*AreaDeviation)
                        iRegionClusterProbableScore = iRegionClusterProbableScore + 1;
                    else 
                        if (RegionArea <= AvgArea + AreaDeviation)
                            iRegionSingleProbableScore = iRegionSingleProbableScore + 1;
                        else
                            iRegionUncertainityScore = iRegionUncertainityScore + 1;
                        end
                    end


                    if (RegionPerimeter > AvgPerimeter + 2*PerimeterDeviation) 
                            iRegionClusterProbableScore = iRegionClusterProbableScore + 1;
                    else 
                        if (RegionPerimeter <= AvgPerimeter + PerimeterDeviation)
                            iRegionSingleProbableScore = iRegionSingleProbableScore + 1;
                        else              
                            iRegionUncertainityScore = iRegionUncertainityScore + 1;                    
                        end
                    end


                    if (RegionMinAxisLength > AvgWidth + 2*WidthDeviation)
                        iRegionClusterProbableScore = iRegionClusterProbableScore + 1;
                    else 
                        if (RegionMinAxisLength <= AvgWidth + WidthDeviation)
                            iRegionSingleProbableScore = iRegionSingleProbableScore + 1;
                        else              
                            iRegionUncertainityScore = iRegionUncertainityScore + 1;
                        end
                    end


                        if (iRegionClusterProbableScore >=3)
                            Kcluster = Kcluster + Iregion;
                            IndicesOfUnclassifiedSeeds (iRegionNumber) = 0;
                            SeedClassificationArray(iRegionNumber) = 2;            
                            NumOfUnclassifiedItems = NumOfUnclassifiedItems - 1;              
                            if( ((RegionMajAxisLength/AvgLength >= (3.75 * resRatio)) && (RegionMajAxisLength/AvgLength < (4.5 * resRatio))) || ((RegionArea/AvgArea >= (3.75 * resRatio2)) && (RegionArea/AvgArea < (4.5 * resRatio2))) || ((RegionMinAxisLength/AvgWidth >= (3.75 * resRatio)) && (RegionMinAxisLength/AvgWidth < (4.5 * resRatio))))
                               iApproxSeeds = 4;
                            else
                               if( ((RegionMajAxisLength/AvgLength >= (3 * resRatio)) && (RegionMajAxisLength/AvgLength < (3.75 * resRatio))) || ((RegionArea/AvgArea >= (3 * resRatio2)) && (RegionArea/AvgArea < (3.75 * resRatio2))) || ((RegionMinAxisLength/AvgWidth >= (3 * resRatio)) && (RegionMinAxisLength/AvgWidth < (3.75 * resRatio))))
                                    iApproxSeeds = 3;
                                else
                                    iApproxSeeds = 2;
                                end
                            end
                            ClusterRegionAndSeedsInformation = [ClusterRegionAndSeedsInformation; iRegionNumber iApproxSeeds];                        
                        else
                            if( ((iRegionSingleProbableScore >=3) && (iRegionClusterProbableScore == 0)))
                                Ksingles = Ksingles + Iregion;
                                IndicesOfUnclassifiedSeeds (iRegionNumber) = 0;
                                SeedClassificationArray(iRegionNumber) = 1;            
                                NumOfUnclassifiedItems = NumOfUnclassifiedItems - 1;              
                                FinalInformationDataSet = [FinalInformationDataSet; xActualCG yActualCG RegionOrientation 1 0];
                            else
                                Kuncertain = Kuncertain + Iregion;

                            end
                        end



                end

            end


        end

    % figure, imshow(Kcluster), title('Identified certainly clusters at first discrimination');
    % figure, imshow(Kuncertain), title('Uncertain region being treated for further analysis');



    % second pass of the classification step, dealing with uncertain regions:

    % in the second pass, further information is utilized by employing more
    % images: various images considered are:
    % a) image segmented by left tilt 5x3 structuring element
    % b) image segmented by right tilt 5x3 structuring element
    % c) image segmented by left tilt 3x3 structuring element
    % d) image segmented by right tilt 3x3 structuring element
    % e) image created by running shrink operator on the morphologically
    % segmented image
    % f) image created by going back to original image in the areas
    % morphologically segmented to erode less brighter pixels, called 80
    % prctile image
    % All the above 6 images created may help in separating out clusters, by
    % yielding more than one object in the suspected uncertain region
    % In case, which would happen only if the suspected region is a cluster,
    % when the number of objects are more than one, then that suspected region
    % is classified as a cluster
    % However, it may also happen that none of the 6 images are able to
    % separate out the cluster or actually the suspected region may just be a
    % single seed extraordinarily segmented. In this case metrics are used with
    % 80 prctile, image, In the 80 prctile image, all the single seeds are
    % reduced to the size of 1-3 pixels in area, more importantly fairly
    % compact structures, the clusters which were not separated are most
    % probably presented as less compact structures. As a result, the area of
    % the bounding box of a probable cluster is much greater than the bounding
    % box area of a single seed (since they show up as compact), thus this is
    % then used as a classifiying step
    % All the above mentioned logic ensures that no single seed shows up as a
    % cluster in the final segmented image i.e. 'spurios seed', which is the requirement for the
    % ultimate step of MARSHAL algorithm. However, it is quite possible that a
    % cluster may be classified as single seed, resulting in 'hidden seed'
    % Note: it is also estimated in this step of classification, that
    % approximately how many single seeds are there in the classified cluster,
    % this is an important input for the next step of extracting single seed
    % out of cluster
    if( NumOfUnclassifiedItems > 0)

    %    reshape the array containing indices of unclassified seeds to weed out   
         IndicesOfUnclassifiedSeedsNew = [];

         for i= 1: nNewSeeds
            if(SeedClassificationArray(i) == 0)
               IndicesOfUnclassifiedSeedsNew = [IndicesOfUnclassifiedSeedsNew; i];
            end
         end




        % arrive at right tilt image
        IrightTilt = (imsubtract(Ineg, imopen(Ineg, strel([0 0 1 1 1; 0 0 1 1 1; 1 1 1 1 1; 1 1 1 0 0;1 1 1 0 0]))) > 20);
        % arrive at left tilt image
        IleftTilt = (imsubtract(Ineg, imopen(Ineg, strel([1 1 1 0 0; 1 1 1 0 0; 1 1 1 1 1; 0 0 1 1 1; 0 0 1 1 1]))) > 20);

        % arrive at small (3x3) right tilt image
        IrightSmallTilt = (imsubtract(Ineg, imopen(Ineg, strel([0 1 1;1 1 1; 1 1 0]))) > 10);

        % figure, imshow(IrightSmallTilt);

        % arrive at small (3x3) left tilt image
        IleftSmallTilt = (imsubtract(Ineg, imopen(Ineg, strel([1 1 0; 1 1 1;0 1 1]))) > 10);

        % figure, imshow(IleftSmallTilt);


        %80 prctile image
        I80 = zeros (size(InegD));
        for iRegionNumber = 1:nNewSeeds

            Ireg = (KLab == iRegionNumber);

            yRegionCorner = round(InitialBoundingBox(iRegionNumber, 1));
            xRegionCorner = round(InitialBoundingBox(iRegionNumber, 2));
            yRegionWidth = round(InitialBoundingBox(iRegionNumber, 3));
            xRegionWidth = round(InitialBoundingBox(iRegionNumber, 4));

            OriginalRegionArea = Areas(iRegionNumber);

                            % for 80 prctile image

                            v = reshape(InegD(xRegionCorner:xRegionCorner + xRegionWidth - 1, yRegionCorner:yRegionCorner + yRegionWidth - 1), prod(size(InegD(xRegionCorner:xRegionCorner + xRegionWidth - 1, yRegionCorner:yRegionCorner + yRegionWidth - 1))),1);

                            vNew = [];

                            for i= 1: length(v)
                               if(v(i) > 0)
                                   vNew = [v(i); vNew];
                               end
                            end

                            iThresh80 = prctile(vNew, 80);

                            Itemp = ((InegD >= iThresh80).*InegD);

            NewRegionArea = sum(sum(Itemp(xRegionCorner:xRegionCorner + xRegionWidth, yRegionCorner:yRegionCorner + yRegionWidth) > 0));   

            if (NewRegionArea > 5*OriginalRegionArea)
                hello = 0;
            else
                I80(xRegionCorner:xRegionCorner + xRegionWidth - 1, yRegionCorner:yRegionCorner + yRegionWidth - 1) = Itemp(xRegionCorner:xRegionCorner + xRegionWidth -1, yRegionCorner:yRegionCorner + yRegionWidth -1);      
            end
        end
        I80 = I80 > 0;

        % figure, imshow(I80);
        I80Lab = bwlabel(I80, 8);
        Ishrink = bwmorph(KLab, 'shrink');
        I80RegionStats = regionprops(I80Lab, 'majoraxislength', 'minoraxislength', 'filledarea');
        I80RegionLengths = cat(1, I80RegionStats.MajorAxisLength);
        I80RegionWidths = cat(1, I80RegionStats.MinorAxisLength);
        I80RegionFilledAreas = cat(1, I80RegionStats.FilledArea);

        %  metrics of 80 prctile image
        AvgLengthOfI80 = mean(I80RegionLengths);
        AvgFilledAreaOfI80 = mean(I80RegionFilledAreas);
        FilledAreaDeviationOfI80 = std (I80RegionFilledAreas);
        Length95OfI80 = prctile(I80RegionLengths, 95);
        LengthDeviationOfI80 = std(I80RegionLengths);
        AvgWidthofI80 = mean(I80RegionWidths);
        Width95OfI80 = prctile(I80RegionWidths, 95);
        WidthDeviationOfI80 = std(I80RegionWidths);



        for iIndexOfUnclassifiedSeed = 1:NumOfUnclassifiedItems
        %     if the CG pixel is black, then definitely a cluster
        %     else if not
        %     compare areas, lengths etc
        % get the region centroid
                iRegionSingleProbableScore = 0;
                iRegionClusterProbableScore = 0;
                iRegionNumber = IndicesOfUnclassifiedSeedsNew(iIndexOfUnclassifiedSeed);
                Iregion = (KLab == iRegionNumber);
                IrightTiltLabel = [];
                IleftTiltLabel= [];
                ItempLab = [];
                Itemp = [];
                RegionOrientation = Orientations (iRegionNumber);
                RegionCentroid = round(Centroids(iRegionNumber,:));    
                yCG = RegionCentroid(1,1);
                xCG = RegionCentroid(1,2);
                yRegionCorner = round(InitialBoundingBox(iRegionNumber, 1));
                xRegionCorner = round(InitialBoundingBox(iRegionNumber, 2));
                yRegionWidth = round(InitialBoundingBox(iRegionNumber, 3));
                xRegionWidth = round(InitialBoundingBox(iRegionNumber, 4));

                SumXK = 0;
                SumYK = 0;
                SumK = 0;
                for iYIndex = yRegionCorner : yRegionCorner + yRegionWidth - 1
                    for iXIndex = xRegionCorner : xRegionCorner + xRegionWidth - 1
                        SumXK = SumXK + iXIndex*InegD(iXIndex, iYIndex);
                        SumYK = SumYK + iYIndex*InegD(iXIndex, iYIndex);
                        SumK = SumK + InegD(iXIndex, iYIndex);
                    end
                end

                xActualCG = round(SumXK/SumK);
                yActualCG = round(SumYK/SumK);
                NumObjectsArray = [];
                iApproxSeeds = 0;

                % images: various images considered are:
                % a) image segmented by left tilt 5x3 structuring element
                % b) image segmented by right tilt 5x3 structuring element
                % c) image segmented by left tilt 3x3 structuring element
                % d) image segmented by right tilt 3x3 structuring element
                % e) image created by running shrink operator on the morphologically
                % segmented image
                % f) image created by going back to original image in the areas
                % morphologically segmented to erode less brighter pixels, called 80
                % prctile image
                % All the above 6 images created may help in separating out clusters, by
                % yielding more than one object in the suspected uncertain region
                % In case, which would happen only if the suspected region is a cluster,
                % when the number of objects are more than one, then that suspected region
                % is classified as a cluster

                [IrightTiltLabel NumOfObjectsInRightTiltImage] = bwlabel( IrightTilt(xRegionCorner:xRegionCorner + xRegionWidth -1, yRegionCorner:yRegionCorner + yRegionWidth -1), 8);
                [IleftTiltLabel NumOfObjectsInLeftTiltImage] = bwlabel( IleftTilt(xRegionCorner:xRegionCorner + xRegionWidth -1, yRegionCorner:yRegionCorner + yRegionWidth -1), 8);
                [IshrinkLabel NumOfObjectsInShrunkImage] = bwlabel( Ishrink(xRegionCorner:xRegionCorner + xRegionWidth -1, yRegionCorner:yRegionCorner + yRegionWidth -1), 8);
                [I80RegionLab NumOfObjectsIn85Image] = bwlabel( I80(xRegionCorner:xRegionCorner + xRegionWidth -1, yRegionCorner:yRegionCorner + yRegionWidth -1), 8);
                [IrightSmallTiltLabel NumOfObjectsInRightSmallImage] = bwlabel( IrightSmallTilt(xRegionCorner:xRegionCorner + xRegionWidth -1, yRegionCorner:yRegionCorner + yRegionWidth -1), 8);
                [IleftSmallTiltLabel NumOfObjectsInLeftSmallImage] = bwlabel( IleftSmallTilt(xRegionCorner:xRegionCorner + xRegionWidth -1, yRegionCorner:yRegionCorner + yRegionWidth -1), 8);

                % label the specific region
                RegionalStatOfI80 = regionprops(I80RegionLab, 'majoraxislength', 'minoraxislength', 'filledarea', 'area');
                NumObjectsArray = [NumOfObjectsInRightTiltImage; NumOfObjectsInLeftTiltImage; NumOfObjectsInShrunkImage; NumOfObjectsIn85Image;NumOfObjectsInRightSmallImage; NumOfObjectsInLeftSmallImage ];
                iApproxSeeds = max(NumObjectsArray);

                if (iApproxSeeds > 1)
                    Kcluster = Kcluster + Iregion;
                    IndicesOfUnclassifiedSeeds (iRegionNumber) = 0;
                    SeedClassificationArray(iRegionNumber) = 2;            
                    NumOfUnclassifiedItems = NumOfUnclassifiedItems - 1;              
                    ClusterRegionAndSeedsInformation = [ClusterRegionAndSeedsInformation; iRegionNumber iApproxSeeds];
                else

                    % for metrics in I80 image
                    % However, it may also happen that none of the 6 images are able to
                    % separate out the cluster or actually the suspected region may just be a
                    % single seed extraordinarily segmented. In this case metrics are used with
                    % 80 prctile, image, In the 80 prctile image, all the single seeds are
                    % reduced to the size of 1-3 pixels in area, more importantly fairly
                    % compact structures, the clusters which were not separated are most
                    % probably presented as less compact structures. As a result, the area of
                    % the bounding box of a probable cluster is much greater than the bounding
                    % box area of a single seed (since they show up as compact), thus this is
                    % then used as a classifiying step
                    % All the above mentioned logic ensures that no single seed shows up as a
                    % cluster in the final segmented image i.e. 'spurios seed', which is the requirement for the
                    % ultimate step of MARSHAL algorithm. However, it is quite possible that a
                    % cluster may be classified as single seed, resulting in 'hidden seed'
                    % Note: it is also estimated in this step of classification, that
                    % approximately how many single seeds are there in the classified cluster,
                    % this is an important input for the next step of extracting single seed
                    % out of cluster

                    RegionalLengthOfI80 = RegionalStatOfI80.MajorAxisLength;
                    RegionalWidthOfI80 = RegionalStatOfI80.MinorAxisLength;
                    RegionalFilledAreaOfI80 = RegionalStatOfI80.FilledArea;
                    RegionAreaOfI80 = RegionalStatOfI80.Area;

                    if( (RegionalFilledAreaOfI80 > AvgFilledAreaOfI80 + FilledAreaDeviationOfI80) && (RegionalWidthOfI80 > AvgWidthofI80))
                        iRegionClusterProbableScore = iRegionClusterProbableScore + 1;
                    else
                        iRegionSingleProbableScore = iRegionSingleProbableScore + 1;    
                    end

                    if( (iRegionClusterProbableScore > 0) || (iRegionSingleProbableScore > 0))
                        if(iRegionClusterProbableScore > iRegionSingleProbableScore)
                            Kcluster = Kcluster + Iregion;
                            IndicesOfUnclassifiedSeeds (iRegionNumber) = 0;
                            SeedClassificationArray(iRegionNumber) = 2;            
                            NumOfUnclassifiedItems = NumOfUnclassifiedItems - 1;              
                            ClusterRegionAndSeedsInformation = [ClusterRegionAndSeedsInformation; iRegionNumber 2];                                
                         else
                            Ksingles = Ksingles + Iregion;
                            IndicesOfUnclassifiedSeeds (iRegionNumber) = 0;
                            SeedClassificationArray(iRegionNumber) = 1;            
                            NumOfUnclassifiedItems = NumOfUnclassifiedItems - 1;              
                            FinalInformationDataSet = [FinalInformationDataSet; xActualCG yActualCG RegionOrientation 1 0];
                        end

                    end

                end

         end



    end

    % figure, imshow(Kcluster), title('Identified, finalized multiple-seed clusters in the segmented image')
    % pixval

    % figure, imshow(Ksingles), title('Identified single seeds in the segmented image')






    % Step 4: extracting single seeds from a cluster

    if (length(ClusterRegionAndSeedsInformation) > 0)
        IclusterTemp = zeros(size(Ineg));
        nClusters = length(ClusterRegionAndSeedsInformation(:,1));
        for iCluster = 1: nClusters
            iClusterRegionNumber = ClusterRegionAndSeedsInformation(iCluster,1);
            iSeedsWithinCluster = ClusterRegionAndSeedsInformation(iCluster,2);
            Iregion = (KLab == iClusterRegionNumber);
            IclusterTemp = [];

            %   find the bounding region to look for
            yClusterRegionCorner = round(InitialBoundingBox(iClusterRegionNumber, 1));
            xClusterRegionCorner = round(InitialBoundingBox(iClusterRegionNumber, 2));
            yClusterRegionWidth = round(InitialBoundingBox(iClusterRegionNumber, 3));
            xClusterRegionWidth = round(InitialBoundingBox(iClusterRegionNumber, 4));

           ClusterCG = round(Centroids(iClusterRegionNumber,:));    
           yClusterCG = ClusterCG(1,1);
           xClusterCG = ClusterCG(1,2);

           %   slope of the line
            OriginalMajAxisOrientation = Orientations(iClusterRegionNumber);
            ClusterMajorAxisLength = MajorAxisLengths(iClusterRegionNumber);
            
            %   get the translation vector
            xFrame = ClusterMajorAxisLength/2 * sin (-OriginalMajAxisOrientation* pi/180);
            yFrame = ClusterMajorAxisLength/2 * cos (-OriginalMajAxisOrientation* pi/180);
            xTranslation = xClusterCG - xFrame;
            yTranslation = yClusterCG - yFrame;

            % Break along the minor axis/clear the pixels along the minor axis   
            %  from the approximate number of seeds, arrive at the different
            %  cut-sections, effective different points on the major axis, through
            %  which the cut planes will pass
            NumOfCutPlanes = iSeedsWithinCluster - 1;

            for i=1:NumOfCutPlanes
                % Steps 1) determine the pixels on the minor axis
                %   find the equation of the line
                %   point on the line
                PointXFrame = ClusterMajorAxisLength * sin (-OriginalMajAxisOrientation* pi/180) * i/iSeedsWithinCluster;
                PointYFrame =  ClusterMajorAxisLength * cos (-OriginalMajAxisOrientation* pi/180)* i/iSeedsWithinCluster;
                PointX = round(PointXFrame + xTranslation);
                PointY = round(PointYFrame + yTranslation);

                if (OriginalMajAxisOrientation < 0)
                    MajAxisOrientation = 180 + OriginalMajAxisOrientation;
                else
                    MajAxisOrientation = OriginalMajAxisOrientation;
                end

                if ( MajAxisOrientation == 90)
                    A = 0;
                    B = 1;
                    C = -PointX;
                else
                    if (MajAxisOrientation == 0)
                        A = 1
                        B = 0;
                        C = -PointY;
                    else
                        if (MajAxisOrientation > 90)
                            MinAxisOrientation = MajAxisOrientation - 90;
                            slope = tan(MinAxisOrientation * pi/180);
                            A = slope;
                            B = 1;
                            C = -PointX - slope*PointY;
                        else
                            if(MajAxisOrientation < 90)
                                MinAxisOrientation = MajAxisOrientation + 90;
                                slope = tan(MinAxisOrientation * pi/180);
                                A = slope;
                                B = 1;
                                C = -PointX - slope*PointY;
                            end
                        end
                    end
                end

                for xIndex = xClusterRegionCorner:xClusterRegionCorner + xClusterRegionWidth-1
                    for yIndex = yClusterRegionCorner:yClusterRegionCorner + yClusterRegionWidth-1
                        res = A*yIndex + B*xIndex + C;
                        if (res >= -1 && res <= 1)
                            Kcluster(xIndex, yIndex) = 0;
                %           combine the single seed extraction from cluster to the
                %           initially segmented image, so that we get an image in which all
                %           the seeds are single seeds
                            KLab(xIndex, yIndex) = 0; 
                        end
                    end
                end
            end
        end

        % Kcluster now has cluster separated into individual seeds, therefore,
        % label this image & take only CGs out of it
        % figure, imshow(Kcluster);

        [KseparatedClusters, nSeparatedSeeds] = bwlabel(Kcluster, 8);
        SeparatedSeedStats = regionprops (KseparatedClusters, 'centroid');
        SeparatedSeedsCentroids = cat(1,SeparatedSeedStats.Centroid);

        % now the regions have the individual seeds separated, so in this region,
        % get the CGs of individual seeds & populate the final information dataset

        for iSeparatedSeedCount = 1: nSeparatedSeeds
            SepRegionCentroid = round(SeparatedSeedsCentroids(iSeparatedSeedCount,:));    
            ySepCG = SepRegionCentroid(1,1);
            xSepCG = SepRegionCentroid(1,2);
            FinalInformationDataSet = [FinalInformationDataSet; xSepCG ySepCG 0 0 1];   
        end
    end

    % take care of correct angle orientation w.r.t our coordinate system
    TotalNumOfSeeds = length(FinalInformationDataSet(:,1));

    % final data set: [CGx CGy Orientation OrientationValidity
    % PartOfCluster]
    for iSeedCount = 1: TotalNumOfSeeds

    %   first see if the orientation is valid  
        bOrientationValid = FinalInformationDataSet(iSeedCount, 4);

    %   get the current orientation  
        dCurrentOrientation = FinalInformationDataSet(iSeedCount, 3); 

        if (bOrientationValid == 1)
            if (dCurrentOrientation <= 0 )
                FinalInformationDataSet (iSeedCount, 3) = dCurrentOrientation + 90;
            else
                FinalInformationDataSet (iSeedCount, 3) = dCurrentOrientation - 90;
            end
        end

    end

else
    msgbox('No seeds found, try decreasing the threshold', 'Error seed segmentation', 'warn', 'modal');
end
% figure, imshow(KLab);

% display the final seed CGs on the original image
% final data set: [CGx CGy Orientation
% OrientationValidity PartOfCluster]


% $Log: BrachytherapySeedSegmentation.m,v $
% Revision 1.10  2007/12/03 21:48:18  anton
% BrachytherapySeedSegmentation.m: Typos in resRatio.
%
% Revision 1.9  2007/08/01 19:06:43  anton
% BrachytherapySeedSegmentation.m: Added support for resolution ratio to
% NTSC.  Also removed countless useless blank lines.
%
% Revision 1.8  2007/07/15 20:34:49  anton
% Misc. : Added new line at eof
%
% Revision 1.7  2006/08/14 20:10:32  anton
% BrachytherapySeedSegmentation.m: Shorter syntax to create morphological element
%
% Revision 1.6  2006/04/20 20:37:57  anton
% Segmentation: BrachytherapySeedSegmentation, lower case bwlabel to avoid
% Matlab warning.
%
% Revision 1.5  2005/12/23 00:51:17  jain
% The five files from Siddharth that I got are added here. Also I checked if these files
% are from which version - looks like cvs does not change the dates of the files and
% hence the confusion. This version is stable with bad cases, but output is still to be
% taken care of - uiwait will be added in the next version.
%
% Revision 1.3  2005/12/06 16:46:30  anton
% BrachytherapySeedSegmentation.m: Correction of seed orientation (+- 90).
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%
