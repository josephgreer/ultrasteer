% $Id: SeedSegmentationGUI.m,v 1.8 2007/08/01 19:08:28 anton Exp $

function varargout = SeedSegmentationGUI(varargin)
% SEEDSEGMENTATIONGUI M-file for SeedSegmentationGUI.fig
%      SEEDSEGMENTATIONGUI, by itself, creates a new SEEDSEGMENTATIONGUI or raises the existing
%      singleton*.
%
%      H = SEEDSEGMENTATIONGUI returns the handle to a new SEEDSEGMENTATIONGUI or the handle to
%      the existing singleton*.
%
%      SEEDSEGMENTATIONGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SEEDSEGMENTATIONGUI.M with the given input arguments.
%
%      SEEDSEGMENTATIONGUI('Property','Value',...) creates a new SEEDSEGMENTATIONGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before SeedSegmentationGUI_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to SeedSegmentationGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help SeedSegmentationGUI

% Last Modified by GUIDE v2.5 29-Dec-2006 12:15:01

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @SeedSegmentationGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @SeedSegmentationGUI_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
                              

if nargin & isstr(varargin{1})
   gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT





% --- Executes just before SeedSegmentationGUI is made visible.
function SeedSegmentationGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to SeedSegmentationGUI (see VARARGIN)

%Clearing the global variables used for output. This way values from the
%previous run will not be carried over. Added by Ameet.
    clear global SeedSegmentation;
    global SeedSegmentation;
    
    SeedSegmentation.Complete = false;
    SeedSegmentation.TaskIdentifier = 0;
    
    % get status from GUI and update view
    SeedSegmentation.SearchClosestPoint = get(handles.FindClosestPointCheck, 'Value');
    SeedSegmentation.UseUserThreshold = get(handles.UserThresholdCheck, 'Value');
    SeedSegmentation.UserThreshold = round(get(handles.Threshold,'Value'));
    set(handles.Threshold, 'Value', SeedSegmentation.UserThreshold);
    set(handles.ThresholdDisplay, 'String', strcat('Threshold: ', num2str(SeedSegmentation.UserThreshold)));  
    
    SeedSegmentation.ImageHandle = handles.ImageDisplay;
    
    % Choose default command line output for SeedSegmentationGUI
    handles.output = hObject;

    % Update handles structure
    guidata(hObject, handles);

    % UIWAIT makes SeedSegmentationGUI wait for user response (see UIRESUME)
    % uiwait(handles.SeedSegmentationGUI);

    % varargin: we pass opened image buffer as input to this GUI based function; 
    % open the file, display it
    [SeedSegmentation.I] = varargin{1};
    SeedSegmentation.DataSet = varargin{2};
    SeedSegmentation.ResRatioToNTSC = varargin{3};
    axes(handles.ImageDisplay);
    
    [rows,cols] = size(SeedSegmentation.I);
    SeedSegmentation.ROI.StartX = 1;
    SeedSegmentation.ROI.StartY = 1;
    SeedSegmentation.ROI.SizeX = rows - 1;
    SeedSegmentation.ROI.SizeY = cols - 1;
    DisplaySeedSegmentation;
    set(handles.SeedNumberText, 'String', strcat('Seeds: ', num2str(size(SeedSegmentation.DataSet, 1))));  

    guidata(hObject, handles);

    

% --- Outputs from this function are returned to the command line.
function varargout = SeedSegmentationGUI_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure

%Uiwait and changed output inserted by by Ameet
    global SeedSegmentation;
    
    uiwait(handles.output)

    varargout{1} = handles.output;
    varargout{2} = SeedSegmentation.Complete;
    if (SeedSegmentation.Complete == false)
        varargout{3} = [];
    else   
        varargout{3} = SeedSegmentation.DataSet;
    end

    clear global SeedSegmentation;
    


function UnToggleAllButtons(handles)
    global SeedSegmentation;
    SeedSegmentation.TaskIdentifier = 0;
    set(handles.AddSeedToggleButton, 'Value', 0);
    set(handles.AddSeedToggleButton, 'BackgroundColor', 'default');
    set(handles.DeleteSeedToggleButton, 'Value', 0);
    set(handles.DeleteSeedToggleButton, 'BackgroundColor', 'default');
    set(handles.UserAlertMsg, 'String', '');    
    set(gcf, 'pointer', 'arrow');


