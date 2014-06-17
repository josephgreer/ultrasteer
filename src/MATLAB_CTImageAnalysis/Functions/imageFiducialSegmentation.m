function [] = imageFiducialSegmentation(start, imageDirectory, ftracDirectory) 


files = dir([imageDirectory '/test*']);
files = files(find(cellfun('length',{files(:).name})==12));
for i = start:length(files)
    imageName = getfield(files(i),'name');
    image = imread([imageDirectory '/' imageName],'tiff');
    imageName = imageName(1:end-5);
    image = im2double(image);
    xySize = size(image);
    
    % look at the size to deduct the resolution.  This is a last minute
    % hack.  By default, we have been using NTSC videos, ie. 480 x 480
    % usable field of view.  The resolution ratio is compared to NTSC.
    resRatioToNTSC = xySize(1) / 480;
    
    bbsFile = strcat(ftracDirectory, '/BBs_', imageName, '.txt');  
    if (length(dir(bbsFile)) == 1)
        bbs = load('-ascii', bbsFile);
    else
        bbs = [];
    end
    line1File = strcat(ftracDirectory, '/Line1_', imageName, '.txt');  
    if (length(dir(line1File)) == 1)
        line1 = load('-ascii', line1File);
    else
        line1 = [];
    end
    line2File = strcat(ftracDirectory, '/Line2_', imageName, '.txt');  
    if (length(dir(line2File)) == 1)
        line2 = load('-ascii', line2File);
    else
        line2 = [];
    end
    line3File = strcat(ftracDirectory, '/Line3_', imageName, '.txt');  
    if (length(dir(line3File)) == 1)
        line3 = load('-ascii', line3File);
    else
        line3 = [];
    end
    ellipse1File = strcat(ftracDirectory, '/Ellipse1_', imageName, '.txt');  
    if (length(dir(ellipse1File)) == 1)
        ellipse1 = load('-ascii', ellipse1File);
    else
        ellipse1 = [];
    end
    ellipse2File = strcat(ftracDirectory, '/Ellipse2_', imageName, '.txt');  
    if (length(dir(ellipse2File)) == 1)
        ellipse2 = load('-ascii', ellipse2File);
    else
        ellipse2 = [];
    end
    
    [handle SegmentationCompleted bbs line1 line2 line3 ellipse1 ellipse2] = FiducialSegmentationGUI(image, bbs, ...
                                                                                                     line1, line2, line3, ...
                                                                                                     ellipse1, ellipse2, ...
                                                                                                     resRatioToNTSC);
         
    % save the segmentation results
    save(bbsFile, 'bbs', '-ascii' );
    save(line1File, 'line1', '-ascii' );
    save(line2File, 'line2', '-ascii' );
    save(line3File, 'line3', '-ascii' );
    save(ellipse1File, 'ellipse1', '-ascii' );
    save(ellipse2File, 'ellipse2', '-ascii' );

        % refresh view
        %rufSegmentationSelectionThumbnails(handles); %?????????
      end 
 
end 
  
    
    
    
   
  