% //-----------------------------------------------------------------------------
% // function:	ZoomToggleButton_Callback() 
% // file:		SeedSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Select region button is clicked
%                   on the GUI, allows the user to select region of
%                   interest where the implanted seeds are, this function
%                   further calls seed segmentation routine to perform the
%                   segmentation, display routine to display the results,
%                   after completeion of segmentation also activates
%                   further buttons to enable the user to accept the
%                   segmentation results or make changes
% // input:	global variable I (image buffer)
% // output: global variables: 
% variables that identify the region of interest coordinates
%           global seedROIStartX; 
%           global seedROIStartY; 
%           global seedROIXWidth; 
%           global seedROIYWidth;
%           global SeedSegmentationROITranslationVector;
% variables in which the results will be put
%             global SeedSegmentationFinalInformationDataSet;
% variable which is a floating buffer for the results, which is modified
% when user performs Add or Delete seeds, all the changes are made to this
% buffer rather than original, it is copied onto original when user clicks
% Accept button
%             global FloatingSeedSegmentationFinalInformationDataSet;
% variable that identifies the current image handle
%             global CurrentImageHandle;
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // calling function: called when Select region button is pressed
% // NOTE: Now, the BrachytherapySeedSegmentation can be called
% either by using providing external threshold value for
% thersholding operation performed after morphological top hat opening
% operation to be 20 or let the function internally calculate a threshold value
% on its own automatically; the value 20 has been arrived at by experience of
% images; 
% //-----------------------------------------------------------------------------

% --- Executes on button press in ZoomToggleButton.
function ZoomToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to ZoomToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    % global variable required:
    global SeedSegmentation;
    
    % get the state and then untoggle everything
    button_state = get(hObject,'Value');
    UnToggleAllButtons(handles);

    if( button_state == get(hObject,'Max'))
    % toggle button is pressed
        % change the foreground color
        set(hObject, 'BackgroundColor', 'white');
        set(handles.UserAlertMsg, 'String', 'Zoom in: Select an area with the mouse');
        set(gcf, 'pointer', 'crosshair');
        
        % get  a rectangular region of interest
        rect = getrect(handles.ImageDisplay);
        % get the corners of the rectangle
        SeedSegmentation.ROI.StartX = round(rect(1,2));
        SeedSegmentation.ROI.StartY = round(rect(1,1));
        SeedSegmentation.ROI.SizeX = round(rect(1,4));
        SeedSegmentation.ROI.SizeY = round(rect(1,3));

        % display results only on region of interest, gives the zoom effect
        set(handles.UserAlertMsg, 'String', '');
        set(gcf, 'pointer', 'arrow');
        DisplaySeedSegmentation;
        
    elseif( button_state == get(hObject,'Min'))
        % toggle button is not pressed
        % change the foreground color
        set(hObject, 'BackgroundColor', 'default');
        
        [rows,cols] = size(SeedSegmentation.I);
        SeedSegmentation.ROI.StartX = 1;
        SeedSegmentation.ROI.StartY = 1;
        SeedSegmentation.ROI.SizeX = rows - 1;
        SeedSegmentation.ROI.SizeY = cols - 1;
        axes(handles.ImageDisplay);
        DisplaySeedSegmentation;
    end

    % Update handles structure
    guidata(hObject, handles);











% //-----------------------------------------------------------------------------
% // function:	AddSeedToggleButton_Callback() 
% // file:		SeedSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Add seed toggle button is clicked
%                   on the GUI; first determines the current state of the
%                   button & toggles it, if the resulting state is 'on',
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) then sets the value of variable iGlobalTask Identifier
%                   to 1 incdicating the current task is to add seeds
%                   wherever the user clicks until the state of the button
%                   is changed
% // input:	
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // output:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // calling function: called when AddSeed toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in AddSeedToggleButton.
function AddSeedToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to AddSeedToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of AddSeedToggleButton

    % global variable required:
    global SeedSegmentation;
    
    % get the state and then untoggle everything
    button_state = get(hObject,'Value');
    UnToggleAllButtons(handles);
    
    % manage the state/color of the add button
    if(button_state == get(hObject,'Max'))
        % toggle button is pressed
        % change the foreground color
        set(handles.AddSeedToggleButton, 'BackgroundColor', 'white');
        set(handles.AddSeedToggleButton, 'Value', get(hObject, 'Max'));
        SeedSegmentation.TaskIdentifier = 1;
        set(handles.UserAlertMsg, 'String', 'Add seed active: Click on image to add a seed');
        set(gcf, 'pointer', 'crosshair');
    end









% //-----------------------------------------------------------------------------
% // function:	DeleteSeedToggleButton_Callback() 
% // file:		SeedSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Delete seed toggle button is clicked
%                   on the GUI; first determines the current state of the
%                   button & toggles it, if the resulting state is 'on',
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) then sets the value of variable iGlobalTaskIdentifier
%                   to 2 incdicating the current task is to delete the
%                   nearest seed wherever the user clicks until the state of the button
%                   is changed
% // input:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // output: 
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // calling function: called when Delete seed toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in DeleteSeedToggleButton.
function DeleteSeedToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to DeleteSeedToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of DeleteSeedToggleButton
% global variable required:

    % seed segmentation related global variables
    global SeedSegmentation;
    
    % get the state and then untoggle everything
    button_state = get(hObject,'Value');
    UnToggleAllButtons(handles);
    
    % manage the state/color of the add button
    if( button_state == get(hObject,'Max'))
        % change the foreground color
        set(handles.DeleteSeedToggleButton, 'BackgroundColor', 'white');
        set(handles.DeleteSeedToggleButton, 'Value', get(hObject, 'Max'));
        SeedSegmentation.TaskIdentifier = 2;
        set(handles.UserAlertMsg, 'String', 'Delete seed active: Click on seed to remove');
        set(gcf, 'pointer', 'crosshair');
    end
    
    
    
% manage the state/color of the delete button
% give a user alert message of what he/she is goin to do

% start taking input in a while loop until delete or accept button is
% pressed or this task is interrupted in some manner






% --- Executes during object creation, after setting all properties.
function Threshold_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Threshold (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background, change
%       'usewhitebg' to 0 to use default.  See ISPC and COMPUTER.
usewhitebg = 1;
if usewhitebg
    set(hObject,'BackgroundColor',[.9 .9 .9]);
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



% //-----------------------------------------------------------------------------
% // function:	Threshold_Callback() 
% // file:		SeedSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Threshold slider is moved 
%                   on the GUI, performs the seed segmentation again with
%                   new value of threshold; allows for user threshold input
% // input:	global variable I (image buffer)
% // output: global variables: 
% variables that identify the region of interest coordinates
%           global seedROIStartX; 
%           global seedROIStartY; 
%           global seedROIXWidth; 
%           global seedROIYWidth;
%           global SeedSegmentationROITranslationVector;
% variables in which the results will be put
%             global SeedSegmentationFinalInformationDataSet;
% variable which is a floating buffer for the results, which is modified
% when user performs Add or Delete seeds, all the changes are made to this
% buffer rather than original, it is copied onto original when user clicks
% Accept button
%             global FloatingSeedSegmentationFinalInformationDataSet;
% variable that identifies the current image handle
%             global CurrentImageHandle;
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% 
% // calling function: called when Select region button is pressed
% // NOTE: This function allows for user threshold input 
% //-----------------------------------------------------------------------------
% --- Executes on slider movement.
% hObject    handle to Threshold (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
function Threshold_Callback(hObject, eventdata, handles)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider

% global variable required:
    global SeedSegmentation;
    UnToggleAllButtons(handles);
    SeedSegmentation.UserThreshold = round(get(hObject,'Value'));
    set(handles.Threshold, 'Value', SeedSegmentation.UserThreshold);
    set(handles.ThresholdDisplay, 'String', strcat('Threshold: ', num2str(SeedSegmentation.UserThreshold)));
    set(handles.UserThresholdCheck, 'Value', 1);
    SeedSegmentation.UseUserThreshold = get(handles.UserThresholdCheck, 'Value');
    guidata(hObject, handles);



% --- Executes on button press in CancelButton.
function CancelButton_Callback(hObject, eventdata, handles)
% hObject    handle to CancelButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global SeedSegmentation;
    UnToggleAllButtons(handles);
    SeedSegmentation.Complete = false;
    delete(handles.SeedSegmentationGUI);

    
% --- Executes during object deletion, before destroying properties.
function SeedSegmentationGUI_DeleteFcn(hObject, eventdata, handles)
% hObject    handle to SeedSegmentationGUI (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    


% --- Executes when user attempts to close SeedSegmentationGUI.
function SeedSegmentationGUI_CloseRequestFcn(hObject, eventdata, handles)
% hObject    handle to SeedSegmentationGUI (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: delete(hObject) closes the figure
    global SeedSegmentation;
    UnToggleAllButtons(handles);
    SeedSegmentation.Complete = false;
    delete(hObject);


% --- Executes on button press in DoneButton.
function DoneButton_Callback(hObject, eventdata, handles)
% hObject    handle to DoneButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% transform the results from Region Of Interest coordinate system to actual Image coordinate system 
% involves addition of translation vector before feeding the points to
% obtain/formulate final equations in image coordinate frame
% 1st the seeds locations
    global SeedSegmentation;
    UnToggleAllButtons(handles);
    SeedSegmentation.Complete = true;
    delete(handles.SeedSegmentationGUI);




% --- Executes on button press in DeleteAllSeedsButton.
function DeleteAllSeedsButton_Callback(hObject, eventdata, handles)
% hObject    handle to DeleteAllSeedsButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global SeedSegmentation;
    SeedSegmentation.DataSet = [];
    UnToggleAllButtons(handles);
    axes(handles.ImageDisplay);
    DisplaySeedSegmentation;
    set(handles.SeedNumberText, 'String', strcat('Seeds: ', num2str(size(SeedSegmentation.DataSet, 1))));  
    

% --- Executes on button press in FindClosestPointCheck.
function FindClosestPointCheck_Callback(hObject, eventdata, handles)
% hObject    handle to FindClosestPointCheck (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global SeedSegmentation;
    SeedSegmentation.SearchClosestPoint = get(hObject, 'Value');


% --- Executes during object creation, after setting all properties.
function FindClosestPointCheck_CreateFcn(hObject, eventdata, handles)
% hObject    handle to FindClosestPointCheck (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called%
    global SeedSegmentation;
    SeedSegmentation.SearchClosestPoint = get(hObject, 'Value');


% --- Executes on button press in UserThresholdCheck.
function UserThresholdCheck_Callback(hObject, eventdata, handles)
% hObject    handle to UserThresholdCheck (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global SeedSegmentation;
    SeedSegmentation.UseUserThreshold = get(hObject, 'Value');

    
% --- Executes on button press in AutoSegmentButton.
function AutoSegmentButton_Callback(hObject, eventdata, handles)
% hObject    handle to AutoSegmentButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    % global variable required:
    global SeedSegmentation;

    % show we are busy
    set(gcf,'Pointer','watch');
    drawnow;
    
    % perform segementation
    % final data set: [CGx CGy Orientation OrientationValidity PartOfCluster]
    SeedSegmentation.DataSet = [];
    UnToggleAllButtons(handles);
    warning off;
    [SeedSegmentation.DataSet ThresholdInUse]= BrachytherapySeedSegmentation(SeedSegmentation.I(SeedSegmentation.ROI.StartX:SeedSegmentation.ROI.StartX + SeedSegmentation.ROI.SizeX, ...
                                                                                                SeedSegmentation.ROI.StartY:SeedSegmentation.ROI.StartY + SeedSegmentation.ROI.SizeY), ...
                                                                             SeedSegmentation.UseUserThreshold, ...
                                                                             SeedSegmentation.UserThreshold, ...
                                                                             SeedSegmentation.ResRatioToNTSC);
    warning on;
    SeedSegmentation.DataSet(:,1) = SeedSegmentation.DataSet(:,1) + SeedSegmentation.ROI.StartX - 1;
    SeedSegmentation.DataSet(:,2) = SeedSegmentation.DataSet(:,2) + SeedSegmentation.ROI.StartY - 1;
    axes(handles.ImageDisplay);
    DisplaySeedSegmentation;
    set(handles.SeedNumberText, 'String', strcat('Seeds: ', num2str(size(SeedSegmentation.DataSet, 1))));
    
    if ~SeedSegmentation.UseUserThreshold
        SeedSegmentation.UserThreshold = round(ThresholdInUse);
        set(handles.Threshold, 'Value', SeedSegmentation.UserThreshold);
        set(handles.ThresholdDisplay, 'String', strcat('Threshold: ', num2str(SeedSegmentation.UserThreshold)));
    end
    
    % show we are done
    set(gcf,'Pointer','arrow');
    drawnow;
    
    guidata(hObject, handles);
    

    
    
 


% --- Executes on button press in RefineSegmentButton.
function RefineSegmentButton_Callback(hObject, eventdata, handles)
% hObject    handle to RefineSegmentButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    % show we are busy
    set(gcf,'Pointer','watch');
    drawnow;
    
    global SeedSegmentation;
    UnToggleAllButtons(handles);
    if (~isempty(SeedSegmentation.DataSet))
        for iCG = 1:length(SeedSegmentation.DataSet(:,1))
            if (SeedSegmentation.DataSet(iCG,5) ~= 1)
                CG = SeedSegmentation.DataSet(iCG,1:2);
                CG = LocateDarkestPoint(SeedSegmentation.I, CG, 1.5 * SeedSegmentation.ResRatioToNTSC);
                SeedSegmentation.DataSet(iCG,1:2) = CG;
            end
        end
        axes(handles.ImageDisplay);
        DisplaySeedSegmentation;
    end
    
    % show we are done
    set(gcf,'Pointer','arrow');
    drawnow;

    
    


% call back function for mouse clicks on image
function SeedSegmentationGUI_WindowButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to ImageDisplay (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global SeedSegmentation;
    
    CurrentAxesHandle = handles.ImageDisplay;
    CurrentPoint = [];
    CurrentPoint = get(CurrentAxesHandle, 'currentpoint');

    switch(SeedSegmentation.TaskIdentifier)
        case 1
    %       add a seed
            XToAdd = round(CurrentPoint (1,2)) + SeedSegmentation.ROI.StartX - 1;
            YToAdd = round(CurrentPoint (1,1)) + SeedSegmentation.ROI.StartY - 1;
            
            if (SeedSegmentation.SearchClosestPoint == 1)
                PointToAdd = LocateDarkestPoint(SeedSegmentation.I, [XToAdd YToAdd], 2.5 * SeedSegmentation.ResRatioToNTSC);
            else
                PointToAdd = [XToAdd YToAdd];
            end
            % final data set: [CGx CGy Orientation OrientationValidity PartOfCluster]
            SeedSegmentation.DataSet = [SeedSegmentation.DataSet; PointToAdd 0 0 0];
            DisplaySeedSegmentation;
            set(handles.SeedNumberText, 'String', strcat('Seeds: ', num2str(size(SeedSegmentation.DataSet, 1))));  

        case 2
    %       delete/remove a seed  
            XToDelete = round(CurrentPoint (1,2));
            YToDelete = round(CurrentPoint (1,1));
            
            % final data set: [CGx CGy Orientation OrientationValidity PartOfCluster]
            ToFindCG = [];
            ToFindCG = [XToDelete + SeedSegmentation.ROI.StartX - 1  YToDelete + SeedSegmentation.ROI.StartY - 1];
            iSeedIndex = FindSeed(SeedSegmentation.DataSet, ToFindCG);        
            if (iSeedIndex > 0)
                DelCG = SeedSegmentation.DataSet(iSeedIndex, 1:2);
                SeedSegmentation.DataSet(iSeedIndex, :) = [];
                DisplaySeedSegmentation;
                set(handles.SeedNumberText, 'String', strcat('Seeds: ', num2str(size(SeedSegmentation.DataSet, 1))));  
            else
                set(gcf, 'pointer', 'crosshair');
                msgbox('No seed in close vicinity found to delete, try again by clicking closer to the seed you wish to delete', 'Error deleting seed', 'warn', 'modal');    
            end
            set(gcf, 'pointer', 'crosshair');
        otherwise
            CurrentPoint
    end



% $Log: SeedSegmentationGUI.m,v $
% Revision 1.8  2007/08/01 19:08:28  anton
% SeedSegmentationGUI.m: Added support for resRatioToNTSC.  Added code
% to change cursor to watch when computing.
%
% Revision 1.7  2007/01/18 03:58:30  anton
% SeedSegmentationGUI.m: Refine only for single seeds, not clusters
%
% Revision 1.6  2007/01/10 19:00:49  anton
% SeedSegmentationGUI.m: Turn warning off when segmenting.  Much, much
% faster now.
%
% Revision 1.5  2007/01/10 05:15:26  anton
% SeedSegmentationGUI.m:  Minor bug related to pointer returning to crosshair
% if an error message is displayed.
%
% Revision 1.4  2007/01/03 22:23:32  anton
% SeedSegmentationGUI.m: Use hObject when possible.
%
% Revision 1.3  2007/01/02 21:47:23  anton
% SeedSegmentation: Swap definition of X and Y to make more sense
%
% Revision 1.2  2006/12/29 19:31:57  anton
% SeedSegmentation: Define search size for closest point.  Use whole image for closest point.
%
% Revision 1.1  2006/12/29 18:03:35  anton
% Segmentation: Added SeedSegmentationGUI for seeds only.  Better interface.
%
% Revision 1.16  2006/09/22 21:55:40  anton
% Segmentation: Added option to look for closest point or not when adding
% seeds by hand
%
% Revision 1.15  2006/04/21 14:48:36  anton
% Segmentation GUI: Removed use of rufMainGUIData in SeedAndSegmentation
% GUI to keep the segmentation usable as stand-alone.
%
% Revision 1.14  2006/04/20 20:43:16  anton
% Segmentation: SeedandFiducialSegementationGUI, major update:
% *: Added buttons "abort" and "accept both"
% *: Change logic so that one can segment in any order (seeds/fiducial)
% *: Factor some code to control button state (enable/disable)
% *: Added flags to check that both seeds and fiducial are segmented
%
% Revision 1.13  2006/04/18 14:58:38  anton
% Segmentation: Added "Abort" button.  Work in progress.
%
% Revision 1.12  2005/12/28 02:46:30  jain
% Some more error checking before returning from the GUI. Added a global flag to
% check if the GUI was killed or or not. Returning this flag and the handle too.
% Clearning global variables after returning.
%
% Revision 1.11  2005/12/23 00:51:17  jain
% The five files from Siddharth that I got are added here. Also I checked if these files
% are from which version - looks like cvs does not change the dates of the files and
% hence the confusion. This version is stable with bad cases, but output is still to be
% taken care of - uiwait will be added in the next version.
%
% Revision 1.8  2005/11/28 19:45:14  anton
% General Update from Siddharth:
% *: Handling failure cases in fiducial segmentation, when only partial region
%    is selected as region of interest for fiducial...however, note that the
%    region should not be too small
% *: Now the BB numbers, line numbers and ellipse numbers are displayed after
%    establishing correspondence to have a visual check for user that
%    correspondence established is correct; Note tht these are displayed only
%    on whole image, not zoomed up region of interest, because there user can
%    make changes....so just on whole image after segmentation & after user
%    says he accepts the results
% *: Few cases prev. unhandled in user intervention in fiducial segmentation
%    validation
%
% Revision 1.7  2005/11/08 00:47:51  svikal
% BB correspondence after edit/user changes
%
% Revision 1.6  2005/11/07 00:05:11  svikal
% integrating with FTRAC;output of segmentation in desired format of FTRAC
%
% Revision 1.5  2005/11/05 22:56:12  svikal
% improved ellipse detection; changed the name of the function of fitting ellipse; hence file changed; also comments
%
% Revision 1.4  2005/11/03 15:54:16  svikal
% changes made for establishing correspondence of BBs
%
% Revision 1.3  2005/10/31 19:39:29  svikal
% few comments and ellipse final equ.but still commented
%
% Revision 1.2  2005/10/27 18:28:51  svikal
% Added CVS tags
%


